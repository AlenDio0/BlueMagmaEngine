#pragma once
#include <BlueMagma/Layer.hpp>
#include <BlueMagma/SoundManager.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class GameLayer : public BM::Layer
{
public:
	GameLayer() noexcept;

	virtual void OnAttach() noexcept override;
	virtual void OnDetach() noexcept override;
	virtual void OnTransition() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnUpdate(float deltaTime) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:
	bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;
private:
	sf::RectangleShape m_Background;

	sf::Text m_Text;
	sf::Clock m_UpdateText;

	BM::SoundManager m_SoundManager;
};
