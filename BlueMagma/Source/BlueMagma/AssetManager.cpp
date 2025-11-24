#include "bmpch.hpp"
#include "AssetManager.hpp"
#include <functional>
#include <yaml-cpp/yaml.h>

namespace BM
{
	using LoadAssetFn = std::function<void(const AssetManager::AssetKey&, const AssetManager::AssetPath&)>;
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
				using AssetKey = AssetManager::AssetKey;
				using AssetPath = AssetManager::AssetPath;

				onLoad(asset["Key"].as<AssetKey>(), AssetPath(asset["Path"].as<std::string>()));
			}
			catch (const std::exception&)
			{
				continue;
			}
		}
	}

	bool AssetManager::LoadYaml(const YamlPath& yamlPath) noexcept
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
			using namespace std::placeholders;

#define LOAD_ASSET_TYPE(type) LoadType(#type, node, std::bind(&AssetManager::Load<type>, this, _1, _2))
			LOAD_ASSET_TYPE(Texture);
			LOAD_ASSET_TYPE(Font);
			LOAD_ASSET_TYPE(SoundBuffer);
#undef LOAD_ASSET_TYPE
		}
	}

	void AssetManager::LoadAsset(const AssetKey& key, std::unique_ptr<AssetHandle> asset) noexcept
	{
		m_Assets[key] = std::move(asset);
	}

	const AssetHandle* AssetManager::GetAsset(const AssetKey& key) const noexcept
	{
		try
		{
			return m_Assets.at(key).get();
		}
		catch (const std::exception&)
		{
			// TODO: Log get asset error
			return nullptr;
		}
	}
}
