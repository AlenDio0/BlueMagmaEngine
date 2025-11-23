#include "bmpch.hpp"
#include "SoundManager.hpp"
#include <SFML/Audio/SoundSource.hpp>

namespace BM
{
	SoundManager::SoundManager(const SoundContext& context) noexcept
		: m_Context(context)
	{
	}

	void SoundManager::SetContext(const SoundContext& context, bool update) noexcept
	{
		m_Context = context;

		if (update)
		{
			for (auto& [key, sound] : m_Sounds)
				SetContext(sound, m_Context);
		}
	}

	void SoundManager::SetContext(const std::string& key, const SoundContext& context) noexcept
	{
		try
		{
			SetContext(*Get(key), context);
		}
		catch (...) {}
	}

	void SoundManager::Add(const std::string& key, const BM::SoundBuffer& buffer) noexcept
	{
		Add(key, buffer, m_Context);
	}

	void SoundManager::Add(const std::string& key, const BM::SoundBuffer& buffer, const SoundContext& context) noexcept
	{
		try
		{
			m_Sounds.emplace(key, buffer);
			SetContext(*Get(key), context);
		}
		catch (...) {}
	}

	void SoundManager::Play(const std::string& key, bool loop, bool wait) noexcept
	{
		try
		{
			sf::Sound& sound = m_Sounds.at(key);
			sound.setLooping(loop);

			sound.play();

			while (wait && sound.getStatus() == sf::SoundSource::Status::Playing)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		catch (...) {}
	}

	static inline void PlayCopy(sf::Sound sound) noexcept {
		try
		{
			sound.setLooping(false);
			sound.play();

			while (sound.getStatus() == sf::SoundSource::Status::Playing)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		catch (...) {}
	}

	void SoundManager::PlayThread(const std::string& key) noexcept
	{
		std::thread thread(&PlayCopy, m_Sounds.at(key));
		thread.detach();
	}

	void SoundManager::Stop(const std::string& key) noexcept
	{
		try
		{
			m_Sounds.at(key).stop();
		}
		catch (...) {}
	}

	void SoundManager::Pause(const std::string& key) noexcept
	{
		try
		{
			m_Sounds.at(key).pause();
		}
		catch (...) {}
	}

	sf::Sound* SoundManager::Get(const std::string& key) noexcept
	{
		try
		{
			return &m_Sounds.at(key);
		}
		catch (const std::exception&)
		{
			return nullptr;
		}
	}

	void SoundManager::SetContext(sf::Sound& sound, const SoundContext& context) const noexcept
	{
		sound.setVolume(context._Volume);
		sound.setPitch(context._Pitch);
		sound.setPan(context._Pan);

		sound.setPosition({ context._PositionX, context._PositionY, context._PositionZ });
		sound.setDirection({ context._DirectionX, context._DirectionY, context._DirectionZ });

		sound.setMinDistance(context._MinDistance);
		sound.setMaxDistance(context._MaxDistance);

		sound.setMinGain(context._MinGain);
		sound.setMaxGain(context._MaxGain);

		sound.setAttenuation(context._Attenuation);
	}
}
