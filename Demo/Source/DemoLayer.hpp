#pragma once
#include <BlueMagma/Layer.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio/Sound.hpp>

class DemoLayer : public BM::Layer
{
public:
	DemoLayer() noexcept;

	virtual void OnTransition() noexcept override;

	virtual void OnEvent(const sf::Event& event) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:
	sf::Sprite m_Sprite;

	sf::Sound m_Sound;
};
