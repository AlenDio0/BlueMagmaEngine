#include "bmpch.hpp"
#include "AssetManager.hpp"

namespace BM
{
	void AssetManager::LoadAsset(const std::string& key, std::unique_ptr<AssetHandle> asset) noexcept
	{
		m_Assets[key] = std::move(asset);
	}

	const AssetHandle* AssetManager::GetAsset(const std::string& key) const noexcept
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
