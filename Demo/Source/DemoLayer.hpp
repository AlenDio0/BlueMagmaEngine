#pragma once
#include <BlueMagma/Layer.hpp>
#include <BlueMagma/SoundManager.hpp>
#include <SFML/Graphics/Sprite.hpp>

class DemoLayer : public BM::Layer
{
public:
	DemoLayer() noexcept;

	virtual void OnTransition() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:
	bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;
	bool OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept;
private:
	sf::Sprite m_Sprite;

	BM::SoundManager m_SoundManager;
};
