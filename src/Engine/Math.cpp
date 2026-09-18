#include <BinF/Engine.hpp>

#if BINF_PLATFORM == DESKTOP_SDL
#include <SDL3/SDL_stdinc.h>
#elif BINF_PLATFORM == FRI3D2024 || BINF_PLATFORM == FRI3D2026
#include <Arduino.h>
#endif

namespace BinF::Engine {
    u32 Random() {
        #if BINF_PLATFORM == DESKTOP_SDL
        SDL_srand(SDL_rand(u16m));
        return static_cast<u32>(SDL_rand(u16m));
        #elif BINF_PLATFORM == FRI3D2024 || BINF_PLATFORM == FRI3D2026
        return esp_random();
        #endif
    }
}