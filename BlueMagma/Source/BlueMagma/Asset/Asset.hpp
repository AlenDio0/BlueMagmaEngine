#pragma once
#include "Core/Vec2.hpp"
#include <cstdint>
#include <filesystem>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace BM
{
	class AssetHandle
	{
	public:
		using AssetPath = std::filesystem::path;
	public:
		inline AssetHandle() noexcept = default;
		inline virtual ~AssetHandle() noexcept = default;

		/*
		*	To implement an Custom Asset you need:
		*	 - Constructor with AssetPath (needed for AssetManager::Load<TAsset>(key, path))
		*	 - Static function 'GetDefault()' that returns const reference of a default asset (needed for AssetManager::Get<TAsset>(key))
		*/
	};

	class Texture : public AssetHandle, public sf::Texture
	{
	public:
		inline Texture()
			: sf::Texture() {}
		inline Texture(const AssetPath& path)
			: sf::Texture(path) {}
		inline Texture(const sf::Image& image)
			: sf::Texture(image) {}

		inline static const Texture& GetDefault() noexcept {
			static Texture texture{ sf::Image{ Vec2u(2), s_TextureBytes } };
			return texture;
		}
	private:
		static constexpr inline const uint8_t s_TextureBytes[] = {
			0xFF, 0x00, 0xFF, 0xFF,
			0x00, 0x00, 0x00, 0xFF,
			0x00, 0x00, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0xFF,
		};
	};

	class Font : public AssetHandle, public sf::Font
	{
	public:
		inline Font()
			: sf::Font() {}
		inline Font(const AssetPath& path)
			: sf::Font(path) {}

		inline static const Font& GetDefault() noexcept {
			static Font font{};
			return font;
		}
	};

	class SoundBuffer : public AssetHandle, public sf::SoundBuffer
	{
	public:
		inline SoundBuffer()
			: sf::SoundBuffer() {}
		inline SoundBuffer(const AssetPath& path)
			: sf::SoundBuffer(path) {}

		inline static const SoundBuffer& GetDefault() noexcept {
			static SoundBuffer soundBuffer{};
			return soundBuffer;
		}
	};
}
