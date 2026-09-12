#pragma once
#include "Math/Vec2.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Component/Core.hpp"
#include "Scene/Component/Render.hpp"

namespace BM
{
	template<typename TBuilder>
	class EntityBuilderBase
	{
	public:
		inline EntityBuilderBase() noexcept = default;
		virtual inline ~EntityBuilderBase() noexcept = default;

		TBuilder& WithParent(EntityHandle entity) noexcept;

		TBuilder& WithTransform(const Component::Transform::LocalSpace& transform) noexcept;
		TBuilder& At(Vec2f position) noexcept;
		TBuilder& AtX(float positionX) noexcept;
		TBuilder& AtY(float positionY) noexcept;
		TBuilder& AtNormalized(Vec2f otherSize, Vec2f otherOrigin, Vec2f normalized = { 0.5f, 0.5f }) noexcept;
		TBuilder& AtNormalizedEntity(Entity entity, Vec2f normalized = { 0.5f, 0.5f }) noexcept;
		TBuilder& WithScale(Vec2f scale) noexcept;
		TBuilder& WithScaleCancelEntity(Entity entity, Vec2f scale = { 1.f, 1.f }) noexcept;
		TBuilder& WithOrigin(Vec2f origin) noexcept;
		TBuilder& WithRotation(float rotation) noexcept;
		TBuilder& AtZ(float z) noexcept;

		TBuilder& WithHidden(bool hidden) noexcept;

		TBuilder ToCopy() const noexcept;

		virtual Entity Build(Scene& scene) noexcept = 0;
	protected:
		Entity BuildBase(Scene& scene) noexcept;

		TBuilder& Self() noexcept;

		template<typename UBuilder>
		inline void CopyEntityState(const EntityBuilderBase<UBuilder>& other) noexcept {
			m_ParentEntity = other.m_ParentEntity;

			m_Transform = other.m_Transform;
			m_Hidden = other.m_Hidden;
		}
	protected:
		template<typename UBuilder> friend class EntityBuilderBase;
	protected:
		std::optional<EntityHandle> m_ParentEntity{};

		Component::Transform::LocalSpace m_Transform{};
		std::optional<Component::Hidden> m_Hidden{};
	};

	//======================================================================================

	class EntityBuilder : public EntityBuilderBase<EntityBuilder>
	{
	public:
		virtual inline Entity Build(Scene& scene) noexcept override {
			return BuildBase(scene);
		}
	};
}

#include "EntityBuilder.inl"
