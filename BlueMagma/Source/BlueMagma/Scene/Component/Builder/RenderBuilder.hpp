#pragma once
#include "EntityBuilder.hpp"
#include "Scene/Component/Render.hpp"

namespace BM
{
	class RectBuilder;
	class CircleBuilder;
	class SpriteBuilder;
	class TextBuilder;

	//======================================================================================

	class WidgetBuilder;

	//======================================================================================

	template<typename TBuilder>
	class RenderBuilderBase : public EntityBuilderBase<TBuilder>
	{
	public:
		TBuilder& WithColor(Color color) noexcept;
		TBuilder& WithOutline(Component::Outline outline) noexcept;
		TBuilder& WithTextureMaterial(Component::TextureMaterial textureMaterial) noexcept;
		TBuilder& WithTexture(const Texture* texture, std::optional<RectInt> rect = {}) noexcept;

		TBuilder& WithScaleAsSizeTexture(Vec2u textureSize, Vec2f size) noexcept;
		TBuilder& WithScaleAsSizeTexture(Vec2f size) noexcept;

		template<typename UBuilder>
		void CopyRenderState(const RenderBuilderBase<UBuilder>& other) noexcept;

		virtual Entity Build(Scene& scene) noexcept override = 0;
	public:
		using EntityBuilderBase<TBuilder>::WithScale;
	protected:
		template<typename UBuilder> friend class RenderBuilderBase;

		using EntityBuilderBase<TBuilder>::Self;
		using EntityBuilderBase<TBuilder>::BuildBase;
	protected:
		[[nodiscard]] Entity BuildRender(Scene& scene);

	protected:
		Color m_Color = ColorDef::White;
		std::optional<Component::Outline> m_Outline;
		std::optional<Component::TextureMaterial> m_TextureMaterial;
	};

	//======================================================================================

	class RenderBuilder : public RenderBuilderBase<RenderBuilder>
	{
	public:
		[[nodiscard]] RectBuilder ToRect() noexcept;
		[[nodiscard]] CircleBuilder ToCircle() noexcept;
		[[nodiscard]] SpriteBuilder ToSprite() noexcept;
		[[nodiscard]] TextBuilder ToText() noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	};

	//======================================================================================

	class RectBuilder : public RenderBuilderBase<RectBuilder>
	{
	public:
		RectBuilder& WithSize(Vec2f size) noexcept;
		RectBuilder& WithCorner(float corner) noexcept;

		[[nodiscard]] WidgetBuilder ToWidget() noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	private:
		Component::RectShape m_RectShape{};
	};

	//======================================================================================

	class CircleBuilder : public RenderBuilderBase<CircleBuilder>
	{
	public:
		CircleBuilder& WithRadius(float radius) noexcept;

		[[nodiscard]] WidgetBuilder ToWidget() noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	private:
		float m_Radius = 0.f;
	};

	//======================================================================================

	class SpriteBuilder : public RenderBuilderBase<SpriteBuilder>
	{
	public:
		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	};

	//======================================================================================

	class TextBuilder : public RenderBuilderBase<TextBuilder>
	{
	public:
		TextBuilder& WithFont(const Font* font) noexcept;
		TextBuilder& WithText(std::string_view text) noexcept;
		TextBuilder& WithCharSize(uint32_t charSize) noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	private:
		Component::TextRender m_TextRender{};
	};
}

#include "RenderBuilder.inl"
