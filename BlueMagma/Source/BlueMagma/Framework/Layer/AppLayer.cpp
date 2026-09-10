#include "bmpch.hpp"
#include "AppLayer.hpp"

namespace BM
{
	void AppLayer::AttachApplication(Application* applicationPtr) noexcept
	{
		m_ApplicationPtr = applicationPtr;
		OnAttachApplication();
	}

	Application& AppLayer::GetApp() const noexcept
	{
		BM_CORE_ASSERT(m_ApplicationPtr != nullptr, "Application is not accessible");
		return *m_ApplicationPtr;
	}

	std::weak_ptr<Window> AppLayer::GetWindow() const noexcept
	{
		return GetApp().GetWindow();
	}

	std::weak_ptr<Renderer> AppLayer::GetRenderer() const noexcept
	{
		return GetApp().GetRenderer();
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
