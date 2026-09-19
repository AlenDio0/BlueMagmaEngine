#include "RenderBuilder.hpp"
#pragma once

namespace BM
{
	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithColor(Color color) noexcept
	{
		m_Color = color;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithOutline(Component::Outline outline) noexcept
	{
		m_Outline = outline;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithTextureMaterial(Component::TextureMaterial textureMaterial) noexcept
	{
		m_TextureMaterial = textureMaterial;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithTexture(const Texture* texture, std::optional<RectInt> rect) noexcept
	{
		return WithTextureMaterial({ texture, rect });
	}

	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithScaleAsSizeTexture(Vec2u textureSize, Vec2f size) noexcept
	{
		return WithScale(size / textureSize);
	}

	template<typename TBuilder>
	inline TBuilder& RenderBuilderBase<TBuilder>::WithScaleAsSizeTexture(Vec2f size) noexcept
	{
		if (m_TextureMaterial)
		{
			const Vec2i cTextureSize = m_TextureMaterial->TexturePtr ? Vec2i(m_TextureMaterial->TexturePtr->getSize()) : Vec2i(0);
			const Vec2u cTextureRectSize = m_TextureMaterial->TextureRect.value_or(RectInt(Vec2i(0), cTextureSize)).Size;
			return WithScaleAsSizeTexture(cTextureRectSize, size);
		}

		return Self();
	}

	template<typename TBuilder>
	template<typename UBuilder>
	inline void BM::RenderBuilderBase<TBuilder>::CopyRenderState(const RenderBuilderBase<UBuilder>& other) noexcept
	{
		this->CopyEntityState(other);
		m_Color = other.m_Color;
		m_Outline = other.m_Outline;
		m_TextureMaterial = other.m_TextureMaterial;
	}

	template<typename TBuilder>
	inline Entity RenderBuilderBase<TBuilder>::BuildRender(Scene& scene)
	{
		Entity entity = BuildBase(scene);

		entity.Add<Component::ColorMaterial>(m_Color);

		if (m_Outline)
			entity.Add<Component::Outline>(m_Outline.value());

		if (m_TextureMaterial)
			entity.Add<Component::TextureMaterial>(m_TextureMaterial.value());

		return entity;
	}
}
