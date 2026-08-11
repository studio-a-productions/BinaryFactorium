#pragma once

#include <BinF/Engine/Platform.hpp>

#if BINF_PLATFORM == FRI3D2024
#include "Fri3d/Badge_pins.h"
#elif BINF_PLATFORM == FRI3D2026
#include "Fri3d/Fri3dXP.hpp"
#endif