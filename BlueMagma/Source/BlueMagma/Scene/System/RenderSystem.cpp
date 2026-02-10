#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

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
				vec2 pixelCoord = gl_TexCoord[0].xy * uSize;
				vec2 center = uSize * 0.5;

				vec2 centerDistance = abs(pixelCoord - center);
				vec2 cornerOffset = centerDistance - (center - uCorner);

				float edgeDistance = length(max(cornerOffset, 0.0)) + min(max(cornerOffset.x, cornerOffset.y), 0.0) - uCorner;

				float alpha = 1.0 - smoothstep(0.0, 1.0, edgeDistance);
				float outlineFactor = smoothstep(uOutline - 0.5, uOutline + 0.5, edgeDistance);

				vec4 fillColor = gl_Color;
				vec4 finalColor = mix(fillColor, uOutlineColor, outlineFactor);

				gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
			})";
		static constexpr const char* s_CircleFrag = R"(
			uniform float uRadius;
			uniform float uOutline;
			uniform vec4 uOutlineColor;

			void main() {
				vec2 pixelCoord = gl_TexCoord[0].xy - vec2(0.5);

				float centerDistance = length(pixelCoord) * (uRadius * 2.0);
				float edgeDistance = centerDistance - uRadius;

				float alpha = 1.0 - smoothstep(0.0, 1.0, edgeDistance);
				float outlineFactor = smoothstep(uOutline - 0.5, uOutline + 0.5, edgeDistance);

				vec4 fillColor = gl_Color;
				vec4 finalColor = mix(fillColor, uOutlineColor, outlineFactor);

				gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
			})";

		static inline sf::Shader s_RectShader{ std::string_view(s_RectFrag), sf::Shader::Type::Fragment };
		static inline sf::Shader s_CircleShader{ std::string_view(s_CircleFrag), sf::Shader::Type::Fragment };
	}

	using namespace Component;

	void RenderSystem::OnRender(const Scene& scene, sf::RenderTarget& target) const noexcept
	{
		auto view = scene.View<Transform>();
		for (auto entity : view)
		{
			const Transform& transform = view.get<Transform>(entity);
			Style style{};
			if (auto entityStyle = scene.TryGetComponent<Style>(entity))
				style = *entityStyle;

			if (auto rect = scene.TryGetComponent<RectRender>(entity))
				DrawRect(target, transform, *rect, style);
			else if (auto circle = scene.TryGetComponent<CircleRender>(entity))
				DrawCircle(target, transform, *circle, style);
			else if (auto texture = scene.TryGetComponent<TextureRender>(entity))
				DrawTexture(target, transform, *texture, style);
			else if (auto text = scene.TryGetComponent<TextRender>(entity))
				DrawText(target, transform, *text, style);
		}
	}

	static inline sf::RenderStates GetRenderStates(const Transform& transform, Vec2f size, Vec2f offset = { 0.f })
	{
		sf::RenderStates states;
		return states.transform
			.translate(transform._Position)
			.scale(transform._Scale)
			.translate(Vec2f(offset + transform._Origin * size).Round() * -1.f);
	}

	static inline sf::RectangleShape s_Rect;
	void RenderSystem::DrawRect(sf::RenderTarget& target, const Transform& transform, RectRender rect, Style style) const noexcept
	{
		const Vec2f cSize = rect._Size;
		auto states = GetRenderStates(transform, cSize);

		if (rect._Corner <= 0.f && style._Outline == 0.f)
		{
			s_Rect.setSize(cSize);
			s_Rect.setFillColor(style._FillColor);
			s_Rect.setOutlineThickness(0.f);

			target.draw(s_Rect, states);
			return;
		}

		auto& shader = Shader::s_RectShader;
		shader.setUniform("uSize", sf::Glsl::Vec2(cSize));
		shader.setUniform("uCorner", rect._Corner);
		shader.setUniform("uOutline", -std::abs(style._Outline));
		shader.setUniform("uOutlineColor", sf::Glsl::Vec4(style._OutlineColor));

		sf::Vertex vertices[6];
		const Vec2f cTopL(0.f, 0.f), cTopR(cSize._X, 0.f), cBottomR(cSize._X, cSize._Y), cBottomL(0.f, cSize._Y);
		const sf::Color cColor = style._FillColor;

		vertices[0] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[1] = sf::Vertex(cTopR, cColor, Vec2f(1.f, 0.f));
		vertices[2] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[3] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[4] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[5] = sf::Vertex(cBottomL, cColor, Vec2f(0.f, 1.f));

		states.shader = &shader;
		states.blendMode = sf::BlendAlpha;

		target.draw(vertices, 6, sf::PrimitiveType::Triangles, states);
	}

	static inline sf::CircleShape s_Circle;
	void RenderSystem::DrawCircle(sf::RenderTarget& target, const Transform& transform, CircleRender circle, Style style) const noexcept
	{
		const float cRadius = circle._Radius;
		const float cDiameter = cRadius * 2.f;
		auto states = GetRenderStates(transform, cDiameter);

		if (style._Outline == 0.f)
		{
			s_Circle.setRadius(cRadius);
			s_Circle.setFillColor(style._FillColor);
			s_Circle.setOutlineThickness(0.f);

			target.draw(s_Circle, states);
			return;
		}

		auto& shader = Shader::s_CircleShader;
		shader.setUniform("uRadius", cRadius);
		shader.setUniform("uOutline", -std::abs(style._Outline));
		shader.setUniform("uOutlineColor", sf::Glsl::Vec4(style._OutlineColor));

		sf::Vertex vertices[6];
		const Vec2f cTopL(0.f, 0.f), cTopR(cDiameter, 0.f), cBottomR(cDiameter, cDiameter), cBottomL(0.f, cDiameter);
		const sf::Color cColor = style._FillColor;

		vertices[0] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[1] = sf::Vertex(cTopR, cColor, Vec2f(1.f, 0.f));
		vertices[2] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[3] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[4] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[5] = sf::Vertex(cBottomL, cColor, Vec2f(0.f, 1.f));

		states.shader = &shader;
		states.blendMode = sf::BlendAlpha;

		target.draw(vertices, 6, sf::PrimitiveType::Triangles, states);
	}

	static inline sf::Sprite s_Sprite{ Texture::GetDefault() };
	void RenderSystem::DrawTexture(sf::RenderTarget& target, const Transform& transform, const TextureRender& texture, Style style) const noexcept
	{
		if (!texture._TexturePtr)
			return;

		s_Sprite.setTexture(*texture._TexturePtr, true);
		if (texture._UseRect)
			s_Sprite.setTextureRect(texture._TextureRect);
		s_Sprite.setColor(style._FillColor);

		RectFloat cBounds = s_Sprite.getLocalBounds();
		target.draw(s_Sprite, GetRenderStates(transform, cBounds._Size, cBounds._Position));
	}

	static inline sf::Text s_Text{ Font::GetDefault() };
	void RenderSystem::DrawText(sf::RenderTarget& target, const Transform& transform, const TextRender& text, Style style) const noexcept
	{
		if (!text._FontPtr)
			return;

		s_Text.setFont(*text._FontPtr);
		s_Text.setString(text._Content);
		s_Text.setCharacterSize(text._CharSize);
		s_Text.setOutlineThickness(style._Outline);
		s_Text.setFillColor(style._FillColor);
		s_Text.setOutlineColor(style._OutlineColor);

		const RectFloat cBounds = s_Text.getLocalBounds();
		target.draw(s_Text, GetRenderStates(transform, cBounds._Size, cBounds._Position));
	}
}
