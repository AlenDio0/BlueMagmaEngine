#pragma once
#include "Scene/System/ISystem.hpp"
#include "Scene/EntityHandle.hpp"
#include "Scene/Scene.hpp"

namespace BM::UI
{
	class WidgetColorSystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;
	private:
		void TryUpdateColor(Registry& registry, EntityHandle entity) noexcept;
	};
}
