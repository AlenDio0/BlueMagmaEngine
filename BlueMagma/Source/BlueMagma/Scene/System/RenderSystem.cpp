#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "Scene/Scene.hpp"

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

	static inline sf::Sprite& CreateSprite(const TextureRender& texture, Style style) noexcept {
		static sf::Sprite sSprite{ Texture::GetDefault() };

		if (!texture.TexturePtr)
			return sSprite;

		sSprite.setTexture(*texture.TexturePtr, true);
		if (texture.TextureRect)
			sSprite.setTextureRect(texture.TextureRect.value());
		sSprite.setColor(style.FillColor);

		return sSprite;
	}
	static inline sf::Text& CreateText(const TextRender& text, Style style) noexcept {
		static sf::Text sText{ Font::GetDefault() };

		if (!text.FontPtr)
			return sText;

		sText.setFont(*text.FontPtr);
		sText.setString(text.Text);
		sText.setCharacterSize(text.CharSize);
		sText.setOutlineThickness(style.Outline);
		sText.setFillColor(style.FillColor);
		sText.setOutlineColor(style.OutlineColor);

		return sText;
	}

	static inline bool IsInCameraBounds(RectFloat cameraBounds, const Transform& transform, Vec2f size) noexcept {

		const Vec2f cScaledSize = size * transform.Scale;
		const RectFloat cEntityBounds{ transform.Position - (transform.Origin * cScaledSize), cScaledSize };

		return cameraBounds.Intersects(cEntityBounds);
	}

	void RenderSystem::OnRender(Scene& scene) const noexcept
	{
		Renderer* renderer = scene.GetRenderer();
		if (!renderer)
			return;
		const RectFloat cCameraBounds = renderer->GetCamera().GetBounds();

		auto view = scene.View<Transform>();
		for (auto entity : view)
		{
			auto hidden = scene.TryGetComponent<Hidden>(entity);
			if (hidden && !hidden->Visible)
				continue;

			const Transform& transform = view.get<Transform>(entity);
			Style style{};
			if (auto entityStyle = scene.TryGetComponent<Style>(entity))
				style = *entityStyle;

			if (auto rect = scene.TryGetComponent<RectRender>(entity))
			{
				if (IsInCameraBounds(cCameraBounds, transform, rect->Size))
					DrawRect(*renderer, transform, *rect, style);
			}
			else if (auto circle = scene.TryGetComponent<CircleRender>(entity))
			{
				if (IsInCameraBounds(cCameraBounds, transform, circle->Radius * 2.f))
					DrawCircle(*renderer, transform, *circle, style);
			}
			else if (auto texture = scene.TryGetComponent<TextureRender>(entity))
			{
				auto& sprite = CreateSprite(*texture, style);
				if (IsInCameraBounds(cCameraBounds, transform, sprite.getLocalBounds().size))
					DrawTexture(*renderer, transform, sprite);
			}
			else if (auto textRender = scene.TryGetComponent<TextRender>(entity))
			{
				auto& text = CreateText(*textRender, style);
				if (IsInCameraBounds(cCameraBounds, transform, text.getLocalBounds().size))
					DrawText(*renderer, transform, text);
			}
		}
	}

	static inline sf::RenderStates GetRenderStates(const Transform& transform, Vec2f size, Vec2f offset = { 0.f })
	{
		sf::RenderStates states;
		return states.transform
			.translate(transform.Position)
			.scale(transform.Scale)
			.translate((offset + (transform.Origin * size)).Round() * -1.f);
	}

	static inline sf::RectangleShape s_Rect;
	void RenderSystem::DrawRect(Renderer& renderer, const Transform& transform, RectRender rect, Style style) const noexcept
	{
		const Vec2f cSize = rect.Size;
		auto states = GetRenderStates(transform, cSize);

		if (rect.Corner <= 0.f && style.Outline == 0.f)
		{
			s_Rect.setSize(cSize);
			s_Rect.setFillColor(style.FillColor);
			s_Rect.setOutlineThickness(0.f);

			renderer.Draw(s_Rect, states);
			return;
		}

		auto& shader = Shader::s_RectShader;
		shader.setUniform("uSize", sf::Glsl::Vec2(cSize));
		shader.setUniform("uCorner", rect.Corner);
		shader.setUniform("uOutline", -std::abs(style.Outline));
		shader.setUniform("uOutlineColor", sf::Glsl::Vec4(style.OutlineColor));

		sf::Vertex vertices[6];
		const Vec2f cTopL(0.f, 0.f), cTopR(cSize.X, 0.f), cBottomR(cSize.X, cSize.Y), cBottomL(0.f, cSize.Y);
		const sf::Color cColor = style.FillColor;

		vertices[0] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[1] = sf::Vertex(cTopR, cColor, Vec2f(1.f, 0.f));
		vertices[2] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[3] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[4] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[5] = sf::Vertex(cBottomL, cColor, Vec2f(0.f, 1.f));

		states.shader = &shader;
		states.blendMode = sf::BlendAlpha;

		renderer.Draw(vertices, 6, sf::PrimitiveType::Triangles, states);
	}

	static inline sf::CircleShape s_Circle;
	void RenderSystem::DrawCircle(Renderer& renderer, const Transform& transform, CircleRender circle, Style style) const noexcept
	{
		const float cRadius = circle.Radius;
		const float cDiameter = cRadius * 2.f;
		auto states = GetRenderStates(transform, cDiameter);

		if (style.Outline == 0.f)
		{
			s_Circle.setRadius(cRadius);
			s_Circle.setFillColor(style.FillColor);
			s_Circle.setOutlineThickness(0.f);

			renderer.Draw(s_Circle, states);
			return;
		}

		auto& shader = Shader::s_CircleShader;
		shader.setUniform("uRadius", cRadius);
		shader.setUniform("uOutline", -std::abs(style.Outline));
		shader.setUniform("uOutlineColor", sf::Glsl::Vec4(style.OutlineColor));

		sf::Vertex vertices[6];
		const Vec2f cTopL(0.f, 0.f), cTopR(cDiameter, 0.f), cBottomR(cDiameter, cDiameter), cBottomL(0.f, cDiameter);
		const sf::Color cColor = style.FillColor;

		vertices[0] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[1] = sf::Vertex(cTopR, cColor, Vec2f(1.f, 0.f));
		vertices[2] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[3] = sf::Vertex(cTopL, cColor, Vec2f(0.f, 0.f));
		vertices[4] = sf::Vertex(cBottomR, cColor, Vec2f(1.f, 1.f));
		vertices[5] = sf::Vertex(cBottomL, cColor, Vec2f(0.f, 1.f));

		states.shader = &shader;
		states.blendMode = sf::BlendAlpha;

		renderer.Draw(vertices, 6, sf::PrimitiveType::Triangles, states);
	}

	void RenderSystem::DrawTexture(Renderer& renderer, const Transform& transform, const sf::Sprite& sprite) const noexcept
	{
		RectFloat cBounds = sprite.getLocalBounds();
		renderer.Draw(sprite, GetRenderStates(transform, cBounds.Size, cBounds.Position));
	}

	void RenderSystem::DrawTexture(Renderer& renderer, const Transform& transform, const TextureRender& texture, Style style) const noexcept
	{
		const auto& sprite = CreateSprite(texture, style);
		DrawTexture(renderer, transform, sprite);
	}

	void RenderSystem::DrawText(Renderer& renderer, const Transform& transform, const sf::Text& text) const noexcept
	{
		const RectFloat cBounds = text.getLocalBounds();
		renderer.Draw(text, GetRenderStates(transform, cBounds.Size, cBounds.Position));
	}

	void RenderSystem::DrawText(Renderer& renderer, const Transform& transform, const TextRender& textRender, Style style) const noexcept
	{
		const auto& text = CreateText(textRender, style);
		DrawText(renderer, transform, text);

	}
}
