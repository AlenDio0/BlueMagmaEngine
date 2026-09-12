#pragma once

namespace BM
{
	template<typename TBuilder>
	inline TBuilder& WidgetBuilderBase<TBuilder>::WithRectShape(Component::RectShape rectShape) noexcept
	{
		m_Widget.Shape = Component::Widget::ShapeType::Rect;
		m_Widget.Size = rectShape.Size;
		m_RectCorner = rectShape.Corner;

		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& WidgetBuilderBase<TBuilder>::WithCircleShape(float radius) noexcept
	{
		m_Widget.Shape = Component::Widget::ShapeType::Circle;
		m_Widget.Size = Vec2f(radius);

		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& WidgetBuilderBase<TBuilder>::WithWidgetColor(Color hoverColor, Color focusColor) noexcept
	{
		m_WidgetColor = { m_Color, hoverColor, focusColor };
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& WidgetBuilderBase<TBuilder>::WithWidgetColor(float hoverFactor, float focusFactor) noexcept
	{
		m_WidgetColor = { m_Color, (m_Color * hoverFactor).WithAlpha(1.f), (m_Color * focusFactor).WithAlpha(1.f) };
		return Self();
	}

	template<typename TBuilder>
	template<typename UBuilder>
	inline void WidgetBuilderBase<TBuilder>::CopyWidgetState(const WidgetBuilderBase<UBuilder>& other) noexcept
	{
		this->CopyRenderState(other);
		m_Widget = other.m_Widget;
		m_RectCorner = other.m_RectCorner;
		m_WidgetColor = other.m_WidgetColor;
	}

	template<typename TBuilder>
	inline Entity WidgetBuilderBase<TBuilder>::BuildWidget(Scene& scene) noexcept
	{
		Entity entity = BuildBase(scene);
		ApplyRender(entity);

		switch (m_Widget.Shape)
		{
			using Shape = Component::Widget::ShapeType;

		case Shape::Rect:
			entity.Add<Component::Widget>(m_Widget.Size, m_Widget.Shape);
			entity.Add<Component::RectShape>(m_Widget.Size, m_RectCorner);
			break;
		case Shape::Circle:
			entity.Add<Component::Widget>(m_Widget.Size * 2.f, m_Widget.Shape);
			entity.Add<Component::CircleShape>(m_Widget.Size.X);
			break;

		default:
			BM_CORE_NOT_IMPLEMENTED();
			break;
		}

		if (m_WidgetColor)
			entity.Add<Component::WidgetColor>(m_WidgetColor.value());

		return entity;
	}
}
