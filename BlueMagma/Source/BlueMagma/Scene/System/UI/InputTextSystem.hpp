#pragma once
#include "Scene/System/ISystem.hpp"
#include "Base/EventDispatcher.hpp"
#include "Scene/Scene.hpp"
#include "Scene/EntityHandle.hpp"
#include "Scene/Component/UI.hpp"
#include "Core/Timer.hpp"

namespace BM::UI
{
	class InputTextSystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;

		virtual void OnEvent(Scene& scene, Event& event) noexcept override;
		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;

		static float CursorIndexCoords(const Component::InputText& input) noexcept;
		static size_t CoordsToCursorIndex(const Component::InputText& input, float coordsX) noexcept;
	private:
		void UpdateInputText(Registry& registry, EntityHandle entity) noexcept;
		void UpdateCursorHidden(Registry& registry, EntityHandle entity) noexcept;

		bool OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept;
		bool OnKeyPressed(const EventHandle::KeyPressed& keyPressed, Scene& scene) noexcept;
		bool OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept;
	private:
		enum SpecialKey : char32_t { Backspace = 8 };
	private:
		Timer m_CursorBlinkTimer;
	};
}
