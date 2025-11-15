#pragma once
#include <BlueMagma/Layer.hpp>

class DemoLayer : public BM::Layer
{
public:
	virtual void OnEvent(const sf::Event& event) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:

};
