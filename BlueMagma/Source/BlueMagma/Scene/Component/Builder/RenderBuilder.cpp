#include "bmpch.hpp"
#include "RenderBuilder.hpp"

#include "WidgetBuilder.hpp"

namespace BM
{
	RectBuilder RenderBuilder::ToRect() noexcept
	{
		RectBuilder builder;
		builder.CopyRenderState(*this);

		return builder;
	}
	CircleBuilder RenderBuilder::ToCircle() noexcept
	{
		CircleBuilder builder;
		builder.CopyRenderState(*this);

		return builder;
	}
	SpriteBuilder RenderBuilder::ToSprite() noexcept
	{
		SpriteBuilder builder;
		builder.CopyRenderState(*this);

		return builder;
	}

	TextBuilder RenderBuilder::ToText() noexcept
	{
		TextBuilder builder;
		builder.CopyRenderState(*this);

		return builder;
	}


	Entity RenderBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);

		return entity;
	}

	//======================================================================================

	RectBuilder& RectBuilder::WithSize(Vec2f size) noexcept
	{
		m_RectShape.Size = size;
		return Self();
	}

	RectBuilder& RectBuilder::WithCorner(float corner) noexcept
	{
		m_RectShape.Corner = corner;
		return Self();
	}

	WidgetBuilder RectBuilder::ToWidget() noexcept
	{
		WidgetBuilder builder;
		builder.CopyRenderState(*this);
		builder.WithRectShape(m_RectShape);

		return builder;
	}

	Entity RectBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);
		entity.Add<Component::RectShape>(m_RectShape);

		return entity;
	}

	//======================================================================================

	CircleBuilder& CircleBuilder::WithRadius(float radius) noexcept
	{
		m_Radius = radius;
		return Self();
	}

	WidgetBuilder CircleBuilder::ToWidget() noexcept
	{
		WidgetBuilder builder;
		builder.CopyRenderState(*this);
		builder.WithCircleShape(m_Radius);

		return builder;
	}

	Entity CircleBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);
		entity.Add<Component::CircleShape>(m_Radius);

		return entity;
	}

	//======================================================================================

	SpriteBuilder& SpriteBuilder::WithTexture(const Texture* texture) noexcept
	{
		m_SpriteShape.TexturePtr = texture;
		return Self();
	}

	SpriteBuilder& SpriteBuilder::WithTextureRect(std::optional<RectInt> textureRect) noexcept
	{
		m_SpriteShape.TextureRect = textureRect;
		return Self();
	}

	SpriteBuilder& SpriteBuilder::WithScaleAsSizeTexture(const Texture* texture, Vec2f size) noexcept
	{
		if (texture)
			return WithScale(size / texture->getSize());

		return Self();
	}

	Entity SpriteBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);
		entity.Add<Component::SpriteShape>(m_SpriteShape);

		return entity;
	}

	//======================================================================================

	TextBuilder& TextBuilder::WithFont(const Font* font) noexcept
	{
		m_TextRender.FontPtr = font;
		return Self();
	}

	TextBuilder& TextBuilder::WithText(std::string text) noexcept
	{
		m_TextRender.Text = text;
		return Self();
	}

	TextBuilder& TextBuilder::WithCharSize(uint32_t charSize) noexcept
	{
		m_TextRender.CharSize = charSize;
		return Self();
	}

	Entity TextBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);
		entity.Add<Component::TextRender>(m_TextRender);

		return entity;
	}

}
