#include "bmpch.hpp"
#include "LayerMachine.hpp"

namespace BM
{
	static inline auto FindLayer(auto& layers, Layer* find) noexcept {
		return std::ranges::find(layers, find, &std::unique_ptr<Layer>::get);
	}

	//======================================================================================

	void LayerMachine::Clear() noexcept
	{
		BM_CORE_FN("LayerMachine is being cleared");

		const size_t cSize = m_Layers.size();
		for (size_t i = 0; i < cSize; i++)
			QueueRemove(m_Layers.at(i).get());
		ProcessLayerChanges();

		BM_CORE_INFO_FN("Cleared {} layers", cSize);
	}

	void LayerMachine::QueueRemove(Layer* layer) noexcept
	{
		BM_CORE_DEBUG_FN_ARGS((void*)layer);

		if (!layer)
			return;

		m_OperationBuffer.push_back(RemoveOperation{ layer });
	}

	void LayerMachine::QueueTransition(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		BM_CORE_DEBUG_FN_ARGS((void*)fromLayer, (void*)toLayer.get());

		if (!fromLayer || !toLayer)
			return;

		m_OperationBuffer.push_back(TransitionOperation{ fromLayer, std::move(toLayer) });
	}

	void LayerMachine::QueuePush(std::unique_ptr<Layer> layer) noexcept
	{
		BM_CORE_DEBUG_FN_ARGS((void*)layer.get());

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

	void LayerMachine::HandleOperation(const RemoveOperation& remove) noexcept
	{
		BM_CORE_DEBUG_FN_ARGS((void*)remove.RemoveLayer);
		BM_CORE_INFO_FN("Removing Layer");

		if (auto find = FindLayer(m_Layers, remove.RemoveLayer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnDetach();
			m_Layers.erase(find);
		}
	}

	void LayerMachine::HandleOperation(TransitionOperation& transition) noexcept
	{
		BM_CORE_DEBUG_FN_ARGS((void*)transition.FromLayer, (void*)transition.ToLayer.get());
		BM_CORE_INFO_FN("Transitioning Layer");

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
		BM_CORE_INFO_FN_ARGS((void*)push.PushLayer.get());
		BM_CORE_INFO_FN("Pushing Layer");

		m_Layers.push_back(std::move(push.PushLayer));
		m_Layers.back()->OnAttach();
	}
}
