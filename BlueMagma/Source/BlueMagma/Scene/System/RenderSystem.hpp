#pragma once
#include "ISystem.hpp"
#include "Scene/Component/Base.hpp"
#include "Base/Renderer.hpp"
#include "Core/Vec2.hpp"
#include "Core/Rect.hpp"
#include "Asset/Asset.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <array>
#include <variant>
#include <tuple>
#include <utility>

namespace BM
{
	struct RenderCommand
	{
		struct SpriteData
		{
			const Texture* TexturePtr = nullptr;
			RectFloat TextureCoords{ Vec2f(0.f), Vec2f(1.f) };
		};
		struct TextData
		{
			const sf::Text* TextPtr = nullptr;
			sf::Transform Matrix{};
		};

		using ShapeVariant = std::variant<Component::RectShape, Component::CircleShape, SpriteData, TextData>;

		struct MaterialType
		{
			sf::Color Color = sf::Color::White;
			const Texture* TexturePtr = nullptr;
			RectFloat TextureCoords{ Vec2f(0.f), Vec2f(1.f) };
		} Material{};
		ShapeVariant Shape{};

		float Z = 0.f;
		std::array<sf::Vertex, 6> Quad{};
		Component::Outline Outline{};

		template<typename TShape>
		inline bool IsShape() const noexcept { return std::holds_alternative<TShape>(Shape); }
		template<typename TShape>
		inline const TShape& GetShape() const noexcept { return std::get<TShape>(Shape); }
	};

	class RenderSystem : public ISystem
	{
	public:
		virtual void OnRender(Scene& scene) const noexcept override;
	public:
		static void DrawRenderCommands(Renderer& renderer, const std::vector<RenderCommand>& commands) noexcept;
		static void FlushBatch(Renderer& renderer, std::vector<sf::Vertex>& batch, const RenderCommand*& keyPtr) noexcept;

		static bool IsInCameraBounds(RectFloat cameraBounds, const Component::Transform& transform, Vec2f size) noexcept;
		static bool HasSameUniform(const RenderCommand& left, const RenderCommand& right) noexcept;

		static sf::RenderStates GetRenderStates(const Component::Transform& transform, Vec2f size, Vec2f offset = Vec2f(0.f)) noexcept;
		static sf::Shader* GetRenderShader(const RenderCommand& command) noexcept;
	};
}
