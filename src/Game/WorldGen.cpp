/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/WorldGen.hpp>

namespace BinF::Game {
    /*
        BinF will use a three-pass system (for now), each working on the last.

        Pass 1: generate terrain (dirt, stone, grass)
        Pass 2: generate ore veins (stone-only)
        Pass 3: decorations
    */

   using Hash = u32;
    // settings
    // controls density of terrain
    constexpr f32 TerrainScale  = 1.0f / 24.0f;
    constexpr f32 DirtGrass     = -0.15f;
    constexpr f32 GrassStone    = 0.15f;
    constexpr u32 TerrainSalt   = 0x1000u;

    constexpr u32 ChunkCornerSize = ChunkSize+1;
    constexpr u32 ChunkPadSize = ChunkCornerSize+2;

    enum class Terrain : BinF::u8 { Dirt = 0, Grass = 1, Stone = 2 };


    Seed WorldSeed = 0U;

    // normalise the hash [0, 1]
    constexpr f32 NormaliseHash(Hash h) {
        return static_cast<f32>(h) / static_cast<f32>(u32m);
    }

    // Smooth out values
    constexpr f32 SmoothStep(f32 t) {
        return t * t * (3.0f - 2.0f * t);
    }

    // Deterministic hashing (;-;)
    static inline Hash Hash2D(const s32 x, const s32 y, const Seed seed) {
        Hash h = seed;
        h ^= static_cast<u32>(x) * 0x1b873593u;
        h = (h << 15) | (h >> 17);
        h *= 0xcc9e2d51u;
        h ^= static_cast<u32>(y) * 0x85ebca6bu;
        h = (h << 13) | (h >> 19);
        h *= 0xe6546b64u;
        h ^= h >> 16; h *= 0x85ebca6bu;
        h ^= h >> 13; h *= 0xc2b2ae35u;
        h ^= h >> 16;
        return h;
    }

    // Bilinear noise mapped [-1, 1]
    static inline f32 ValueNoise2D(const f32 x, const f32 y, const Seed seed) {
        const s32 x0 = static_cast<s32>(floorf(x));
        const s32 y0 = static_cast<s32>(floorf(y));
        const s32 x1 = x0 + 1, y1 = y0 + 1;

        const f32 sx = SmoothStep(x - static_cast<f32>(x0));
        const f32 sy = SmoothStep(y - static_cast<f32>(y0));

        const f32 n00 = NormaliseHash(Hash2D(x0, y0, seed));
        const f32 n10 = NormaliseHash(Hash2D(x1, y0, seed));
        const f32 n01 = NormaliseHash(Hash2D(x0, y1, seed));
        const f32 n11 = NormaliseHash(Hash2D(x1, y1, seed));

        const f32 nx0 = Engine::Lerp(n00, n10, sx);
        const f32 nx1 = Engine::Lerp(n01, n11, sx);
        return Engine::Lerp(nx0, nx1, sy) * 2.0f - 1.0f;
    }

    // Fractal sum of Noise2D
    static inline f32 Fbm2D(const f32 x, const f32 y,
                            const u32 octaves = 3,
                            const f32 lacunarity = 2.0f,
                            const f32 gain = 0.5f) {
        f32 sum = 0.0f, amp = 0.5f, freq = 1.0f;
        for (u32 i = 0; i < octaves; ++i) {
            sum += ValueNoise2D(x * freq, y * freq, WorldSeed + i * 0x9E3779B9u) * amp;
            freq *= lacunarity;
            amp  *= gain;
        }
        return sum;
    }
}