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
	public:
		bool LoadYaml(const std::string& yamlPath) noexcept;

		//======================================================================================

		void LoadAsset(const std::string& key, std::unique_ptr<AssetHandle> asset) noexcept;
		template<std::derived_from<AssetHandle> TAsset>
		inline bool Load(const std::string& key, const std::filesystem::path& path) noexcept {
			BM_CORE_FN("key: {}, path: {}", key, path.string());
			std::unique_ptr<AssetHandle> asset;

			try
			{
				asset = std::make_unique<TAsset>(path);
			}
			catch (const std::exception& e)
			{
				BM_CORE_ERROR("{}(key: '{}', path: '{}') Exception caught\n - {}", __FUNCTION__, key, path.string(), e.what());
				return false;
			}

			LoadAsset(key, std::move(asset));
			return true;
		}

		//======================================================================================

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset& Get(const std::string& key) const noexcept {
			if (auto asset = dynamic_cast<const TAsset*>(GetAsset(key)))
				return *asset;

			BM_CORE_WARN("{}(key: '{}') Invalid Asset conversion or Asset not found\n - Returned a default asset", __FUNCTION__, key);
			return TAsset::GetDefault();
		}
		const AssetHandle* GetAsset(const std::string& key) const noexcept;
	private:
		std::unordered_map<std::string, std::unique_ptr<AssetHandle>> m_Assets;
	};
}
