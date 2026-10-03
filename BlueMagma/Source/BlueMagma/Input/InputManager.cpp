#include "bmpch.hpp"
#include "InputManager.hpp"

#include "BlueMagma/Event/EventDispatcher.hpp"

namespace BM
{
	static inline bool ComputeBindEvent(auto bindCode, auto currentCode, const auto& callback, const auto& event, bool release) noexcept {
		if (!callback)
			return false;

		if (static_cast<uint32_t>(bindCode) != static_cast<uint32_t>(currentCode))
			return false;

		return callback(event, release);
	}

	//======================================================================================

	void InputManager::OnEvent(Event& event) const noexcept
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressedEvent));
		dispatcher.Dispatch<BM::EventHandle::MouseButtonReleased>(BM_EVENT_FN(OnMouseReleasedEvent));

		dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressedEvent));
		dispatcher.Dispatch<BM::EventHandle::KeyReleased>(BM_EVENT_FN(OnKeyReleasedEvent));
	}

	void InputManager::AddButton(Mouse::Button button, const MouseFn& callback, bool onPressed, bool onReleased) noexcept
	{
		if (onPressed)
			m_MouseOnPressedBinds.emplace_back(button, callback);

		if (onReleased)
			m_MouseOnReleasedBinds.emplace_back(button, callback);
	}

	void InputManager::AddKey(Keyboard::Scancode scancode, const KeyFn& callback, bool onPressed, bool onReleased) noexcept
	{
		if (onPressed)
			m_KeyOnPressedBinds.emplace_back(scancode, callback);

		if (onReleased)
			m_KeyOnReleasedBinds.emplace_back(scancode, callback);
	}

	void InputManager::AddKey(Keyboard::Key key, const KeyFn& callback, bool onPressed, bool onReleased) noexcept
	{
		AddKey(Keyboard::KeyToScancode(key), callback, onPressed, onReleased);
	}

	void InputManager::AddAnyButton(const std::vector<Mouse::Button>& buttons, const MouseFn& callback, bool onPressed, bool onReleased) noexcept
	{
		for (Mouse::Button button : buttons)
			AddButton(button, callback, onPressed, onReleased);
	}

	void InputManager::AddAnyKey(const std::vector<Keyboard::Scancode>& scancodes, const KeyFn& callback, bool onPressed, bool onReleased) noexcept
	{
		for (Keyboard::Scancode scancode : scancodes)
			AddKey(scancode, callback, onPressed, onReleased);
	}

	void InputManager::AddAnyKey(const std::vector<Keyboard::Key>& keys, const KeyFn& callback, bool onPressed, bool onReleased) noexcept
	{
		for (Keyboard::Key key : keys)
			AddKey(key, callback, onPressed, onReleased);
	}

	bool InputManager::OnMousePressedEvent(EventHandle::MouseButtonPressed mousePressed) const noexcept
	{
		const MouseEvent cMousePressed{ .Button = mousePressed.button, .Position = mousePressed.position };

		for (const auto& mouseBind : m_MouseOnPressedBinds)
		{
			const bool cDispatched = ComputeBindEvent(mouseBind.Button, cMousePressed.Button, mouseBind.Callback, cMousePressed, false);
			if (cDispatched)
				return true;
		}

		return false;
	}

	bool InputManager::OnMouseReleasedEvent(EventHandle::MouseButtonReleased mouseReleased) const noexcept
	{
		const MouseEvent cMouseReleased{ .Button = mouseReleased.button, .Position = mouseReleased.position };

		for (const auto& mouseBind : m_MouseOnReleasedBinds)
		{
			const bool cDispatched = ComputeBindEvent(mouseBind.Button, cMouseReleased.Button, mouseBind.Callback, cMouseReleased, true);
			if (cDispatched)
				return true;
		}

		return false;
	}

	bool InputManager::OnKeyPressedEvent(EventHandle::KeyPressed keyPressed) const noexcept
	{
		const KeyEvent cKeyPressed{ .Scancode = keyPressed.scancode, .CtrlPressed = keyPressed.control ,
			.ShiftPressed = keyPressed.shift, .AltPressed = keyPressed.alt, .SystemPressed = keyPressed.system };

		for (const auto& keyBind : m_KeyOnPressedBinds)
		{
			const bool cDispatched = ComputeBindEvent(keyBind.Scancode, keyPressed.scancode, keyBind.Callback, cKeyPressed, false);
			if (cDispatched)
				return true;
		}

		return false;
	}

	bool BM::InputManager::OnKeyReleasedEvent(EventHandle::KeyReleased keyReleased) const noexcept
	{
		const KeyEvent cKeyReleased{ .Scancode = keyReleased.scancode, .CtrlPressed = keyReleased.control ,
			.ShiftPressed = keyReleased.shift, .AltPressed = keyReleased.alt, .SystemPressed = keyReleased.system };

		for (const auto& keyBind : m_KeyOnReleasedBinds)
		{
			const bool cDispatched = ComputeBindEvent(keyBind.Scancode, keyReleased.scancode, keyBind.Callback, cKeyReleased, true);
			if (cDispatched)
				return true;
		}

		return false;
	}
}
