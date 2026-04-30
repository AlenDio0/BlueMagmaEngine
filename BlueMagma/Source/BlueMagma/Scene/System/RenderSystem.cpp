#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Core/Transform2D.hpp"

namespace BM
{
	namespace Shader
	{
		static constexpr const char* s_RectFrag = R"(
			uniform vec2 uSize;
			uniform float uCorner;
			uniform float uOutline;
			uniform vec4 uOutlineColor;

			void main() {
				vec2 pixelCoord = gl_TexCoord[0].xy - 0.5;
				vec2 center = uSize * 0.5;

				vec2 fullSize = uSize + 2.0;
				vec2 cornerOffset = abs(pixelCoord * fullSize) - center + uCorner;
				
				float distance = length(max(cornerOffset, 0.0)) + min(max(cornerOffset.x, cornerOffset.y), 0.0) - uCorner;

				float alpha = 1.0 - smoothstep(0.0, 1.0, distance);
				if (alpha <= 0.0)
					discard;

				vec4 finalColor = gl_Color;
				if (uOutline > 0.0)
				{
					float outlineFactor = smoothstep(-uOutline - 1.0, -uOutline, distance);
					finalColor = mix(gl_Color, uOutlineColor, outlineFactor);
				}

				gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
			})";
		static constexpr const char* s_CircleFrag = R"(
			uniform float uRadius;
			uniform float uOutline;
			uniform vec4 uOutlineColor;

