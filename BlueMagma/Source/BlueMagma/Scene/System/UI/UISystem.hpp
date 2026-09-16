#pragma once
#include "Scene/System/SystemPack.hpp"

#include "WidgetSystem.hpp"
#include "WidgetColorSystem.hpp"
#include "ClickableSystem.hpp"
#include "InputTextSystem.hpp"

namespace BM
{
	using UISystemPack = SystemPack<UI::WidgetSystem, UI::WidgetColorSystem, UI::ClickableSystem, UI::InputTextSystem>;
}
