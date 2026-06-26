#pragma once 

#include "common.hpp"

namespace BinF::Engine {
    template<typename T>
    T Clamp(T val, T min, T max);
}