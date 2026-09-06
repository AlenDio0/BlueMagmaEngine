#include "bmpch.hpp"
#include "AssetManager.hpp"

#include <yaml-cpp/yaml.h>

#include <functional>

namespace BM
{
	using LoadAssetFn = std::function<void(const std::string&, const std::filesystem::path&)>;
	static inline void LoadType(std::string_view type, const YAML::Node& node, const LoadAssetFn& onLoad) noexcept {
		if (!onLoad)
			return;

		auto assets = node[type];
		if (!assets.IsSequence())
			return;

		for (auto asset : assets)
		{
			try
			{
				onLoad(asset["Key"].as<std::string>(), std::filesystem::path(asset["Path"].as<std::string>()));
			}
			catch (const std::exception&)
			{
				continue;
			}
		}
	}

	bool AssetManager::LoadYaml(const std::string& yamlPath) noexcept
	{
		YAML::Node node;
		try
		{
			node = YAML::LoadFile(yamlPath);
		}
		catch (const std::exception&)
		{
			return false;
		}

		{
			namespace ph = std::placeholders;

#define LOAD_ASSET_TYPE(type) LoadType(#type, node, std::bind(&AssetManager::Load<type>, this, ph::_1, ph::_2))
			LOAD_ASSET_TYPE(Texture);
			LOAD_ASSET_TYPE(Font);
			LOAD_ASSET_TYPE(SoundBuffer);
#undef LOAD_ASSET_TYPE
		}

		return true;
	}

	void AssetManager::LoadAsset(const std::string& key, std::unique_ptr<AssetHandle> asset) noexcept
	{
		BM_CORE_FN_ARGS(key);

		if (m_Assets.contains(key))
		{
			BM_CORE_WARN_FN_ARGS(key);
			BM_CORE_WARN_FN("Already found another asset related to key, replacing asset");
		}

		m_Assets[key] = std::move(asset);

		BM_CORE_INFO_FN("Loaded asset (key: '{}')", key);
	}

	const AssetHandle* AssetManager::GetAsset(const std::string& key) const noexcept
	{
		BM_CORE_FN_ARGS(key);

		try
		{
			return m_Assets.at(key).get();
		}
		catch (const std::exception& e)
		{
			BM_CORE_ERROR_FN_ARGS(key);
			BM_CORE_ERROR_FN("Cannot find asset, exception caught: {}", e.what());
			return nullptr;
		}
	}
}
