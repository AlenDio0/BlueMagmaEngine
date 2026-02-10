#pragma once
#include "ISystem.hpp"
#include <cstdint>

namespace BM
{
	class UISystem : public ISystem
	{
	public:
		virtual void OnEvent(Scene& scene, Event& event) noexcept;
		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;
	private:
		bool OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept;
		bool OnMouseMoved(const EventHandle::MouseMoved& mouseMoved, Scene& scene) noexcept;
		bool OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept;

		void UpdateColor(Scene& scene) noexcept;
		void UpdateInputText(Scene& scene) noexcept;
	private:
		enum SpecialKey : uint32_t { Delete = 8, Enter = 13, Escape = 27 };
	};
}
