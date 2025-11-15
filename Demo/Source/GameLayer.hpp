#pragma once
#include <BlueMagma/Layer.hpp>

class GameLayer : public BM::Layer
{
public:
	virtual void OnAttach() noexcept override;
	virtual void OnDetach() noexcept override;
	virtual void OnTransition() noexcept override;

	virtual void OnEvent(const sf::Event& event) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:
};
