/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "common.hpp"

namespace BinF::Engine {
    enum class MemType : u8 {
        GFX,
        Internal,
        External,
        Fast,
        STD
    };

    inline u32 MemCaps(const MemType type) {
        switch (type) {
            case MemType::GFX:      return  MALLOC_CAP_DMA;
            case MemType::Fast:
            case MemType::Internal: return MALLOC_CAP_INTERNAL;
            case MemType::STD:
            case MemType::External: return MALLOC_CAP_SPIRAM;
            default: 
                Logger.Warn("(Memory) Invalid MemType!");
                return MALLOC_CAP_SPIRAM;
        }
    }

    template<typename T>
    inline T* Malloc(const u32 count, const MemType type=MemType::STD) {
        return static_cast<T*>(heap_caps_malloc(count*sizeof(T), MemCaps(type)));
    }
    template<typename T>
    inline T* Malloc(const MemType type=MemType::STD) {
        return Malloc<T>(1U, type);
    }
    template<typename T>
    inline T* Calloc(const u32 n=1U, const MemType type=MemType::STD) {
        return static_cast<T*>(heap_caps_calloc(n, sizeof(T), MemCaps(type)));
    }
    template<typename T>
    inline T* Realloc(T* ptr, u32 count, const MemType type=MemType::STD) {
        return static_cast<T*>(heap_caps_realloc(ptr, count*sizeof(T), MemCaps(type)));
    }
    template<typename T> /* just so you can call "Free<>()" */
    inline void Free(T* ptr) {
        heap_caps_free(ptr);
    }


    // C++ keyword abstraction
    template <typename T, typename... Args>
    inline T* New(const MemType type, Args&&... args) {
        void* mem = Malloc<T>(type);
        if (!mem) return nullptr;
        else return new (mem) T(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    inline T* New(Args&&... args) {
        return New<T>(MemType::STD, std::forward<Args>(args)...);
    }
    template <typename T>
    inline void Delete(T*obj) {
        if (!obj) return;
        obj->~T();
        Free<T>(obj);
    }
}