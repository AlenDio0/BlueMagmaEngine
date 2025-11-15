#include "bmpch.hpp"
#include "LayerMachine.hpp"

namespace BM
{
	void LayerMachine::PushLayer(std::unique_ptr<Layer> layer) noexcept
	{
		if (!layer)
			return;

		m_Layers.push_back(std::move(layer));
		m_Layers.back()->OnAttach();
	}

	void LayerMachine::TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		if (!fromLayer || !toLayer)
			return;

		m_Transition._FromLayer = fromLayer;
		m_Transition._ToLayer = std::move(toLayer);
	}

	void LayerMachine::RemoveLayer(Layer* layer) noexcept
	{
		if (!layer)
			return;

		m_RemoveLayer = layer;
	}

	const std::vector<std::unique_ptr<Layer>>& LayerMachine::GetLayers() noexcept
	{
		return m_Layers;
	}

	bool LayerMachine::ProcessLayerChanges() noexcept
	{
		if (m_RemoveLayer)
			OnRemove();

		if (m_Transition)
			OnTransition();

		return !m_Layers.empty();
	}

	static inline auto FindLayer(auto& layers, Layer* find) noexcept {
		return std::ranges::find_if(layers, [&](const std::unique_ptr<Layer>& layer) noexcept { return layer.get() == find; });
	}

	void LayerMachine::OnRemove() noexcept
	{
		if (auto find = FindLayer(m_Layers, m_RemoveLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnDetach();
			m_Layers.erase(find);
		}

		m_RemoveLayer = nullptr;
	}

	void LayerMachine::OnTransition() noexcept
	{
		if (auto find = FindLayer(m_Layers, m_Transition._FromLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnTransition();
			layer->OnDetach();

			layer = std::move(m_Transition._ToLayer);
			layer->OnAttach();
		}

		m_Transition = {};
	}
}
