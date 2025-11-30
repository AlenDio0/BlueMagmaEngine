#pragma once
#include "Log.hpp"
#include "Asset.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <concepts>
#include <exception>

namespace BM
{
	class AssetManager
	{
	public:
		using AssetKey = std::string;
		using AssetPath = std::filesystem::path;
		using YamlPath = std::string;
	public:
		bool LoadYaml(const YamlPath& yamlPath) noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline bool Load(const AssetKey& key, const AssetPath& path) noexcept {
			BM_CORE_FN("key: {}, path: {}", key, path.string());
			std::unique_ptr<AssetHandle> asset;

			try
			{
				asset = std::make_unique<TAsset>(path);
			}
			catch (const std::exception& e)
			{
				BM_CORE_ERROR("Exception caught (key: {}, path: {})\n - {}", key, path.string(), e.what());
				return false;
			}

			LoadAsset(key, std::move(asset));
			return true;
		}
		void LoadAsset(const AssetKey& key, std::unique_ptr<AssetHandle> asset) noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset* Get(const AssetKey& key) const noexcept {
			if (auto asset = dynamic_cast<const TAsset*>(GetAsset(key)))
				return asset;

			BM_CORE_WARN("No Asset attached to key '{}'\n - Possibly return a default asset", key);

			if (std::is_base_of<Texture, TAsset>())
			{
				return dynamic_cast<const TAsset*>(&s_DefaultTexture);
			}

			if (std::is_base_of<Font, TAsset>())
				return dynamic_cast<const TAsset*>(&s_DefaultFont);

			if (std::is_base_of<SoundBuffer, TAsset>())
				return dynamic_cast<const TAsset*>(&s_DefaultSoundBuffer);

			BM_CORE_ERROR("No default asset for the key '{}'\n - Returned nullptr", key);
			return nullptr;
		}
		const AssetHandle* GetAsset(const AssetKey& key) const noexcept;
	private:
		std::unordered_map<AssetKey, std::unique_ptr<AssetHandle>> m_Assets;
	private:
		static Texture s_DefaultTexture;
		static Font s_DefaultFont;
		static SoundBuffer s_DefaultSoundBuffer;
	};
}
