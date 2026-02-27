#pragma once
#include "Asset.hpp"
#include <limits>
#include <unordered_map>
#include <string>
#include <SFML/Audio/Sound.hpp>


namespace BM
{
	struct SoundContext
	{
		float Volume = 100.f;
		float Pitch = 1.f;
		float Pan = 0.f;

		float PositionX = 0.f, PositionY = 0.f, PositionZ = 0.f;
		float DirectionX = 0.f, DirectionY = 0.f, DirectionZ = -1.f;

		float MinDistance = 1.f;
		float MaxDistance = std::numeric_limits<float>::max();

		float MinGain = std::numeric_limits<float>::min();
		float MaxGain = std::numeric_limits<float>::max();

		float Attenuation = 1.f;
	};

	class SoundManager
	{
	private:
		using milliseconds = std::chrono::milliseconds;
	public:
		SoundManager(const SoundContext& context = {}) noexcept;

		void SetContext(const SoundContext& context, bool update = true) noexcept;
		void SetContext(const std::string& key, const SoundContext& context) noexcept;

		void Add(const std::string& key, const BM::SoundBuffer& buffer) noexcept;
		void Add(const std::string& key, const BM::SoundBuffer& buffer, const SoundContext& context) noexcept;

		void Play(const std::string& key, bool loop = false, bool wait = false, milliseconds delay = milliseconds(100)) noexcept;
		void PlayThread(const std::string& key, milliseconds delay = milliseconds(100)) noexcept;

		void Stop(const std::string& key) noexcept;
		void Pause(const std::string& key) noexcept;

		sf::Sound* Get(const std::string& key) noexcept;
	private:
		void SetContext(sf::Sound& sound, const SoundContext& context) const noexcept;
	private:
		std::unordered_map<std::string, sf::Sound> m_Sounds;

		SoundContext m_Context;
	};
}
