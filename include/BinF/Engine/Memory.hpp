/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "common.hpp"
#include "Logger.hpp"

#if BINF_PLATFORM == DESKTOP_SDL
#include <SDL3/SDL_stdinc.h>
#endif

namespace BinF::Engine {
    enum class MemType : BinF::u8 {
        GFX,
        Internal,
        External,
        Fast,
        STD
    };

    #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
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
    #endif

    template<typename T>
    inline T* Malloc(const u32 count, const MemType type=MemType::STD) {
        
        return static_cast<T*>(
            #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
            heap_caps_malloc(count*sizeof(T), MemCaps(type))
            #elif BINF_PLATFORM == DESKTOP_SDL
            SDL_malloc(count*sizeof(T))
            #endif
        );

    }
    template<typename T>
    inline T* Malloc(const MemType type=MemType::STD) {
        return Malloc<T>(1U, type);
    }
    template<typename T>
    inline T* Calloc(const u32 n=1U, const MemType type=MemType::STD) {
        
        return static_cast<T*>(
            #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
            heap_caps_calloc(n, sizeof(T), MemCaps(type))
            #elif BINF_PLATFORM == DESKTOP_SDL
            SDL_calloc(n, sizeof(T))
            #endif
        );
    }
    template<typename T>
    inline T* Realloc(T* ptr, u32 count, const MemType type=MemType::STD) {
        return static_cast<T*>(
            #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
            heap_caps_realloc(ptr, count*sizeof(T), MemCaps(type))
            #elif BINF_PLATFORM == DESKTOP_SDL
            SDL_realloc(ptr, count*sizeof(T))
            #endif
        );
    }
    template<typename T> /* just so you can call "Free<>()" */
    inline void Free(T* ptr) {
        #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
        heap_caps_free(ptr);
        #elif BINF_PLATFORM == DESKTOP_SDL
        SDL_free(ptr);
        #endif

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