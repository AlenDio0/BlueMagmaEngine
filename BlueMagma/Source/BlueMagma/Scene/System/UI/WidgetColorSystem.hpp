#pragma once
#include "BlueMagma/Scene/System/ISystem.hpp"

#include "BlueMagma/Scene/EntityHandle.hpp"
#include "BlueMagma/Scene/Scene.hpp"

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
