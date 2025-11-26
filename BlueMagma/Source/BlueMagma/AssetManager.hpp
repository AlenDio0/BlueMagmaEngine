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
			BM_CORE_FUNC("key: {}, path: {}", key, path.string());
			std::unique_ptr<AssetHandle> asset;

			try
			{
				asset = std::make_unique<TAsset>(path);
			}
			catch (const std::exception& e)
			{
				BM_CORE_ERROR("Exception caught (key: {}, path: {}): {}", key, path.string(), e.what());
				return false;
			}

			LoadAsset(key, std::move(asset));
			return true;
		}
		void LoadAsset(const AssetKey& key, std::unique_ptr<AssetHandle> asset) noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset* Get(const AssetKey& key) const noexcept {
			return dynamic_cast<const TAsset*>(GetAsset(key));
		}
		const AssetHandle* GetAsset(const AssetKey& key) const noexcept;
	private:
		std::unordered_map<AssetKey, std::unique_ptr<AssetHandle>> m_Assets;
	};
}
