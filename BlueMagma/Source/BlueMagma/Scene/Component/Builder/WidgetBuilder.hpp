#pragma once
#include "RenderBuilder.hpp"

#include "Math/Vec2.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Component/UI.hpp"

#include <optional>

namespace BM
{
	class ButtonBuilder;
	class InputTextBuilder;

	//======================================================================================

	template<typename TBuilder>
	class WidgetBuilderBase : public RenderBuilderBase<TBuilder>
	{
	public:
		TBuilder& WithRectShape(Component::RectShape rectShape) noexcept;
		TBuilder& WithCircleShape(float radius) noexcept;

		TBuilder& WithWidgetColor(Color hoverColor, Color focusColor) noexcept;
		TBuilder& WithWidgetColor(float hoverFactor, float focusFactor) noexcept;

		template<typename UBuilder>
		void CopyWidgetState(const WidgetBuilderBase<UBuilder>& other) noexcept;

		virtual Entity Build(Scene& scene) noexcept = 0;
	protected:
		Entity BuildWidget(Scene& scene) noexcept;
	protected:
		template<typename UBuilder> friend class WidgetBuilderBase;

		using EntityBuilderBase<TBuilder>::Self;
		using EntityBuilderBase<TBuilder>::BuildBase;
		using RenderBuilderBase<TBuilder>::ApplyRender;
		using RenderBuilderBase<TBuilder>::m_Color;
	protected:
		Component::Widget m_Widget{};

		float m_RectCorner = 0.f;

		std::optional<Component::WidgetColor> m_WidgetColor{};
	};

	//======================================================================================

	class WidgetBuilder : public WidgetBuilderBase<WidgetBuilder>
	{
	public:
		ButtonBuilder ToButton() noexcept;
		InputTextBuilder ToInputText() noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	};

	//======================================================================================

	class ButtonBuilder : public WidgetBuilderBase<ButtonBuilder>
	{
	public:
		ButtonBuilder& OnClick(const Component::Clickable::OnClickFn& onClick) noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		Component::Clickable::OnClickFn m_OnClick;
	};

	//======================================================================================

	class InputTextBuilder : public WidgetBuilderBase<InputTextBuilder>
	{
	public:
		InputTextBuilder& WithText(std::string_view text) noexcept;
		InputTextBuilder& WithPlaceholder(std::string_view placeholder) noexcept;
		InputTextBuilder& WithMaxLength(size_t maxLength) noexcept;
		InputTextBuilder& WithPolicy(const Component::InputText::PolicyFn& policy) noexcept;

		InputTextBuilder& WithTextChild(Entity entity) noexcept;
		InputTextBuilder& WithCursorChild(Entity entity) noexcept;

		TextBuilder DefaultTextChildBuilder(Vec2f normalized = { 0.5f, 0.5f }) const noexcept;
		RectBuilder DefaultCursorChildBuilder() const noexcept;

		virtual Entity Build(Scene& scene) noexcept override;
	private:
		Component::InputText m_InputText{};
	};
}

#include "WidgetBuilder.inl"
