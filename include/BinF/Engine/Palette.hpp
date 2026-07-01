#pragma once

#include "Colour.hpp"

namespace BinF::Engine {
    using colourID = u8;
    constexpr colourID NaC = 0;
    constexpr u8 PaletteSize = u8m;

    // user MUST change their palette here (for now)
    // later the engine may add additional palette support (runtime loaded) it'll though slower
    const colour ColourPalette[PaletteSize] = { 
        0U,     // indx 0 reserved in engine
    };
}