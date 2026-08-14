/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
/* BinF */
#define BINF            0x42696E46


#define FRI3D2024       2024
#define FRI3D2026       2026

#define BINF_LOG_ALL    0x69
#define BINF_INFO       5
#define BINF_WARN       4
#define BINF_ERROR      3
#define BINF_CRITICAL   2
#define BINF_NONE       0

#ifndef BINF_PLATFORM
#define BINF_PLATFORM
#error [BinF] BinF::Engine requires platform specification
#endif

#ifndef BINF_LOGLEVEL
#define BINF_LOGLEVEL BINF_INFO
#ifndef BINF_LOGLEVEL_UNDEFINED
#define BINF_LOGLEVEL_UNDEFINED
#warning [BinF] No level given, defaulting to BINF_INFO
#endif
#endif

