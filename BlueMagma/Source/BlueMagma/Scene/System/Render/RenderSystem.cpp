#include "bmpch.hpp"
#include "RenderSystem.hpp"

#include "Shader/RectFrag.hpp"
#include "Shader/RectTexturedFrag.hpp"
#include "Shader/CircleFrag.hpp"
#include "Shader/CircleTexturedFrag.hpp"

#include "Math/Transform2D.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"

namespace BM
{
	static inline sf::Shader s_RectShader{ std::string_view(Shader::s_RectFrag), sf::Shader::Type::Fragment };
	static inline sf::Shader s_RectTexturedShader{ std::string_view(Shader::s_RectTexturedFrag), sf::Shader::Type::Fragment };
	static inline sf::Shader s_CircleShader{ std::string_view(Shader::s_CircleFrag), sf::Shader::Type::Fragment };
	static inline sf::Shader s_CircleTexturedShader{ std::string_view(Shader::s_CircleTexturedFrag), sf::Shader::Type::Fragment };

	//======================================================================================

	using namespace Component;

	static inline void UpdateTextCache(const TextRender& textRender) noexcept {
		auto& [fontPtr, text, size, lastFontPtr, lastText, lastSize, cachedText, cachedBounds] = textRender;

		const bool cFontUpdated = fontPtr != lastFontPtr;
		const bool cSizeUpdated = size != lastSize;
		const bool cTextUpdated = text != lastText;

		if (cFontUpdated)
		{
			lastFontPtr = fontPtr;
			cachedText.setFont(fontPtr ? *fontPtr : Font::GetDefault());
		}
		if (cSizeUpdated)
		{
			lastSize = size;
			cachedText.setCharacterSize(size);
		}
		if (cTextUpdated)
			lastText = text;

		if (cFontUpdated || cSizeUpdated || cTextUpdated)
		{
			std::string finalFixedText;
			{
				const std::string_view cFixedText = "|${";
				const size_t cEndlineCount = std::ranges::count(text, '\n');

				finalFixedText.reserve(cFixedText.size() + cEndlineCount);
				finalFixedText.append(cFixedText).append(cEndlineCount, '\n');
			}

			cachedText.setString(finalFixedText);
			const RectFloat cFixedBounds = cachedText.getGlobalBounds();

			if (cTextUpdated)
				cachedText.setString(text);

			cachedBounds = cachedText.getGlobalBounds();
			cachedBounds.Y = cFixedBounds.Y;
			cachedBounds.Height = cFixedBounds.Height;
		}
	}

	[[nodiscard]] static inline sf::Text& GetCachedText(const TextRender& textRender) noexcept {
		UpdateTextCache(textRender);
		return textRender.CachedText;
	}

	[[nodiscard]] static inline RectFloat GetCachedTextBounds(const TextRender& textRender) noexcept {
		UpdateTextCache(textRender);
		return textRender.CachedBounds;
	}

