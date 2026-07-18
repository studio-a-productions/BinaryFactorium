/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include <BinF/Engine.hpp>
#include <SD.h>


namespace BinF::Engine {
    typedef struct BFFSFile {
        struct BFFSFile*next;
        char* name;
        bool readonly;
    } BFFSFile;

    struct BFFSDir {
        BFFSDir*next;
        BFFSDir*children;
        BFFSFile*files;
        char*name;
        bool readonly;
    };

    typedef struct {
        u32 version;
        u32 filecount;
        u64 total;
        u64 used;
        BFFSDir*dirs;
        BFFSFile*files;
    } BFFSTable;


    BFFSTable* ParseTable(FilePath);
    bool WriteTable(FilePath, BFFSTable);

}