/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "common.hpp"

namespace BinF::Engine {
    // a number representing an active file within a FS
    using FileID = u32;
    // basically path strings for names (unix-like)
    using FilePath = const char*;

    // all fs use "file 0" as the lookup table or just as an invalid ID
    constexpr FileID FileInvalid = 0;

    enum class FSResult : BinF::u8 {
        Ok = 0,
        NotFound,
        IOError
    };
    enum class FSState : BinF::u8 {
        Good = 0,
        Bad
    };


    // handle of a specific file for a FS
    class FileHandle;


    // File System (IO handle)
    // may later get virtual functions and logic
    class FileSystemClass;


    class FileSystemClass {
    public:
        FileSystemClass();
        ~FileSystemClass();

        FSState 
        Begin();

        FSState
        State();
        
        FileHandle
        GetFile(FilePath);
        
        FileID 
        GetFileID(FilePath);
        
        bool 
        FileExists(FilePath);

        // size of 0 means error/invalid
        u32 FileSize(FilePath);
        u32 FileSize(FileID);

        FileHandle
        CreateFile(FilePath, u32 size=256);

        FSResult
        ReadFile(FileID, void* dest, u32 size = 0U /* 0 means full file */, u32 offset=0U);
        
        FSResult
        WriteFile(FileID id, const void*, u32 size /* sizeof data */, u32 offset=0U);

        FSResult
        RenameFile(FileID, FilePath);

        // Deletes file, ID is then invalid as it points to nullptr
        FSResult 
        DeleteFile(FileID);
        // ID is clear for other files, thus invalid (unsafe)
        FSResult
        FreeID(FileID);
    private:
        // opaque handle implementation
        void* m_data;
        u32 m_size;

        // delete old file and move to new
        void MoveFile(FileID, FilePath);
    };

    // default FileSystem implementation of BinF
    extern FileSystemClass FileSystem;

    class FileHandle {
    public:
        FileHandle(FileSystemClass& fs=FileSystem, const FileID fid=0U) : m_fs{fs}, m_id{fid} { }
        bool IsValid() const;
        u32 Size() const;

        FSResult
        Read(void*dest, u32 size=0U /* 0 means full file */, u32 offset = 0U);

        FSResult
        Write(const void* dat, u32 size /* sizeof data */, u32 offset=0U);
        FSResult
        Delete();
        FSResult
        Rename(FilePath);
    private:
        FileID m_id;
        FileSystemClass& m_fs;
    };
}