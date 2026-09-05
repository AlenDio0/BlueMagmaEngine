#pragma once
#include "Base/EventDispatcher.hpp"

namespace BM
{
	class Application;
	class LayerMachine;

	class Layer
	{
	public:
		inline Layer() noexcept = default;
		inline virtual ~Layer() noexcept = default;

		Layer(const Layer&) = delete;
		Layer(Layer&&) = delete;
		Layer& operator=(const Layer&) = delete;
		Layer& operator=(Layer&&) = delete;

		inline virtual void OnAttach() noexcept {}
		inline virtual void OnDetach() noexcept {}
		inline virtual void OnTransition() noexcept {}

		inline virtual void OnEvent(Event& event) noexcept {}
		inline virtual void OnTick(float timeStep) noexcept {}
		inline virtual void OnUpdate(float deltaTime) noexcept {}
		inline virtual void OnRender() noexcept {}
	};
}
