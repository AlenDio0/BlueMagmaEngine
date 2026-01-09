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
		float _Volume = 100.f;
		float _Pitch = 1.f;
		float _Pan = 0.f;

		float _PositionX = 0.f, _PositionY = 0.f, _PositionZ = 0.f;
		float _DirectionX = 0.f, _DirectionY = 0.f, _DirectionZ = -1.f;

		float _MinDistance = 1.f;
		float _MaxDistance = std::numeric_limits<float>::max();

		float _MinGain = std::numeric_limits<float>::min();
		float _MaxGain = std::numeric_limits<float>::max();

		float _Attenuation = 1.f;
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
