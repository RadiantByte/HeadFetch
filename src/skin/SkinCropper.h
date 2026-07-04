#pragma once

#include "../core/Types.h"
#include "../game/Offsets.h"
#include <cstdint>
#include <cstring>

namespace hf::Head {

#pragma pack(push, 4)
struct RawImageHeader {
	std::uint32_t format, width, height, depth;
	std::uint8_t  usage;
	std::uint8_t  pad[7];
};
#pragma pack(pop)
static_assert(sizeof(RawImageHeader) == 24);

inline const RawImageHeader* findSkinImage(void* skinOwner){
	const auto base = reinterpret_cast<std::uintptr_t>(skinOwner);
	for(std::size_t off = 0; off <= Game::MaxSkinImageScan; off += 4){
		auto* img = reinterpret_cast<const RawImageHeader*>(base + off);
		if(img->format != 4 || (img->depth != 0 && img->depth != 1)
			|| img->width < 64 || img->width > 256 || img->width % 64 != 0
			|| img->height < 32 || img->height > 256 || img->height % 32 != 0){
			continue;
		}
		auto imgAddr = reinterpret_cast<std::uintptr_t>(img);
		auto pixels    = *reinterpret_cast<const std::uint8_t* const*>(imgAddr + Game::Image_BytesOffset);
		auto byteCount = *reinterpret_cast<const std::size_t*>(imgAddr + Game::Image_BytesSizeOffset);
		auto required  = static_cast<std::size_t>(img->width) * img->height * 4;
		if(pixels && byteCount >= required && byteCount <= required * 2){
			return img;
		}
	}
	return nullptr;
}

inline bool extract(void* skinOwner, HeadPixels& out){
	if(!skinOwner){ return false; }
	auto* img = findSkinImage(skinOwner);
	if(!img){ return false; }
	auto imgAddr = reinterpret_cast<std::uintptr_t>(img);
	auto pixels = *reinterpret_cast<const std::uint8_t* const*>(imgAddr + Game::Image_BytesOffset);
	auto scale  = img->width / 64;
	if(scale == 0){ return false; }
	const int upScale = HEAD_TEX_SIZE / 8;

	auto copyLayer = [&](int skinX, bool overlay){
		for(int y = 0; y < 8; ++y){
			for(int x = 0; x < 8; ++x){
				int srcX = skinX + x * scale + scale / 2;
				int srcY = 8 * scale + y * scale + scale / 2;
				const auto* src = pixels + (srcY * img->width + srcX) * 4;
				for(int sy = 0; sy < upScale; ++sy){
					for(int sx = 0; sx < upScale; ++sx){
						int dx = x * upScale + sx;
						int dy = y * upScale + sy;
						auto* dst = out.data() + (dy * HEAD_TEX_SIZE + dx) * 4;
						if(!overlay){
							std::memcpy(dst, src, 4);
							continue;
						}
						auto sa = src[3];
						if(sa == 0){ continue; }
						if(sa == 255){ std::memcpy(dst, src, 4); continue; }
						float a = sa / 255.0f;
						float ia = 1.0f - a;
						dst[0] = static_cast<std::uint8_t>(src[0] * a + dst[0] * ia + 0.5f);
						dst[1] = static_cast<std::uint8_t>(src[1] * a + dst[1] * ia + 0.5f);
						dst[2] = static_cast<std::uint8_t>(src[2] * a + dst[2] * ia + 0.5f);
					}
				}
			}
		}
	};

	out.fill(0);
	copyLayer(8 * scale, false);
	copyLayer(40 * scale, true);
	return true;
}

inline bool extractFromEntry(std::uintptr_t entry, HeadPixels& out){
	auto skinOwner = *reinterpret_cast<void* const*>(entry + Game::PLP_EntrySkinRef);
	return extract(skinOwner, out);
}

} // namespace hf::Head
