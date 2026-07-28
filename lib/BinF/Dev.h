/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#ifndef X_BINF_DEVCOMMON_INCLUDED
#define X_BINF_DEVCOMMON_INCLUDED

#include <Arduino.h>


typedef void (*BinFCallback)(int);
typedef void* (*BinFAlloc)(const size_t);

#endif