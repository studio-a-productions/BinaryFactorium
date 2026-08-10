/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once 

#include "common.hpp"

namespace BinF::Engine {
    template<typename T>
    T Clamp(T val, T min, T max);
    template<typename T>
    constexpr T Sqr(const T a) {
        return a*a;
    }
    constexpr f32 Lerp(const f32 a, const f32 b, const f32 t) {
        return a + (b - a) * t;
    }
}