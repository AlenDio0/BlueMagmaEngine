#include "bmpch.hpp"
#include "LayerMachine.hpp"

namespace BM
{
	void LayerMachine::QueueRemove(Layer* layer) noexcept
	{
		BM_CORE_FUNC("layer: {}", (void*)layer);

		if (!layer)
			return;

		m_OperationBuffer.push_back(RemoveOperation{ layer });
	}

	void LayerMachine::QueueTransition(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		BM_CORE_FUNC("fromLayer: {}, toLayer: {}", (void*)fromLayer, (void*)toLayer.get());

		if (!fromLayer || !toLayer)
			return;

		m_OperationBuffer.push_back(TransitionOperation{ fromLayer, std::move(toLayer) });
	}

	void LayerMachine::QueuePush(std::unique_ptr<Layer> layer) noexcept
	{
		BM_CORE_FUNC("layer: {}", (void*)layer.get());

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
		BM_CORE_INFO("{} Remove(layer: {})", __FUNCTION__, (void*)remove._Layer);

		if (auto find = FindLayer(m_Layers, remove._Layer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnDetach();
			m_Layers.erase(find);
		}
	}

	void LayerMachine::HandleOperation(TransitionOperation& transition) noexcept
	{
		BM_CORE_INFO("{} Transition(fromLayer: {}, toLayer: {})", __FUNCTION__,
			(void*)transition._FromLayer, (void*)transition._ToLayer.get());

		if (auto find = FindLayer(m_Layers, transition._FromLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnTransition();
			layer->OnDetach();

			layer = std::move(transition._ToLayer);
			layer->OnAttach();
		}
	}

	void LayerMachine::HandleOperation(PushOperation& push) noexcept
	{
		BM_CORE_INFO("{} Push(layer: {})", __FUNCTION__, (void*)push._Layer.get());

		m_Layers.push_back(std::move(push._Layer));
		m_Layers.back()->OnAttach();
	}
}
