#pragma once
#include "ISystem.hpp"
#include "Scene/Component/Base.hpp"
#include "Base/Renderer.hpp"
#include "Core/Vec2.hpp"
#include "Core/Rect.hpp"
#include "Asset/Asset.hpp"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>
#include <variant>
#include <vector>
#include <array>
#include <tuple>
#include <utility>

namespace BM
{
	struct RenderCommand
	{
		struct RectData { Vec2f Size{}; float Corner = 0.f; };
		struct CircleData { float Radius = 0.f; };
		struct TextureData { const Texture* Ptr = nullptr; };
		struct TextData { sf::Text TextCopy{ Font::GetDefault() }; sf::Transform Matrix{}; };

		using DataType = std::variant<RectData, CircleData, TextureData, TextData>;
		DataType Data;

		float Z = 0.f;
		sf::Vertex Quad[6]{};

		float Outline = 0.f;
		sf::Color OutlineColor{};

		template<typename TData>
		inline const TData& Get() const noexcept { return std::get<TData>(Data); }
		template<typename TData>
		inline bool Is() const noexcept { return std::holds_alternative<TData>(Data); }
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

		static sf::RenderStates GetRenderStates(const Component::Transform& transform, Vec2f size, Vec2f offset = { 0.f }) noexcept;
		static sf::Shader* GetRenderShader(const RenderCommand& command) noexcept;
	};
}
