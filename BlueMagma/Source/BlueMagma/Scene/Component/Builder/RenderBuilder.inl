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
	inline TBuilder& RenderBuilderBase<TBuilder>::WithTexture(const Texture* texture, std::optional<RectInt> rect) noexcept
	{
		m_TextureMaterial = { texture, rect };
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
	inline void RenderBuilderBase<TBuilder>::ApplyRender(Entity entity)
	{
		entity.Add<Component::ColorMaterial>(m_Color);

		if (m_Outline)
			entity.Add<Component::Outline>(m_Outline.value());
		if (m_TextureMaterial)
			entity.Add<Component::TextureMaterial>(m_TextureMaterial.value());
	}
}
