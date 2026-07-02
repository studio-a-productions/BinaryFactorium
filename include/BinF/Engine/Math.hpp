/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once 

#include "common.hpp"

namespace BinF::Engine {
    template<typename T>
    T Clamp(T val, T min, T max);
}