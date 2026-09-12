#include "bmpch.hpp"
#include "WidgetBuilder.hpp"

namespace BM
{
	ButtonBuilder WidgetBuilder::ToButton() noexcept
	{
		ButtonBuilder builder;
		builder.CopyWidgetState(*this);

		return builder;
	}

	InputTextBuilder WidgetBuilder::ToInputText() noexcept
	{
		InputTextBuilder builder;
		builder.CopyWidgetState(*this);

		return builder;
	}

	Entity WidgetBuilder::Build(Scene& scene) noexcept
	{
		return BuildWidget(scene);
	}

	//======================================================================================

	ButtonBuilder& ButtonBuilder::OnClick(const Component::Clickable::OnClickFn& onClick) noexcept
	{
		m_OnClick = onClick;
		return Self();
	}

	Entity ButtonBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildWidget(scene);
		entity.Add<Component::Clickable>(m_OnClick);

		return entity;
	}

	//======================================================================================

	InputTextBuilder& InputTextBuilder::WithText(std::string_view text) noexcept
	{
		m_InputText.Text = text;
		return Self();
	}

	InputTextBuilder& InputTextBuilder::WithPlaceholder(std::string_view placeholder) noexcept
	{
		m_InputText.Placeholder = placeholder;
		return Self();
	}

	InputTextBuilder& InputTextBuilder::WithMaxLength(size_t maxLength) noexcept
	{
		m_InputText.MaxLength = maxLength;
		return Self();
	}

	InputTextBuilder& InputTextBuilder::WithPolicy(const Component::InputText::PolicyFn& policy) noexcept
	{
		m_InputText.Policy = policy;
		return Self();
	}

	InputTextBuilder& InputTextBuilder::WithTextChild(Entity entity) noexcept
	{
		m_InputText.TextChild = entity;
		return Self();
	}

	InputTextBuilder& InputTextBuilder::WithCursorChild(Entity entity) noexcept
	{
		m_InputText.CursorChild = entity;
		return Self();
	}

	TextBuilder InputTextBuilder::DefaultTextChildBuilder(Vec2f normalized) const noexcept
	{
		return TextBuilder().AtNormalized(m_Widget.Size, m_Transform.State.Origin, normalized).WithOrigin(Vec2f(0.5f))
			.WithColor(BM::ColorDef::Black);
	}

	RectBuilder InputTextBuilder::DefaultCursorChildBuilder() const noexcept
	{
		BM::Component::Transform::LocalSpace cursorTransform;
		float cursorHeight = m_Widget.Size.Y / 1.5f;

		if (m_InputText.TextChild)
		{
			cursorTransform = m_InputText.TextChild.Get<Component::Transform>().Local;

			if (auto textRender = m_InputText.TextChild.TryGet<Component::TextRender>())
				cursorHeight = static_cast<float>(textRender->CharSize);
		}

		return RectBuilder().WithTransform(cursorTransform).WithOrigin(Vec2f(0.5f)).AtZ(cursorTransform.Z + 1.f)
			.WithHidden(true).WithColor(BM::ColorDef::Black).WithSize(Vec2f(2.f, cursorHeight));
	}

	Entity InputTextBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildWidget(scene);

		if (m_InputText.TextChild)
		{
			m_InputText.TextChild.AssignParent(entity);
			m_InputText.TextChild.AddOrGet<Component::TextRender>();
		}
		else
		{
			m_InputText.TextChild = DefaultTextChildBuilder()
				.WithParent(entity).Build(scene);
		}

		if (m_InputText.CursorChild)
		{
			m_InputText.CursorChild.AssignParent(entity);
			m_InputText.CursorChild.AddOrGet<Component::Hidden>();
		}
		else
		{
			m_InputText.CursorChild = DefaultCursorChildBuilder()
				.WithParent(entity).Build(scene);
		}

		entity.Add<Component::InputText>(m_InputText);

		m_InputText.TextChild = {};
		m_InputText.CursorChild = {};

		return entity;
	}
}
