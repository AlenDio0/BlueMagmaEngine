#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
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

			uniform sampler2D uTexture;
			uniform bool uHasTexture;

			void main() {
				vec2 pixelCoord = gl_TexCoord[0].xy - 0.5;
				vec2 center = uSize * 0.5;

				vec2 fullSize = uSize + 2.0;
				vec2 cornerOffset = abs(pixelCoord * fullSize) - center + uCorner;
				
				float distance = length(max(cornerOffset, 0.0)) + min(max(cornerOffset.x, cornerOffset.y), 0.0) - uCorner;

				float alpha = 1.0 - smoothstep(0.0, 1.0, distance);
				if (alpha <= 0.0)
					discard;

				vec4 finalColor = uHasTexture ? texture2D(uTexture, gl_TexCoord[0].xy) * gl_Color : gl_Color;
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

			uniform sampler2D uTexture;
			uniform bool uHasTexture;

			void main() {
				vec2 pixelCoord = gl_TexCoord[0].xy - 0.5;

				float fullRadius = uRadius + 1.0;
				float distance = (length(pixelCoord) * fullRadius * 2.0) - uRadius;

				float alpha = 1.0 - smoothstep(0.0, 1.0, distance);
				if (alpha <= 0.0)
					discard;

				vec4 finalColor = uHasTexture ? texture2D(uTexture, gl_TexCoord[0].xy) * gl_Color : gl_Color;
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

	static inline sf::Text CreateText(const TextRender& textRender) noexcept
	{
		static sf::Text sText{ Font::GetDefault() };

		if (!textRender.FontPtr)
			return sf::Text{ Font::GetDefault() };

		sText.setFont(*textRender.FontPtr);
		sText.setString(textRender.Text);
		sText.setCharacterSize(textRender.CharSize);

		return sText;
	}

	static inline std::array<sf::Vertex, 6> BuildQuad(const Transform& transform, sf::Color color, Vec2f size, RectFloat coords) noexcept
	{
		const auto cMatrix = RenderSystem::GetRenderStates(transform, size).transform;
		const Vec2f cMin = coords.Min(), cMax = coords.Max();
		return { {
			{ cMatrix.transformPoint(Vec2f(0.f)),			color, cMin },
			{ cMatrix.transformPoint(Vec2f(size.X, 0.f)),	color, {cMax.X, cMin.Y} },
			{ cMatrix.transformPoint(size),					color, cMax },
			{ cMatrix.transformPoint(Vec2f(0.f)),			color, cMin },
			{ cMatrix.transformPoint(size),					color, cMax },
			{ cMatrix.transformPoint(Vec2f(0.f, size.Y)),	color, {cMin.X, cMax.Y} }
			} };
	}

	static inline void PopulateRenderCommand(Entity entity, RenderCommand& command) noexcept
	{
		if (auto* outline = entity.TryGet<Outline>())
			command.Outline = *outline;

		if (auto* color = entity.TryGet<ColorMaterial>())
			command.Material.Color = color->Color;

		if (auto* texture = entity.TryGet<TextureMaterial>())
		{
			command.Material.TexturePtr = texture->TexturePtr;
			command.Material.TextureCoords = texture->TextureRect.value_or(RectInt({ 0, 0 }, texture->TexturePtr->getSize()));
		}
	}

	static inline void BuildText(const Transform& transform, const TextRender& textRender, RenderCommand& outCommand) noexcept
	{
		sf::Text text = CreateText(textRender);
		text.setFillColor(outCommand.Material.Color);
		text.setOutlineThickness(outCommand.Outline.Thickness);
		text.setOutlineColor(outCommand.Outline.Color);

		const RectFloat cBounds = text.getLocalBounds();
		outCommand.Shape = RenderCommand::TextData{
			.TextCopy = std::move(text),
			.Matrix = RenderSystem::GetRenderStates(transform, cBounds.Size, cBounds.Position).transform
		};
	}

	static inline void BuildSprite(const Transform& transform, const SpriteShape& sprite, Vec2f size, RenderCommand& outCommand) noexcept
	{
		const auto& texture = sprite.TexturePtr;
		if (!texture)
			return;

		const RectInt cCoords = sprite.TextureRect.value_or(RectInt({ 0, 0 }, texture->getSize()));
		outCommand.Shape = RenderCommand::SpriteData{ .TexturePtr = texture, .TextureCoords = cCoords };

		outCommand.Quad = BuildQuad(transform, outCommand.Material.Color, size, cCoords);
	}

	template<typename TRenderComp>
	static inline RenderCommand BuildRenderCommand(Entity entity, const Transform& transform, const TRenderComp& render, Vec2f size) noexcept
	{
		RenderCommand command{ .Z = transform.Global.Z };
		PopulateRenderCommand(entity, command);

		if constexpr (std::is_same_v<TRenderComp, TextRender>)
		{
			BuildText(transform, render, command);
			return command;
		}
		if constexpr (std::is_same_v<TRenderComp, SpriteShape>)
		{
			BuildSprite(transform, render, size, command);
			return command;
		}

		if constexpr (std::is_same_v<TRenderComp, RectShape>)
			command.Shape = RectShape{ .Size = size, .Corner = render.Corner };
		else if constexpr (std::is_same_v<TRenderComp, CircleShape>)
			command.Shape = CircleShape{ .Radius = render.Radius };

		command.Quad = BuildQuad(transform, command.Material.Color, size, command.Material.TextureCoords);

		return command;
	}

	template<typename TRenderComp>
	static inline Vec2f GetRenderSize(const TRenderComp& render) noexcept
	{
		if constexpr (std::is_same_v<TRenderComp, RectShape>)
			return render.Size;
		else if constexpr (std::is_same_v<TRenderComp, CircleShape>)
			return Vec2f(render.Radius * 2.f);
		else if constexpr (std::is_same_v<TRenderComp, SpriteShape>)
			return render.TextureRect.value_or(RectInt(Vec2i(0.f), render.TexturePtr->getSize())).Size;
		else if constexpr (std::is_same_v<TRenderComp, TextRender>)
			return CreateText(render).getLocalBounds().size;

		return Vec2f::Zero();
	}

	template<typename TRenderComp>
	static inline void CollectRender(Scene& scene, RectFloat cameraBounds, std::vector<RenderCommand>& outCommands) noexcept
	{
		auto view = scene.View<Transform, TRenderComp>();
		for (const auto& [entity, transform, render] : view.each())
		{
			auto hidden = scene.TryGetComponent<Hidden>(entity);
			if (hidden && !hidden->Visible)
				continue;

			const Vec2f cSize = GetRenderSize<TRenderComp>(render);
			if (!RenderSystem::IsInCameraBounds(cameraBounds, transform, cSize))
				continue;

			outCommands.push_back(BuildRenderCommand(scene.GetEntity(entity), transform, render, cSize));
		}
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

		CollectRender<RectShape>(scene, cCameraBounds, sRenderCommands);
		CollectRender<CircleShape>(scene, cCameraBounds, sRenderCommands);
		CollectRender<SpriteShape>(scene, cCameraBounds, sRenderCommands);
		CollectRender<TextRender>(scene, cCameraBounds, sRenderCommands);

		std::ranges::stable_sort(sRenderCommands, [](const auto& left, const auto& right) {
			return left.Z < right.Z; });

		DrawRenderCommands(*renderer, sRenderCommands);
	}

	void RenderSystem::DrawRenderCommands(Renderer& renderer, const std::vector<RenderCommand>& commands) noexcept
	{
		static std::vector<sf::Vertex> sBatch;
		sBatch.clear();

		const RenderCommand* commandKey = nullptr;
		for (const auto& command : commands)
		{
			if (command.IsShape<RenderCommand::TextData>())
			{
				FlushBatch(renderer, sBatch, commandKey);

				const RenderCommand::TextData& textData = command.GetShape<RenderCommand::TextData>();
				sf::RenderStates states;
				states.blendMode = sf::BlendAlpha;
				states.transform = textData.Matrix;

				renderer.Draw(textData.TextCopy, states);
				continue;
			}

			if (commandKey && !HasSameUniform(command, *commandKey))
				FlushBatch(renderer, sBatch, commandKey);

			if (!commandKey)
				commandKey = &command;

			sBatch.insert(sBatch.end(), command.Quad.begin(), command.Quad.end());
		}
		FlushBatch(renderer, sBatch, commandKey);
	}

	void RenderSystem::FlushBatch(Renderer& renderer, std::vector<sf::Vertex>& batch, const RenderCommand*& keyPtr) noexcept
	{
		if (batch.empty() || !keyPtr)
			return;

		sf::RenderStates states;
		states.blendMode = sf::BlendAlpha;
		states.shader = GetRenderShader(*keyPtr);

		if (keyPtr->IsShape<RenderCommand::SpriteData>())
			states.texture = keyPtr->GetShape<RenderCommand::SpriteData>().TexturePtr;
		else if (keyPtr->Material.TexturePtr)
			states.texture = keyPtr->Material.TexturePtr;

		renderer.Draw(batch.data(), batch.size(), sf::PrimitiveType::Triangles, states);

		batch.clear();
		keyPtr = nullptr;
	}

	bool RenderSystem::HasSameUniform(const RenderCommand& left, const RenderCommand& right) noexcept
	{
		if (left.Shape.index() != right.Shape.index())
			return false;

		if (left.Material.TexturePtr != right.Material.TexturePtr)
			return false;

		if (left.Outline.Thickness != right.Outline.Thickness)
			return false;
		if (left.Outline.Color != right.Outline.Color)
			return false;

		return std::visit([](const auto& left, const auto& right) {
			using TLeft = std::decay_t<decltype(left)>;
			using TRight = std::decay_t<decltype(right)>;

			if constexpr (!std::is_same_v<TLeft, TRight>)
				return false;
			else if constexpr (std::is_same_v<TLeft, RectShape>)
				return left.Size == right.Size && left.Corner == right.Corner;
			else if constexpr (std::is_same_v<TLeft, CircleShape>)
				return left.Radius == right.Radius;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::SpriteData>)
				return left.TexturePtr == right.TexturePtr;
			else if constexpr (std::is_same_v<TLeft, RenderCommand::TextData>)
				return false;

			return false;
			}, left.Shape, right.Shape);
	}

	bool RenderSystem::IsInCameraBounds(RectFloat cameraBounds, const Transform& transform, Vec2f size) noexcept
	{
		const auto& [position, scale, rotation, z] = transform.Global;
		const auto& origin = transform.Local.State.Origin;

		const RectFloat cEntityBounds{ Vec2f(0.f), size };
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
		return std::visit([&](const auto& shape) -> sf::Shader* {
			using TShape = std::decay_t<decltype(shape)>;

			const bool cHasTexture = command.Material.TexturePtr != nullptr;
			if constexpr (std::is_same_v<TShape, RectShape>) {
				auto& shader = Shader::s_RectShader;
				shader.setUniform("uSize", sf::Glsl::Vec2(shape.Size));
				shader.setUniform("uCorner", shape.Corner);
				shader.setUniform("uOutline", command.Outline.Thickness);
				shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
				shader.setUniform("uHasTexture", cHasTexture);
				return &shader;
			}
			else if constexpr (std::is_same_v<TShape, CircleShape>) {
				auto& shader = Shader::s_CircleShader;
				shader.setUniform("uRadius", shape.Radius);
				shader.setUniform("uOutline", command.Outline.Thickness);
				shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
				shader.setUniform("uHasTexture", cHasTexture);
				return &shader;
			}

			return nullptr;
			}, command.Shape);
	}
}
