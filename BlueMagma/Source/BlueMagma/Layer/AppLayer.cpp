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

	LayerMachine& AppLayer::GetLayers() const noexcept
	{
		return GetApp().Layers;
	}

	AssetManager& AppLayer::GetAssets() const noexcept
	{
		return GetApp().Assets;
	}

	void AppLayer::QueueRemoveLayer() noexcept
	{
		GetLayers().QueueRemove(this);
	}

	void AppLayer::QueueTransition(std::unique_ptr<Layer> toAppLayer) noexcept
	{
		GetLayers().QueueTransition(this, std::move(toAppLayer));
	}
}
