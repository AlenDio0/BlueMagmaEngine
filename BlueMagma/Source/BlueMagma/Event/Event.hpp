#pragma once
#include <SFML/Window/Event.hpp>

namespace BM
{
	using EventHandle = sf::Event;

	//======================================================================================

	struct Event
	{
		EventHandle Handle;
		bool Done = false;

		inline Event(EventHandle handle) noexcept
			: Handle(handle) {
		}
	};
}
