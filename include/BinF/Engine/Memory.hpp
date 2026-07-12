/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "common.hpp"

namespace BinF::Engine {
    enum class MemType {
        GFX,
        Internal,
        External,
        Fast,
        STD
    };

    inline void* Malloc(const u32 size, const MemType type=MemType::STD) {
        switch (type) {
        case MemType::GFX:      return heap_caps_malloc(size, MALLOC_CAP_DMA);
        case MemType::Fast:     return heap_caps_malloc(size, MALLOC_CAP_INTERNAL);
        case MemType::Internal: return heap_caps_malloc(size, MALLOC_CAP_INTERNAL);
    //  case MemType::External: return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    //  case MemType::STD:      return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        default:                return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        }
    }
    inline void* Calloc(const u32 n, const u32 size, const MemType type=MemType::STD) {
        switch (type) {
        case MemType::GFX:      return heap_caps_calloc(n, size, MALLOC_CAP_DMA);
        case MemType::Fast:     return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL);
        case MemType::Internal: return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL);
    //  case MemType::STD:      return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM);
    //  case MemType::External: return heap_caps_malloc(n, size, MALLOC_CAP_SPIRAM);
        default:                return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM);
        }
    }
    inline void Free(void* ptr) {
        heap_caps_free(ptr);
    }
}