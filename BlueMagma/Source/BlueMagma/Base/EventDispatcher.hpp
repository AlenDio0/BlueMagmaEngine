#pragma once
#include <SFML/Window/Event.hpp>
#include <functional>

#define BM_EVENT_FN(fn, ...) [&](const auto& e) -> bool { return fn(e, ##__VA_ARGS__); }

namespace BM
{
	class EventHandle : public sf::Event {};

	struct Event
	{
		EventHandle _Handle;
		bool _Done = false;

		inline Event(EventHandle handle) noexcept
			: _Handle(handle) {
		}
	};

	class EventDispatcher
	{
		template<typename TEvent>
		using EventFn = std::function<bool(const TEvent&)>;
	public:
		inline EventDispatcher(Event& event)
			: m_EventPtr(&event) {
		}

		template<typename TEvent>
		inline bool Dispatch(const EventFn<TEvent>& handler) noexcept {
			if (const TEvent* event = m_EventPtr->_Handle.getIf<TEvent>())
			{
				if (handler)
					m_EventPtr->_Done = handler(*event);

				return true;
			}

			return false;
		}
	private:
		Event* m_EventPtr;
	};
}
