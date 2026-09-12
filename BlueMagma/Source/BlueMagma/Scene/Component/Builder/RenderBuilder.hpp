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
		TBuilder& WithTexture(const Texture* texture, std::optional<RectInt> rect = {}) noexcept;

		template<typename UBuilder>
		void CopyRenderState(const RenderBuilderBase<UBuilder>& other) noexcept;

		virtual Entity Build(Scene& scene) noexcept = 0;
	protected:
		template<typename UBuilder> friend class RenderBuilderBase;

		using EntityBuilderBase<TBuilder>::Self;
		using EntityBuilderBase<TBuilder>::BuildBase;
	protected:
		void ApplyRender(Entity entity);

	protected:
		Color m_Color = ColorDef::White;
		std::optional<Component::Outline> m_Outline;
		std::optional<Component::TextureMaterial> m_TextureMaterial;
	};

	//======================================================================================

	class RenderBuilder : public RenderBuilderBase<RenderBuilder>
	{
	public:
		RectBuilder ToRect() noexcept;
		CircleBuilder ToCircle() noexcept;
		SpriteBuilder ToSprite() noexcept;
		TextBuilder ToText() noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	};

	//======================================================================================

	class RectBuilder : public RenderBuilderBase<RectBuilder>
	{
	public:
		RectBuilder& WithSize(Vec2f size) noexcept;
		RectBuilder& WithCorner(float corner) noexcept;

		WidgetBuilder ToWidget() noexcept;

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

		WidgetBuilder ToWidget() noexcept;

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
		SpriteBuilder& WithTexture(const Texture* texture) noexcept;
		SpriteBuilder& WithTextureRect(std::optional<RectInt> textureRect = {}) noexcept;

		SpriteBuilder& WithScaleAsSizeTexture(const Texture* texture, Vec2f size) noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	private:
		Component::SpriteShape m_SpriteShape{};
	};

	//======================================================================================

	class TextBuilder : public RenderBuilderBase<TextBuilder>
	{
	public:
		TextBuilder& WithFont(const Font* font) noexcept;
		TextBuilder& WithText(std::string text) noexcept;
		TextBuilder& WithCharSize(uint32_t charSize) noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		template<typename TBuilder> friend class RenderBuilderBase;
	private:
		Component::TextRender m_TextRender{};
	};
}

#include "RenderBuilder.inl"
