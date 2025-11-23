#include "bmpch.hpp"
#include "Layer.hpp"
#include "Application.hpp"

namespace BM
{
	Application& Layer::GetApp() const noexcept
	{
		return Application::Get();
	}

	Window& Layer::GetWindow() const noexcept
	{
		return GetApp().GetWindow();
	}

	LayerMachine& Layer::GetMachine() const noexcept
	{
		return GetApp().GetMachine();
	}

	AssetManager& Layer::GetAssets() const noexcept
	{
		return GetApp().GetAssets();
	}

	void Layer::RemoveLayer() noexcept
	{
		GetMachine().RemoveLayer(this);
	}

	void Layer::QueueTransition(std::unique_ptr<Layer> toLayer) noexcept
	{
		GetMachine().TransitionLayer(this, std::move(toLayer));
	}
}
