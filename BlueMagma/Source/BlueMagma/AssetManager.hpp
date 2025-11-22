#pragma once
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
		template<std::derived_from<AssetHandle> TAsset, typename... Args>
		inline bool Load(const std::string& key, Args&&... args) noexcept {
			try
			{
				LoadAsset(key, std::move(std::make_unique<TAsset>(std::forward<Args>(args)...)));
				return true;
			}
			catch (const std::exception&)
			{
				// TODO: Log loading asset error
				return false;
			}
		}
		void LoadAsset(const std::string& key, std::unique_ptr<AssetHandle> asset) noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset* Get(const std::string& key) const noexcept {
			return dynamic_cast<const TAsset*>(GetAsset(key));
		}
		const AssetHandle* GetAsset(const std::string& key) const noexcept;
	private:
		std::unordered_map<std::string, std::unique_ptr<AssetHandle>> m_Assets;
	};
}
