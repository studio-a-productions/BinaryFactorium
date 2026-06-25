#pragma once

#include "Colour.hpp"

namespace BinF::Engine {
    using colourID = u8;
    constexpr colourID nac = 0;

    const colour colourPalatte = {
        0x00 // can be anything (ID 0 == no colour/transparent)
    };
}