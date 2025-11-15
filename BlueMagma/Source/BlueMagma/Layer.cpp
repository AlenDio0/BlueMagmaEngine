#include "bmpch.hpp"
#include "Layer.hpp"
#include "Application.hpp"

namespace BM
{
	Application& Layer::GetApp() const noexcept
	{
		return Application::Get();
	}

	void Layer::RemoveLayer() noexcept
	{
		GetApp().RemoveLayer(this);
	}

	void Layer::QueueTransition(std::unique_ptr<Layer> toLayer) noexcept
	{
		GetApp().TransitionLayer(this, std::move(toLayer));
	}
}
