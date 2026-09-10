#pragma once
#include "Asset.hpp"

#include "Core/Log.hpp"

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
		bool LoadYaml(const std::string& yamlPath) noexcept;

		//======================================================================================

		void LoadAsset(const std::string& key, std::shared_ptr<AssetHandle> asset) noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline bool Load(const std::string& key, const std::filesystem::path& path) noexcept {
			BM_CORE_DEBUG_FN_ARGS(key, path.string());
			std::shared_ptr<AssetHandle> asset;

			try
			{
				asset = std::make_shared<TAsset>(path);
			}
			catch (const std::exception& e)
			{
				BM_CORE_ERROR_FN_ARGS(key, path.string());
				BM_CORE_ERROR_FN("Failed to load asset, exception caught: {}", e.what());
				return false;
			}

			LoadAsset(key, std::move(asset));
			return true;
		}

		//======================================================================================

		template<std::derived_from<AssetHandle> TAsset>
		inline std::weak_ptr<TAsset> Retrieve(const std::string& key) noexcept {
			if (auto asset = std::dynamic_pointer_cast<TAsset>(GetAsset(key).lock()))
				return asset;

			return {};
		}

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset& Get(const std::string& key) noexcept {
			if (const TAsset* asset = dynamic_cast<const TAsset*>(GetAsset(key).lock().get()))
				return *asset;

			BM_CORE_WARN_FN_ARGS(key);
			BM_CORE_WARN_FN("Invalid asset or not found, returned a default asset");

			return TAsset::GetDefault();
		}

		std::weak_ptr<AssetHandle> GetAsset(const std::string& key) noexcept;
	private:
		std::unordered_map<std::string, std::shared_ptr<AssetHandle>> m_Assets;
	};
}