			void main() {
				vec2 pixelCoord = gl_TexCoord[0].xy - 0.5;

				float fullRadius = uRadius + 1.0;
				float distance = (length(pixelCoord) * fullRadius * 2.0) - uRadius;

				float alpha = 1.0 - smoothstep(0.0, 1.0, distance);
				if (alpha <= 0.0)
					discard;

				vec4 finalColor = gl_Color;
				if (uOutline > 0.0)
				{
					float outlineFactor = smoothstep(-uOutline - 1.0, -uOutline, distance);
					finalColor = mix(gl_Color, uOutlineColor, outlineFactor);
				}

				gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
			})";

		static inline sf::Shader s_RectShader{ std::string_view(s_RectFrag), sf::Shader::Type::Fragment };
		static inline sf::Shader s_CircleShader{ std::string_view(s_CircleFrag), sf::Shader::Type::Fragment };
	}

	using namespace Component;

	static inline sf::Text CreateText(const TextRender& textRender, Style style) noexcept
	{
		sf::Text text{ Font::GetDefault() };

		if (!textRender.FontPtr)
			return text;

		text.setFont(*textRender.FontPtr);
		text.setString(textRender.Text);
		text.setCharacterSize(textRender.CharSize);
		text.setOutlineThickness(style.Outline);
		text.setFillColor(style.FillColor);
		text.setOutlineColor(style.OutlineColor);

		return text;
	}

	static inline RenderCommand BuildRenderCommand(const Transform& transform, auto& render, Vec2f size, const Style& style) noexcept
	{
		using TRenderComp = std::decay_t<decltype(render)>;

		const auto cColor = style.FillColor;
		RenderCommand command{
			.Z = transform.Global.Z,
			.Outline = std::abs(style.Outline),
			.OutlineColor = style.OutlineColor
		};

		Vec2f offset{ 0.f };
		Vec2f min{ 0.f }, max{ 1.f };

		if constexpr (std::is_same_v<TRenderComp, RectRender>)
		{
			command.Data = RenderCommand::RectData{
				.Size = size, .Corner = render.Corner
			};
		}
		else if constexpr (std::is_same_v<TRenderComp, CircleRender>)
		{
			command.Data = RenderCommand::CircleData{
				.Radius = render.Radius
			};
		}
		else if constexpr (std::is_same_v<TRenderComp, TextureRender>)
		{
			command.Data = RenderCommand::TextureData{
				.Ptr = render.TexturePtr
			};

			const RectInt cTextureRect = render.TextureRect ? render.TextureRect.value() : RectInt({ 0, 0 }, size);
			offset = cTextureRect.Position;
			min = cTextureRect.Min();
			max = cTextureRect.Max();

			return command;
		}
		else if constexpr (std::is_same_v<TRenderComp, TextRender>)
		{
			sf::Text text = CreateText(render, style);
			const RectFloat cBounds = text.getLocalBounds();

			command.Data = RenderCommand::TextData{
				.TextCopy = std::move(text),
				.Matrix = RenderSystem::GetRenderStates(transform, cBounds.Size, cBounds.Position).transform
			};

			return command;
		}

		const auto cMatrix = RenderSystem::GetRenderStates(transform, size, offset).transform;
		command.Quad[0] = { cMatrix.transformPoint(Vec2f(0.f)),			cColor,	min };
		command.Quad[1] = { cMatrix.transformPoint(Vec2f(size.X, 0.f)),	cColor,	{max.X, min.Y} };
		command.Quad[2] = { cMatrix.transformPoint(size),				cColor,	max };
		command.Quad[3] = { cMatrix.transformPoint(Vec2f(0.f)),			cColor, min };
		command.Quad[4] = { cMatrix.transformPoint(size),				cColor,	max };
		command.Quad[5] = { cMatrix.transformPoint(Vec2f(0.f, size.Y)),	cColor, {min.X, max.Y} };

		return command;
	}

	template<typename TRenderComp>
	static inline Vec2f GetRenderSize(const TRenderComp& render) noexcept
	{
		if constexpr (std::is_same_v<TRenderComp, RectRender>)
			return render.Size;
		else if constexpr (std::is_same_v<TRenderComp, CircleRender>)
			return Vec2f(render.Radius * 2.f);
		else if constexpr (std::is_same_v<TRenderComp, TextureRender>)
			return Vec2f(render.TexturePtr->getSize());
		else if constexpr (std::is_same_v<TRenderComp, TextRender>)
		{
			sf::Text text = CreateText(render, {});
			return text.getLocalBounds().size;
		}
	}

	template<typename TRenderComp>
	static inline void CollectRenderCommands(Scene& scene, RectFloat cameraBounds, std::vector<RenderCommand>& commands) noexcept
	{
		scene.View<Transform, TRenderComp>().each([&](auto entity, const auto& transform, const auto& render) {
			auto hidden = scene.TryGetComponent<Hidden>(entity);
			if (hidden && !hidden->Visible)
				return;

			const Vec2f cSize = GetRenderSize<TRenderComp>(render);
			if (!RenderSystem::IsInCameraBounds(cameraBounds, transform, cSize))
				return;

			Style style{};
			if (auto eStyle = scene.TryGetComponent<Style>(entity))
				style = *eStyle;

			commands.push_back(BuildRenderCommand(transform, render, cSize, style));
			});
	}

	void RenderSystem::OnRender(Scene& scene) const noexcept
	{
		Renderer* renderer = scene.GetRenderer();
		if (!renderer)
			return;
		const RectFloat cCameraBounds = renderer->GetCamera().GetBounds();

		static std::vector<RenderCommand> sRenderCommands;
		sRenderCommands.clear();
		sRenderCommands.reserve(scene.View<Transform>().size());

		CollectRenderCommands<RectRender>(scene, cCameraBounds, sRenderCommands);
		CollectRenderCommands<CircleRender>(scene, cCameraBounds, sRenderCommands);
		CollectRenderCommands<TextureRender>(scene, cCameraBounds, sRenderCommands);
		CollectRenderCommands<TextRender>(scene, cCameraBounds, sRenderCommands);

		std::ranges::stable_sort(sRenderCommands, [](const auto& left, const auto& right) {
			return left.Z < right.Z; });

		DrawRenderCommands(*renderer, sRenderCommands);
	}

	void RenderSystem::DrawRenderCommands(Renderer& renderer, const std::vector<RenderCommand>& commands) noexcept
	{
		static std::vector<sf::Vertex> sBatch;
		sBatch.clear();

		const RenderCommand* batchKey = nullptr;
		for (const auto& command : commands)
		{
			if (command.Is<RenderCommand::TextData>())
			{
				FlushBatch(renderer, sBatch, batchKey);

				const auto& textData = command.Get<RenderCommand::TextData>();
				sf::RenderStates states;
				states.blendMode = sf::BlendAlpha;
				states.transform = textData.Matrix;

				renderer.Draw(textData.TextCopy, states);
				continue;
			}

			if (batchKey && !HasSameUniform(command, *batchKey))
				FlushBatch(renderer, sBatch, batchKey);

			if (!batchKey)
				batchKey = &command;

			sBatch.append_range(command.Quad);
		}
		FlushBatch(renderer, sBatch, batchKey);
	}

	void RenderSystem::FlushBatch(Renderer& renderer, std::vector<sf::Vertex>& batch, const RenderCommand*& keyPtr) noexcept
	{
		if (batch.empty() || !keyPtr)
			return;

		sf::RenderStates states;
		states.blendMode = sf::BlendAlpha;
		states.shader = GetRenderShader(*keyPtr);
		if (keyPtr->Is<RenderCommand::TextureData>())
			states.texture = keyPtr->Get<RenderCommand::TextureData>().Ptr;

		renderer.Draw(batch.data(), batch.size(), sf::PrimitiveType::Triangles, states);

		batch.clear();
		keyPtr = nullptr;
	}

	bool RenderSystem::HasSameUniform(const RenderCommand& left, const RenderCommand& right) noexcept
	{
		if (left.Outline != right.Outline)
			return false;
		if (left.OutlineColor != right.OutlineColor)
			return false;

		return std::visit([](const auto& left, const auto& right) {
			using TLeft = std::decay_t<decltype(left)>;
			using TRight = std::decay_t<decltype(right)>;

			if constexpr (!std::is_same_v<TLeft, TRight>)
				return false;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::RectData>)
				return left.Size == right.Size && left.Corner == right.Corner;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::CircleData>)
				return left.Radius == right.Radius;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::TextureData>)
				return left.Ptr == right.Ptr;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::TextData>)
				return false;

			return false;
			}, left.Data, right.Data);
	}

	bool RenderSystem::IsInCameraBounds(RectFloat cameraBounds, const Transform& transform, Vec2f size) noexcept
	{
		const auto& [position, scale, rotation, z] = transform.Global;
		const auto& origin = transform.Local.State.Origin;

		const RectFloat cEntityBounds{ {0.f}, size };
		const auto cMatrix = Transform2D::ToMatrix({ position, scale, origin, rotation }, cEntityBounds);
		const RectFloat cLocalCameraBounds = cMatrix.getInverse().transformRect(cameraBounds);

		return cLocalCameraBounds.Intersects(cEntityBounds);
	}

	sf::RenderStates RenderSystem::GetRenderStates(const Transform& transform, Vec2f size, Vec2f offset) noexcept
	{
		const auto& [position, scale, rotation, z] = transform.Global;
		const auto& origin = transform.Local.State.Origin;

		return sf::RenderStates{ Transform2D::ToMatrix({position, scale, origin, rotation}, {offset, size}) };
	}

	sf::Shader* RenderSystem::GetRenderShader(const RenderCommand& command) noexcept
	{
		return std::visit([&](const auto& data) -> sf::Shader* {
			using TData = std::decay_t<decltype(data)>;

			if constexpr (std::is_same_v<TData, RenderCommand::RectData>) {
				auto& shader = Shader::s_RectShader;
				shader.setUniform("uSize", sf::Glsl::Vec2(data.Size));
				shader.setUniform("uCorner", data.Corner);
				shader.setUniform("uOutline", command.Outline);
				shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.OutlineColor));
				return &shader;
			}
			else if constexpr (std::is_same_v<TData, RenderCommand::CircleData>) {
				auto& shader = Shader::s_CircleShader;
				shader.setUniform("uRadius", data.Radius);
				shader.setUniform("uOutline", command.Outline);
				shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.OutlineColor));
				return &shader;
			}

			return nullptr;
			}, command.Data);
	}
}
