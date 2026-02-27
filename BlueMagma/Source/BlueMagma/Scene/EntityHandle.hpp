#pragma once
#include <entt/entity/fwd.hpp>
#include <cstdint>

namespace BM
{
	using EntityHandle = entt::entity;
}

namespace std
{
	template<>
	struct formatter<BM::EntityHandle>
	{
		constexpr auto parse(auto& context) {
			return context.begin();
		}

		inline auto format(const BM::EntityHandle& handle, auto& context) const noexcept {
			return std::format_to(context.out(), "{}", static_cast<uint32_t>(handle));
		}
	};
}
