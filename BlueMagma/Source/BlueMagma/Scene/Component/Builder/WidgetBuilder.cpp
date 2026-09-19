#include "bmpch.hpp"
#include "WidgetBuilder.hpp"

#include "Scene/System/UI/CheckboxSystem.hpp"

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
		return TextBuilder().AtNormalized(m_Widget.Size, m_Transform.State.Origin, normalized)
			.AtZ(0.1f).WithOrigin(Vec2f(0.5f)).WithColor(BM::ColorDef::Black);
	}

	RectBuilder InputTextBuilder::DefaultCursorChildBuilder() const noexcept
	{
		BM::Component::Transform::LocalSpace cursorTransform;
		if (m_InputText.TextChild)
			cursorTransform = m_InputText.TextChild.Get<Component::Transform>().Local;

		const Vec2f cCursorSize{ 2.f, m_Widget.Size.Y / 1.5f };

		return RectBuilder().WithTransform(cursorTransform).WithOrigin(Vec2f(0.5f)).AtZ(0.2f)
			.WithHidden(true).WithColor(BM::ColorDef::Black).WithSize(cCursorSize);
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
			m_InputText.TextChild = DefaultTextChildBuilder().WithParent(entity)
				.Build(scene);
		}

		if (m_InputText.CursorChild)
		{
			m_InputText.CursorChild.AssignParent(entity);
			m_InputText.CursorChild.AddOrGet<Component::Hidden>();
		}
		else
		{
			m_InputText.CursorChild = DefaultCursorChildBuilder().WithParent(entity)
				.Build(scene);
		}

		entity.Add<Component::InputText>(m_InputText);

		m_InputText.TextChild = {};
		m_InputText.CursorChild = {};

		return entity;
	}

	//======================================================================================

	CheckboxBuilder& CheckboxBuilder::WithActive(bool active) noexcept
	{
		m_Checkbox.Active = active;
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::OnChanged(const Component::Checkbox::OnChangedFn& onChanged) noexcept
	{
		m_Checkbox.OnChanged = onChanged;
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::WithActiveColor(Color color) noexcept
	{
		m_Checkbox.ActiveMaterial = Component::ColorMaterial{ color };
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::WithInactiveColor(Color color) noexcept
	{
		m_Checkbox.InactiveMaterial = Component::ColorMaterial{ color };
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::WithActiveTexture(const Texture* texture, std::optional<RectInt> textureRect) noexcept
	{
		m_Checkbox.ActiveMaterial = Component::TextureMaterial{ texture, textureRect };
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::WithInactiveTexture(const Texture* texture, std::optional<RectInt> textureRect) noexcept
	{
		m_Checkbox.InactiveMaterial = Component::TextureMaterial{ texture, textureRect };
		return Self();
	}

	CheckboxBuilder& CheckboxBuilder::WithCheckChild(Entity entity) noexcept
	{
		m_Checkbox.CheckChild = entity;
		return Self();
	}

	RenderBuilder CheckboxBuilder::DefaultCheckChildBuilder(Vec2f normalized) const noexcept
	{
		RenderBuilder builder;
		builder.AtNormalized(m_Widget.Size, m_Transform.State.Origin, normalized).WithOrigin(Vec2f(0.5f)).AtZ(0.1f);

		if (m_Checkbox.Active)
		{
			if (auto material = UI::CheckboxSystem::TryGetColorMaterial(m_Checkbox.ActiveMaterial))
				builder.WithColor(material->Color);
			else if (auto material = UI::CheckboxSystem::TryGetTextureMaterial(m_Checkbox.ActiveMaterial))
				builder.WithTextureMaterial(material.value()).WithScaleAsSizeTexture(m_Widget.Size);;
		}
		else
		{
			if (auto material = UI::CheckboxSystem::TryGetColorMaterial(m_Checkbox.InactiveMaterial))
				builder.WithColor(material->Color);
			else if (auto material = UI::CheckboxSystem::TryGetTextureMaterial(m_Checkbox.InactiveMaterial))
				builder.WithTextureMaterial(material.value()).WithScaleAsSizeTexture(m_Widget.Size);;
		}

		return builder;
	}

	Entity CheckboxBuilder::Build(Scene& scene) noexcept
	{
		Entity entity = BuildWidget(scene);

		if (m_Checkbox.CheckChild)
		{
			m_Checkbox.CheckChild.AssignParent(entity);
		}
		else
		{
			m_Checkbox.CheckChild = DefaultCheckChildBuilder().WithParent(entity)
				.Build(scene);
		}

		entity.Add<Component::Checkbox>(m_Checkbox);

		m_Checkbox.CheckChild = {};

		return entity;
	}
}
