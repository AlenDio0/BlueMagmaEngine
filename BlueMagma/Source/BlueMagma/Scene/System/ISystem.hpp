#pragma once
#include "Base/EventDispatcher.hpp"
#include <functional>

namespace BM
{
	class Scene;

	using SystemEventFn = std::function<void(Scene&, Event&)>;
	using SystemUpdateFn = std::function<void(Scene&, float)>;
	using SystemRenderFn = std::function<void(Scene&)>;

	class ISystem
	{
	public:
		inline ISystem() noexcept = default;
		inline virtual void OnAttach(Scene& scene) noexcept {}

		inline virtual void OnEvent(Scene& scene, Event& event) noexcept {}
		inline virtual void OnUpdate(Scene& scene, float deltaTime) noexcept {}
		inline virtual void OnRender(Scene& scene) const noexcept {}

		/*
		*	To implement a Custom System you need:
		*	 - Public ISystem as a parent class
		*	 - Implement any virtual function you need
		*	 - To implement member variables, if needed, you can extend its lifetime with the Scene
		*	    constructing it as a CtxComponent
		*/
	};
}
