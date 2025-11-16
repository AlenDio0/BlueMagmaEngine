#include "bmpch.hpp"
#include "LayerMachine.hpp"

namespace BM
{
	void LayerMachine::RemoveLayer(Layer* layer) noexcept
	{
		if (!layer)
			return;

		m_OperationBuffer.push_back(RemoveOperation{ layer });
	}

	void LayerMachine::TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		if (!fromLayer || !toLayer)
			return;

		m_OperationBuffer.push_back(TransitionOperation{ fromLayer, std::move(toLayer) });
	}

	void LayerMachine::PushLayer(std::unique_ptr<Layer> layer) noexcept
	{
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
		if (auto find = FindLayer(m_Layers, remove._Layer); find != m_Layers.end())
		{
			std::unique_ptr<Layer>& layer = *find;

			layer->OnDetach();
			m_Layers.erase(find);
		}
	}

	void LayerMachine::HandleOperation(TransitionOperation& transition) noexcept
	{
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
		m_Layers.push_back(std::move(push._Layer));
		m_Layers.back()->OnAttach();
	}
}
