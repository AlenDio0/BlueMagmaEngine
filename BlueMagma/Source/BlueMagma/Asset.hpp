#pragma once
#include <filesystem>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace BM
{
	class AssetHandle
	{
	public:
		inline virtual ~AssetHandle() noexcept = default;
	};

	class Texture : public AssetHandle, public sf::Texture
	{
	public:
		inline Texture(const std::filesystem::path& path)
			: sf::Texture(path) {}
	};

	class Font : public AssetHandle, public sf::Font
	{
	public:
		inline Font(const std::filesystem::path& path)
			: sf::Font(path) {}
	};

	class SoundBuffer : public AssetHandle, public sf::SoundBuffer
	{
	public:
		inline SoundBuffer(const std::filesystem::path& path)
			: sf::SoundBuffer(path) {}
	};
}
