#include "bmpch.hpp"
#include "LayerMachine.hpp"

namespace BM
{
	void LayerMachine::QueueRemove(Layer* layer) noexcept
	{
		BM_CORE_FN("layer: {}", (void*)layer);

		if (!layer)
			return;

		m_OperationBuffer.push_back(RemoveOperation{ layer });
	}

	void LayerMachine::QueueTransition(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		BM_CORE_FN("fromLayer: {}, toLayer: {}", (void*)fromLayer, (void*)toLayer.get());

		if (!fromLayer || !toLayer)
			return;

		m_OperationBuffer.push_back(TransitionOperation{ fromLayer, std::move(toLayer) });
	}

	void LayerMachine::QueuePush(std::unique_ptr<Layer> layer) noexcept
	{
		BM_CORE_FN("layer: {}", (void*)layer.get());

		if (!layer)
			return;

		m_OperationBuffer.push_back(PushOperation{ std::move(layer) });
	}

	const std::vector<std::unique_ptr<Layer>>& LayerMachine::GetLayers() noexcept
	{
		return m_Layers;
	}

	bool LayerMachine::ProcessLayerChanges() noexcept
	{
		for (auto& operation : m_OperationBuffer)
			std::visit([&](auto& op) { HandleOperation(op); }, operation);
		m_OperationBuffer.clear();

		return !m_Layers.empty();
	}

	static inline auto FindLayer(auto& layers, Layer* find) noexcept {
		return std::ranges::find(layers, find, &std::unique_ptr<Layer>::get);
	}

	void LayerMachine::HandleOperation(const RemoveOperation& remove) noexcept
	{
		BM_CORE_INFO("Removing Layer [Layer: {}]", (void*)remove.RemoveLayer);

		if (auto find = FindLayer(m_Layers, remove.RemoveLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnDetach();
			m_Layers.erase(find);
		}
	}

	void LayerMachine::HandleOperation(TransitionOperation& transition) noexcept
	{
		BM_CORE_INFO("Transitioning Layer [FromLayer: {}, ToLayer: {}]",
			(void*)transition.FromLayer, (void*)transition.ToLayer.get());

		if (auto find = FindLayer(m_Layers, transition.FromLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnTransition();
			layer->OnDetach();

			layer = std::move(transition.ToLayer);
			layer->OnAttach();
		}
	}

	void LayerMachine::HandleOperation(PushOperation& push) noexcept
	{
		BM_CORE_INFO("Pushing Layer [Layer: {}]", (void*)push.PushLayer.get());

		m_Layers.push_back(std::move(push.PushLayer));
		m_Layers.back()->OnAttach();
	}
}
