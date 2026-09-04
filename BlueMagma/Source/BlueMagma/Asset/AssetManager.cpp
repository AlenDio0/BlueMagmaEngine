#include "bmpch.hpp"
#include "AssetManager.hpp"
#include <functional>
#include <yaml-cpp/yaml.h>

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
		BM_CORE_ASSERT(!m_Assets.contains(key), "Key has already been used");
		m_Assets[key] = std::move(asset);

		BM_CORE_INFO("Loaded Asset with key '{}'", key);
	}

	const AssetHandle* AssetManager::GetAsset(const std::string& key) const noexcept
	{
		BM_CORE_TRACE("{}(key: '{}') Trying to retrieve an asset", __FUNCTION__, key);
		try
		{
			return m_Assets.at(key).get();
		}
		catch (const std::exception& e)
		{
			BM_CORE_ERROR("{}(key: '{}') Exception caught\n - {}", __FUNCTION__, key, e.what());
			return nullptr;
		}
	}
}
