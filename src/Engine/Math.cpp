/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine/Math.hpp>

namespace BinF::Engine {
    template <typename T>
    T Clamp(T val, T min, T max) {
        return (val < min ? min : (val > max ? max : val));
    }
}