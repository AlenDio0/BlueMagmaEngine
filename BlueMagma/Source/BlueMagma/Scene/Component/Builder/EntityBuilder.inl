#pragma once

namespace BM
{
	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithParent(EntityHandle entity) noexcept
	{
		m_ParentEntity = entity;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithTransform(const Component::Transform::LocalSpace& transform) noexcept
	{
		m_Transform = transform;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::At(Vec2f position) noexcept
	{
		m_Transform.State.Position = position;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::AtX(float positionX) noexcept
	{
		m_Transform.State.Position.X = positionX;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::AtY(float positionY) noexcept
	{
		m_Transform.State.Position.Y = positionY;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::AtNormalized(Vec2f otherSize, Vec2f otherOrigin, Vec2f normalized) noexcept
	{
		m_Transform.State.Position = (otherSize * otherOrigin) * -1.f + (otherSize * normalized);
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::AtNormalizedEntity(Entity entity, Vec2f normalized) noexcept
	{
		if (entity && entity.HasAny<Component::RectShape, Component::CircleShape>())
		{
			Vec2f size{};
			if (auto rectShape = entity.TryGet<Component::RectShape>())
				size = rectShape->Size;
			else if (auto circleShape = entity.TryGet<Component::CircleShape>())
				size = Vec2f(circleShape->Radius * 2.f);

			return AtNormalized(size, entity.Get<Component::Transform>().Local.State.Origin, normalized);
		}

		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithScale(Vec2f scale) noexcept
	{
		m_Transform.State.Scale = scale;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithScaleCancelEntity(Entity entity, Vec2f scale) noexcept
	{
		return WithScale(scale / entity.Get<Component::Transform>().Local.State.Scale);
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithOrigin(Vec2f origin) noexcept
	{
		m_Transform.State.Origin = origin;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithRotation(float rotation) noexcept
	{
		m_Transform.State.Rotation = rotation;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::AtZ(float z) noexcept
	{
		m_Transform.Z = z;
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::WithHidden(bool hidden) noexcept
	{
		m_Hidden = { !hidden };
		return Self();
	}

	template<typename TBuilder>
	inline TBuilder EntityBuilderBase<TBuilder>::ToCopy() const noexcept
	{
		return static_cast<const TBuilder&>(*this);
	}

	template<typename TBuilder>
	inline Entity EntityBuilderBase<TBuilder>::BuildBase(Scene& scene) noexcept
	{
		Entity entity = !m_ParentEntity ? scene.CreateEntity(m_Transform) :
			scene.CreateEntityWithParent(m_ParentEntity.value(), m_Transform);

		if (m_Hidden)
			entity.Add<Component::Hidden>(m_Hidden.value());

		return entity;
	}

	template<typename TBuilder>
	inline TBuilder& EntityBuilderBase<TBuilder>::Self() noexcept
	{
		return static_cast<TBuilder&>(*this);
	}
}
