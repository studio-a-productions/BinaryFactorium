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
    constexpr f32 TerrainScale  = 0.116f;
    constexpr f32 DirtGrass     = -0.6f;
    constexpr f32 GrassStone    = 0.51f;
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

    // Fractal sum of Noise2D (Fractal Brownian Motion)
    static inline f32 Fbm2D(const f32 x, const f32 y,
                            const u8  octaves = 4,
                            const f32 lacunarity = 2.0f,
                            const f32 gain = 0.5f) {
        f32 sum = 0.0f, amp = 0.5f, freq = 1.0f;
        for (u32 i = 0; i < octaves; i++) {
            sum += ValueNoise2D(x * freq, y * freq, WorldSeed + i * 0x9E3779B9u) * amp;
            freq *= lacunarity;
            amp  *= gain;
        }
        return sum;
    }

    // Sprite Tables
    struct TerrainSprites {
        Tile DirtGrass[16];   // [0] = Pure Dirt    [15] = Pure Grass
        Tile GrassStone[16];  // [0] = Pure Grass   [15] = Pure Stone
    };


    // add later when all sprites are here ...
    constexpr TerrainSprites Sprites = {
        {
            41, 37, 36, 34, 39, 32, 26, 29, 38, 27, 33, 28, 35, 30, 31, 25
        },
        {
            25, 20, 21, 19, 22, 17, 11, 13, 23, 10, 16, 14, 18, 12, 15, 9 
        }
    };

    struct OreSprite {
        Tile Id;
        f32  Scale;     // smaller = bigger
        f32  Threshold; // higher = rarer
        u32  Salt;
    };

    constexpr OreSprite Ores[] = {
        {
            0, 100.f, 2.f, 4
        }
    };

    constexpr u32 OreCount = sizeof(Ores) / sizeof(Ores[0]);

    // raw material from noie (no rule enforcement)
    static inline Terrain SampleRawTerrain(const WorldAxis worldCornerX, const WorldAxis worldCornerY) {
        const f32 n = Fbm2D(
            static_cast<f32>(worldCornerX) * TerrainScale + static_cast<f32>(TerrainSalt),
            static_cast<f32>(worldCornerY) * TerrainScale
        );
        if (n < DirtGrass)  return Terrain::Dirt;
        if (n < GrassStone) return Terrain::Grass;
        return Terrain::Stone;
    }

    // enforce rules
    static inline Terrain CorrectTerrain(const Terrain self, const Terrain up, const Terrain down, const Terrain left, const Terrain right) {
        if (self == Terrain::Dirt &&
            (up == Terrain::Stone || down == Terrain::Stone ||
             left == Terrain::Stone || right == Terrain::Stone))
            return Terrain::Grass;

        if (self == Terrain::Stone &&
            (up == Terrain::Dirt || down == Terrain::Dirt ||
             left == Terrain::Dirt || right == Terrain::Dirt))
            return Terrain::Grass;

        return self;
    }

    static inline void BuildCornerTerrain(const WorldAxis chunkX, const WorldAxis chunkY, Terrain (&out)[ChunkCornerSize][ChunkCornerSize]) {
        Terrain raw[ChunkPadSize][ChunkPadSize];

        const s32 originX = static_cast<s32>(chunkX) * static_cast<s32>(ChunkSize);
        const s32 originY = static_cast<s32>(chunkY) * static_cast<s32>(ChunkSize);

        for (u32 py = 0; py < ChunkPadSize; py++) {
            for (u32 px = 0; px < ChunkPadSize; px++) {
                const s32 worldX = originX + static_cast<s32>(px) - 1;
                const s32 worldY = originY + static_cast<s32>(py) - 1;
                raw[py][px] = SampleRawTerrain(worldX, worldY);
            }
        }

        for (u32 cy = 0; cy < ChunkCornerSize; cy++) {
            for (u32 cx = 0; cx < ChunkCornerSize; cx++) {
                const u32 px = cx + 1, py = cy + 1;
                out[cy][cx] = CorrectTerrain(
                    raw[py][px],
                    raw[py - 1][px], raw[py + 1][px],
                    raw[py][px - 1], raw[py][px + 1]
                );
            }
        }
    }

    static inline u8 MakeMask(const Terrain tl, const Terrain tr, const Terrain bl, const Terrain br, const Terrain highMat) {
        u8 mask = 0;
        if (tl == highMat) mask |= 0x1;
        if (tr == highMat) mask |= 0x2;
        if (bl == highMat) mask |= 0x4;
        if (br == highMat) mask |= 0x8;
        return mask;
    }

    static inline Tile ResolveTile(const Terrain (&corners)[ChunkCornerSize][ChunkCornerSize], const u32 tx, const u32 ty) {
        const Terrain tl = corners[ty][tx];
        const Terrain tr = corners[ty][tx + 1];
        const Terrain bl = corners[ty + 1][tx];
        const Terrain br = corners[ty + 1][tx + 1];

        const bool hasDirt  = (tl == Terrain::Dirt)  || (tr == Terrain::Dirt)  || (bl == Terrain::Dirt)  || (br == Terrain::Dirt);
        const bool hasStone = (tl == Terrain::Stone) || (tr == Terrain::Stone) || (bl == Terrain::Stone) || (br == Terrain::Stone);

        // diagonal check
        if (hasDirt && hasStone) return Sprites.DirtGrass[15]; 

        if (hasDirt) {
            return Sprites.DirtGrass[MakeMask(tl, tr, bl, br, Terrain::Grass)];
        }
        if (hasStone) {
            return Sprites.GrassStone[MakeMask(tl, tr, bl, br, Terrain::Stone)];
        }
        return Sprites.DirtGrass[15];
    }


    static inline void ApplyOrePass(ChunkData& chunk, const WorldAxis chunkX, const WorldAxis chunkY) {
        const s32 originTileX = static_cast<s32>(chunkX) * static_cast<s32>(ChunkSize);
        const s32 originTileY = static_cast<s32>(chunkY) * static_cast<s32>(ChunkSize);

        for (u32 ty = 0; ty < ChunkSize; ty++) {
            for (u32 tx = 0; tx < ChunkSize; tx++) {
                Tile& tile = chunk.Tiles[ty * ChunkSize + tx];
                if (tile != Sprites.GrassStone[15]) continue; /* only full stones */

                const f32 wx = static_cast<f32>(originTileX + static_cast<s32>(tx));
                const f32 wy = static_cast<f32>(originTileY + static_cast<s32>(ty));

                for (u32 i = 0; i < OreCount; i++) {
                    const OreSprite& ore = Ores[i];
                    const f32 n = Fbm2D(wx * ore.Scale + static_cast<f32>(ore.Salt), wy * ore.Scale, 6U);
                    if (n > ore.Threshold) {
                        tile = ore.Id;
                        break;
                    }
                }
            }
        }
    }


    ChunkData* GenerateChunk(const WorldAxis x, const WorldAxis y) {
        ChunkData* chunk = Engine::New<ChunkData>(Engine::MemType::Internal);
        chunk->x = x;
        chunk->y = y;

        // pass 1
        Terrain corners[ChunkCornerSize][ChunkCornerSize];
        BuildCornerTerrain(x, y, corners);

        for (u32 ty = 0; ty < ChunkSize; ty++)
            for (u32 tx = 0; tx < ChunkSize; tx++)
                chunk->Tiles[ty * ChunkSize + tx] = ResolveTile(corners, tx, ty);

        // pass 2
        ApplyOrePass(*chunk, x, y);

        // pas 3 later for vegit

        return chunk;
    }

}