	[[nodiscard]] static inline std::array<sf::Vertex, 6> BuildQuad(const Transform& transform, Color color, Vec2f size, RectFloat coords) noexcept {
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

	static inline void PopulateRenderCommand(Entity entity, RenderCommand& command) noexcept {
		if (auto* outline = entity.TryGet<Outline>())
			command.Outline = *outline;

		if (auto* color = entity.TryGet<ColorMaterial>())
			command.Material.Color = color->Color;

		if (auto* texture = entity.TryGet<TextureMaterial>())
		{
			command.Material.TexturePtr = texture->TexturePtr;
			command.Material.TextureCoords = texture->TextureRect.value_or(
				RectInt(Vec2i(0), texture->TexturePtr ? Vec2i(texture->TexturePtr->getSize()) : Vec2i(0)));
		}
	}

	//======================================================================================

	static inline void BuildQuadCommand(const Transform& transform, Vec2f size, RenderCommand& outCommand) noexcept {
		const RectFloat coords = outCommand.Material.TexturePtr
			? outCommand.Material.TextureCoords
			: RectFloat(Vec2f(0.f), Vec2f(1.f));

		outCommand.Quad = BuildQuad(transform, outCommand.Material.Color, size, coords);
	}

	static inline void BuildRect(const Transform& transform, const RectShape& rect, Vec2f size, RenderCommand& outCommand) noexcept {
		outCommand.Shape = RectShape{ .Size = size, .Corner = rect.Corner };
		outCommand.Shader = !outCommand.Material.TexturePtr ? RenderCommand::ShaderType::Rect :
			RenderCommand::ShaderType::RectTextured;

		BuildQuadCommand(transform, size, outCommand);
	}

	static inline void BuildCircle(const Transform& transform, const CircleShape& circle, Vec2f size, RenderCommand& outCommand) noexcept {
		outCommand.Shape = CircleShape{ .Radius = circle.Radius };
		outCommand.Shader = !outCommand.Material.TexturePtr ? RenderCommand::ShaderType::Circle :
			RenderCommand::ShaderType::CircleTextured;

		BuildQuadCommand(transform, size, outCommand);
	}

	static inline void BuildSprite(const Transform& transform, const SpriteShape& sprite, Vec2f size, RenderCommand& outCommand) noexcept {
		const auto& texture = sprite.TexturePtr;
		if (!texture)
			return;

		const RectInt cCoords = sprite.TextureRect.value_or(RectInt({ 0, 0 }, texture->getSize()));

		outCommand.Material.TexturePtr = sprite.TexturePtr;
		outCommand.Material.TextureCoords = cCoords;

		outCommand.Quad = BuildQuad(transform, outCommand.Material.Color, size, cCoords);
	}

	static inline void BuildText(const Transform& transform, const TextRender& textRender, RenderCommand& outCommand) noexcept {
		sf::Text& text = GetCachedText(textRender);
		text.setFillColor(outCommand.Material.Color);
		text.setOutlineThickness(outCommand.Outline.Thickness);
		text.setOutlineColor(outCommand.Outline.Color);

		const RectFloat cBounds = GetCachedTextBounds(textRender);
		outCommand.Shape = RenderCommand::TextData{
			.TextPtr = &text,
			.Matrix = RenderSystem::GetRenderStates(transform, cBounds.Size, cBounds.Position).transform
		};
	}

	template<typename TRenderComp>
	[[nodiscard]] static inline RenderCommand BuildRenderCommand(Entity entity, const Transform& transform, const TRenderComp& render, Vec2f size) noexcept {
		RenderCommand command{ .Z = transform.Global.Z };
		PopulateRenderCommand(entity, command);

		if constexpr (std::is_same_v<TRenderComp, RectShape>)
			BuildRect(transform, render, size, command);
		else if constexpr (std::is_same_v<TRenderComp, CircleShape>)
			BuildCircle(transform, render, size, command);
		else if constexpr (std::is_same_v<TRenderComp, SpriteShape>)
			BuildSprite(transform, render, size, command);
		else if constexpr (std::is_same_v<TRenderComp, TextRender>)
			BuildText(transform, render, command);

		return command;
	}

	//======================================================================================

	template<typename TRenderComp>
	[[nodiscard]] static inline Vec2f GetRenderSize(const TRenderComp& render) noexcept {
		if constexpr (std::is_same_v<TRenderComp, RectShape>)
			return render.Size;
		else if constexpr (std::is_same_v<TRenderComp, CircleShape>)
			return Vec2f(render.Radius * 2.f);
		else if constexpr (std::is_same_v<TRenderComp, SpriteShape>)
			return render.TextureRect.value_or(RectInt(Vec2i(0.f), render.TexturePtr->getSize())).Size;
		else if constexpr (std::is_same_v<TRenderComp, TextRender>)
			return GetCachedTextBounds(render).Size;

		return Vec2f::Zero();
	}

	//======================================================================================

	template<typename TRenderComp>
	static inline void CollectRender(Scene& scene, RectFloat cameraBounds, std::vector<RenderCommand>& outCommands) noexcept {
		auto view = scene.View<Transform, TRenderComp>();
		for (const auto& [entity, transform, render] : view.each())
		{
			if (auto hidden = scene.TryGetComponent<Hidden>(entity); hidden && !hidden->Visible)
				continue;

			const Vec2f cSize = GetRenderSize<TRenderComp>(render);
			if (!RenderSystem::IsInCameraBounds(cameraBounds, transform, cSize))
				continue;

			outCommands.push_back(BuildRenderCommand(scene.GetEntity(entity), transform, render, cSize));
		}
	}

	//======================================================================================

	void RenderSystem::OnRender(Scene& scene) const noexcept
	{
		auto renderer = scene.GetRenderer().lock();
		if (!renderer)
		{
			static bool sLogWarned = false;
			if (!sLogWarned)
			{
				BM_CORE_WARN_FN("Scene doesn't have a Renderer attached");
				sLogWarned = true;
			}

			return;
		}

		const RectFloat cCameraBounds = renderer->GetCamera().GetBounds();

		m_RenderCommands.clear();
		m_RenderCommands.reserve(scene.View<Transform>().size());

		CollectRender<RectShape>(scene, cCameraBounds, m_RenderCommands);
		CollectRender<CircleShape>(scene, cCameraBounds, m_RenderCommands);
		CollectRender<SpriteShape>(scene, cCameraBounds, m_RenderCommands);
		CollectRender<TextRender>(scene, cCameraBounds, m_RenderCommands);

		std::ranges::stable_sort(m_RenderCommands, [](const auto& left, const auto& right) {
			if (left.Z != right.Z)
				return left.Z < right.Z;

			if (left.Shader != right.Shader)
				return left.Shader < right.Shader;

			if (left.Material.TexturePtr != right.Material.TexturePtr)
				return left.Material.TexturePtr < right.Material.TexturePtr;

			if (left.Outline.Thickness != right.Outline.Thickness)
				return left.Outline.Thickness < right.Outline.Thickness;

			return std::visit([&](const auto& leftShape) -> bool {
				using TLeft = std::decay_t<decltype(leftShape)>;
				const auto& rightShape = std::get<TLeft>(right.Shape);

				if constexpr (std::is_same_v<TLeft, RectShape>)
					return leftShape.Size.X != rightShape.Size.X ? leftShape.Size.X < rightShape.Size.X :
					leftShape.Size.Y != rightShape.Size.Y ? leftShape.Size.Y < rightShape.Size.Y :
					leftShape.Corner < rightShape.Corner;
				else if constexpr (std::is_same_v<TLeft, CircleShape>)
					return leftShape.Radius < rightShape.Radius;

				return false;
				}, left.Shape);
			});

		DrawRenderCommands(*renderer, m_RenderCommands);
	}

	void RenderSystem::DrawRenderCommands(Renderer& renderer, const std::vector<RenderCommand>& commands) noexcept
	{
		static std::vector<sf::Vertex> sQuadBatch;
		sQuadBatch.clear();
		sQuadBatch.reserve(commands.size() * 6ull);

		const RenderCommand* commandKey = nullptr;
		for (const auto& command : commands)
		{
			if (command.IsShape<RenderCommand::TextData>())
			{
				FlushBatch(renderer, sQuadBatch, commandKey);

				const RenderCommand::TextData& textData = command.GetShape<RenderCommand::TextData>();
				sf::RenderStates states;
				states.blendMode = sf::BlendAlpha;
				states.transform = textData.Matrix;

				renderer.Draw(*textData.TextPtr, states);
				continue;
			}

			if (commandKey && !HasSameUniform(command, *commandKey))
				FlushBatch(renderer, sQuadBatch, commandKey);

			if (!commandKey)
				commandKey = &command;

			sQuadBatch.insert(sQuadBatch.end(), command.Quad.begin(), command.Quad.end());
		}

		FlushBatch(renderer, sQuadBatch, commandKey);
	}

	void RenderSystem::FlushBatch(Renderer& renderer, std::vector<sf::Vertex>& batch, const RenderCommand*& keyPtr) noexcept
	{
		if (batch.empty() || !keyPtr)
			return;

		sf::RenderStates states;
		states.blendMode = sf::BlendAlpha;
		states.shader = GetRenderShader(*keyPtr);

		if (keyPtr->Material.TexturePtr)
			states.texture = keyPtr->Material.TexturePtr;

		renderer.Draw(batch.data(), batch.size(), sf::PrimitiveType::Triangles, states);

		batch.clear();
		keyPtr = nullptr;
	}

	bool RenderSystem::HasSameUniform(const RenderCommand& left, const RenderCommand& right) noexcept
	{
		if (left.Shader != right.Shader)
			return false;
		if (left.Material.TexturePtr != right.Material.TexturePtr)
			return false;
		if (left.Outline.Thickness != right.Outline.Thickness)
			return false;
		if (left.Outline.Color != right.Outline.Color)
			return false;

		return std::visit([&right](const auto& leftShape) -> bool {
			using TLeft = std::decay_t<decltype(leftShape)>;
			const auto& rightShape = std::get<TLeft>(right.Shape);

			if constexpr (std::is_same_v<TLeft, RectShape>)
				return leftShape.Size == rightShape.Size && leftShape.Corner == rightShape.Corner;
			else if constexpr (std::is_same_v<TLeft, CircleShape>)
				return leftShape.Radius == rightShape.Radius;

			return false;
			}, left.Shape);
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
		switch (command.Shader)
		{
			using ShaderType = RenderCommand::ShaderType;

		case ShaderType::Rect:
		{
			auto& shader = s_RectShader;
			const auto& shape = command.GetShape<RectShape>();

			shader.setUniform("uSize", sf::Glsl::Vec2(shape.Size));
			shader.setUniform("uCorner", shape.Corner);
			shader.setUniform("uOutline", command.Outline.Thickness);
			shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
			return &shader;
		}
		case ShaderType::RectTextured:
		{
			auto& shader = s_RectTexturedShader;
			const auto& shape = command.GetShape<RectShape>();

			shader.setUniform("uSize", sf::Glsl::Vec2(shape.Size));
			shader.setUniform("uCorner", shape.Corner);
			shader.setUniform("uOutline", command.Outline.Thickness);
			shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
			return &shader;
		}

		case ShaderType::Circle:
		{
			auto& shader = s_CircleShader;
			const auto& shape = command.GetShape<CircleShape>();

			shader.setUniform("uRadius", shape.Radius);
			shader.setUniform("uOutline", command.Outline.Thickness);
			shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
			return &shader;
		}
		case ShaderType::CircleTextured:
		{
			auto& shader = s_CircleTexturedShader;
			const auto& shape = command.GetShape<CircleShape>();

			shader.setUniform("uRadius", shape.Radius);
			shader.setUniform("uOutline", command.Outline.Thickness);
			shader.setUniform("uOutlineColor", sf::Glsl::Vec4(command.Outline.Color));
			return &shader;
		}

		default:
			break;
		}
		return nullptr;
	}
}
