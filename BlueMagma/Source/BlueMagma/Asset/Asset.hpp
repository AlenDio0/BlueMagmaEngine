#pragma once
#include "DefaultFont.hpp"
#include "Math/Vec2.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <cstdint>
#include <filesystem>

namespace BM
{
	class AssetHandle
	{
	public:
		inline AssetHandle() noexcept = default;
		inline virtual ~AssetHandle() noexcept = default;

		/*
		*	To implement an Custom Asset you need:
		*	 - Constructor with AssetPath (needed for AssetManager::Load<TAsset>(key, path))
		*	 - Static function 'GetDefault()' that returns const reference of a default asset (needed for AssetManager::Get<TAsset>(key))
		*/
	};

	//======================================================================================

	class Texture : public AssetHandle, public sf::Texture
	{
	public:
		inline Texture()
			: sf::Texture() {}
		inline explicit Texture(const std::filesystem::path& path)
			: sf::Texture(path) {}
		inline explicit Texture(const sf::Image& image)
			: sf::Texture(image) {}

		inline static const Texture& GetDefault() noexcept {
			static Texture* sTexture = new Texture{ sf::Image{ Vec2u(2), s_TextureBytes } };
			return *sTexture;
		}
	private:
		static constexpr inline const uint8_t s_TextureBytes[] = {
			0xFF, 0x00, 0xFF, 0xFF,
			0x00, 0x00, 0x00, 0xFF,
			0x00, 0x00, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0xFF,
		};
	};

	//======================================================================================

	class Font : public AssetHandle, public sf::Font
	{
	public:
		inline Font()
			: sf::Font() {}
		inline explicit Font(const std::filesystem::path& path)
			: sf::Font(path) {}
		inline explicit Font(const void* data, size_t bytesSize)
			: sf::Font(data, bytesSize) {}

		inline static const Font& GetDefault() noexcept {
			static Font* sFont = new Font(CreateDefault());
			return *sFont;
		}
	private:
		inline static const Font CreateDefault() noexcept {
			Font font{ DefaultFont::s_TinyTTFBytes, DefaultFont::s_TinyTTFLength };
			font.setSmooth(false);
			return font;
		}
	};

	//======================================================================================

	class SoundBuffer : public AssetHandle, public sf::SoundBuffer
	{
	public:
		inline SoundBuffer()
			: sf::SoundBuffer() {}
		inline explicit SoundBuffer(const std::filesystem::path& path)
			: sf::SoundBuffer(path) {}

		inline static const SoundBuffer& GetDefault() noexcept {
			static SoundBuffer* sSoundBuffer = new SoundBuffer{};
			return *sSoundBuffer;
		}
	};
}
