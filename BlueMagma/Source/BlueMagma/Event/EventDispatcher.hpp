#pragma once
#include "Event.hpp"

#include <functional>

#define BM_EVENT_FN(fn, ...) [&](const auto& e) -> bool { return fn(e, ##__VA_ARGS__); }

namespace BM
{
	class EventDispatcher
	{
		template<typename TEvent>
		using OnEvent = std::function<bool(const TEvent&)>;
	public:
		constexpr explicit EventDispatcher(Event& event) noexcept
			: m_EventPtr(&event) {
		}

		template<typename TEvent>
		constexpr bool Dispatch(const OnEvent<TEvent>& handler) noexcept {
			if (m_EventPtr->Done)
				return false;

			if (const TEvent* event = m_EventPtr->Handle.getIf<TEvent>())
			{
				if (handler)
					m_EventPtr->Done = handler(*event);

				return true;
			}

			return false;
		}
	private:
		Event* m_EventPtr;
	};
}
