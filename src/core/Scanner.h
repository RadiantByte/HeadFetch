#pragma once

#include <libhat.hpp>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace hf::Scan {

inline std::vector<std::uintptr_t> findAllInRange(const void* rangeStart, std::size_t rangeLen, const char* name){
	std::vector<std::uintptr_t> results;
	auto nameLen = std::strlen(name);
	if(nameLen == 0 || rangeLen < nameLen){ return results; }
	const auto* hay = static_cast<const std::uint8_t*>(rangeStart);
	for(std::size_t i = 0; i + nameLen <= rangeLen; ++i){
		if(std::memcmp(hay + i, name, nameLen) == 0){
			results.push_back(reinterpret_cast<std::uintptr_t>(hay + i));
		}
	}
	return results;
}

inline std::uintptr_t findPtrInRange(const void* rangeStart, std::size_t rangeLen, std::uintptr_t value){
	auto span = std::span<const std::byte>(static_cast<const std::byte*>(rangeStart), rangeLen);
	auto result = hat::find_pattern(span, hat::object_to_signature(value));
	auto* found = result.get();
	return found ? reinterpret_cast<std::uintptr_t>(found) : 0;
}

} // namespace hf::Scan
