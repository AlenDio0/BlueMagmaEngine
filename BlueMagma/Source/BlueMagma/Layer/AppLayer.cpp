#include "bmpch.hpp"
#include "AppLayer.hpp"

namespace BM
{
	Application& AppLayer::GetApp() const noexcept
	{
		return Application::Get();
	}

	Window& AppLayer::GetWindow() const noexcept
	{
		return GetApp().GetWindow();
	}

	Renderer& AppLayer::GetRenderer() const noexcept
	{
		return GetWindow().GetRenderer();
	}

	LayerMachine& AppLayer::GetMachine() const noexcept
	{
		return GetApp().GetMachine();
	}

	AssetManager& AppLayer::GetAssets() const noexcept
	{
		return GetApp().GetAssets();
	}

	void AppLayer::QueueRemoveLayer() noexcept
	{
		GetMachine().QueueRemove(this);
	}

	void AppLayer::QueueTransition(std::unique_ptr<Layer> toAppLayer) noexcept
	{
		GetMachine().QueueTransition(this, std::move(toAppLayer));
	}
}
