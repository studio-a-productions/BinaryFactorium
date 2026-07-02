/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include <Arduino.h>

namespace BinF {

    // std types -------------------

    using u64 = uint64_t;
    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8  = uint8_t;

    using s64 = int64_t;
    using s32 = int32_t;
    using s16 = int16_t;
    using s8  = int8_t;

    using f32 = float_t;
    using f64 = double_t;

    constexpr u64 u64m = 0xFFFFFFFFFFFFFFFF;
    constexpr u32 u32m = 0xFFFFFFFF;
    constexpr u16 u16m = 0xFFFF;
    constexpr u8  u8m  = 0xFF;
}