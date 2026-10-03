#pragma once
#include "Mouse.hpp"
#include "Keyboard.hpp"

#include "BlueMagma/Event/Event.hpp"

#include <vector>
#include <functional>

/*
*  It works, but it's very jammy and I hate it.
*  TODO: Find a better way to do adapt the Fn<bool(auto, bool)> into a simple Fn<void(void)>.
*/
#define BM_INPUT_FN(fn) [&](const auto& event, bool isRelease) -> bool { fn(); return false; }
#define BM_INPUT_DISPATCH_FN(fn) [&](const auto& event, bool isRelease) -> bool { fn(); return true; }

namespace BM
{
	class InputManager
	{
	public:
		struct MouseEvent
		{
			Mouse::Button Button{};
			Vec2i Position{};
		};
		struct KeyEvent
		{
			Keyboard::Scancode Scancode{};
			bool CtrlPressed = false;
			bool ShiftPressed = false;
			bool AltPressed = false;
			bool SystemPressed = false;
		};
	public:
		using MouseFn = std::function<bool(MouseEvent, bool)>;
		using KeyFn = std::function<bool(KeyEvent, bool)>;
	public:
		void OnEvent(Event& event) const noexcept;

		void AddButton(Mouse::Button button, const MouseFn& callback, bool onPressed = true, bool onReleased = false) noexcept;
		void AddKey(Keyboard::Scancode scancode, const KeyFn& callback, bool onPressed = true, bool onReleased = false) noexcept;
		void AddKey(Keyboard::Key key, const KeyFn& callback, bool onPressed = true, bool onReleased = false) noexcept;

		void AddAnyButton(const std::vector<Mouse::Button>& buttons, const MouseFn& callback, bool onPressed = true, bool onReleased = false) noexcept;
		void AddAnyKey(const std::vector<Keyboard::Scancode>& scancodes, const KeyFn& callback, bool onPressed = true, bool onReleased = false) noexcept;
		void AddAnyKey(const std::vector<Keyboard::Key>& keys, const KeyFn& callback, bool onPressed = true, bool onReleased = false) noexcept;
	public:
		bool OnMousePressedEvent(EventHandle::MouseButtonPressed mousePressed) const noexcept;
		bool OnMouseReleasedEvent(EventHandle::MouseButtonReleased mouseReleased) const noexcept;

		bool OnKeyPressedEvent(EventHandle::KeyPressed keyPressed) const noexcept;
		bool OnKeyReleasedEvent(EventHandle::KeyReleased keyReleased) const noexcept;
	private:
		struct MouseInput
		{
			Mouse::Button Button;
			MouseFn Callback;
		};
		struct KeyInput
		{
			Keyboard::Scancode Scancode;
			KeyFn Callback;
		};
	private:
		std::vector<MouseInput> m_MouseOnPressedBinds;
		std::vector<MouseInput> m_MouseOnReleasedBinds;

		std::vector<KeyInput> m_KeyOnPressedBinds;
		std::vector<KeyInput> m_KeyOnReleasedBinds;
	};
}
