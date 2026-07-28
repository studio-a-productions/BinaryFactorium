/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/
#include <BinF/Engine.hpp>
#include <BinF/Engine/FileSystem.hpp>
#include <Fri3dBadge_pins.h>
#include <SPI.h>
#include <SD.h>

constexpr BinF::u32 ONE_KB = 1024;

namespace BinF::Engine {
    constexpr FilePath TableFileName = "/table.bff";
    constexpr FileID MaxOpenFiles = 16U;
    constexpr u32 MaxFileName = FILENAME_MAX-24U;
    constexpr u16 MaxFiles = 100U;
    constexpr u32 TableBufferSize = 100U;
    constexpr u64 FSMinSize = ONE_KB*5000ULL;   /* ~ 5 MB  */
    constexpr u32 MaxFileSize = ONE_KB*10U;     /* ~ 10 KB */

    using FileName = char[FILENAME_MAX];
    
    FileHandle InvalidFile = FileHandle();

    inline FSResult 
    CouldNotCreate(FilePath path) {
        Logger.Error("(FileSys) Couldn't Create %s", path);
        return FSResult::IOError;
    }

    struct FileSysImpl {
        char FileNames[MaxOpenFiles][FILENAME_MAX];
        File Files[MaxOpenFiles];
        char* Buffer;
    };

    // FileHandle Impl ---------------------------------
    FileHandle::~FileHandle() {
        if (IsValid()) m_fs.FreeID(m_id);
    }

    bool FileHandle::IsValid() const {
        return (m_id != FileInvalid);
    }

    u32 FileHandle::Size() const {
        return m_fs.FileSize(m_id);
    }

    FSResult FileHandle::Read(void*dest, u32 size, u32 offset) const {
        return m_fs.ReadFile(m_id, dest, size, offset);
    }

    FSResult FileHandle::Write(const void*dat, u32 size, u32 offset) const {
        return m_fs.WriteFile(m_id, dat, size, offset);
    }

    FSResult FileHandle::Rename(FilePath path) const {
        return m_fs.RenameFile(m_id, path);
    }

    FSResult FileHandle::Delete() {
        FSResult rs = m_fs.DeleteFile(m_id);
        if (rs != FSResult::Ok)
            Logger.Error("(FileSys) Deleting file (id:%u) error '%u'", m_id, rs);
        else if ((rs=m_fs.FreeID(m_id)) != FSResult::Ok)
            Logger.Error("(FileSys) Freeing (id:%u) error '%u'", m_id, rs);
        m_id = FileInvalid; // discard id always (if something goes wrong, it probably doesn't matter)
        return rs;
    }






    // FileSys Impl ------------------------------------

    FileSystemClass::FileSystemClass() : m_State{FSState::Good}, m_Impl{FileSysImpl()} { }
    FileSystemClass::~FileSystemClass() {
        Free<char>(m_Impl.Buffer);
    }

    FSState FileSystemClass::Begin() {
        m_Impl.Buffer = Calloc<char>(MaxFileSize, MemType::External);
        if (!m_Impl.Buffer) {
            Logger.Crit("(FileSys) Failed to allocate buffer of size %zu (bytes)", sizeof(char)*MaxFileSize);
            return Bad();
        }

        if (!SD.begin(PIN_SDCARD_CS, SPI, SPI_FREQUENCY, "/sd", MaxFiles, true)) {
            Logger.Crit("(FileSys) Failed to init SD");
            return Bad();
        }

        m_State = FSState::Good;
        return FSState::Good;
    }

    FSState FileSystemClass::State() const { return m_State; }

    FileID FileSystemClass::NewFileID(FilePath path) {
        if (path && PathValid(path))
        for (u16 i = 0; i < MaxOpenFiles; i++) {
            if (m_Impl.FileNames[i][0] == '\0') {
                strcpy(&m_Impl.FileNames[i][0], path);
                m_Impl.Files[i] = SD.open(path, FILE_READ, !SD.exists(path));
                return i+1;
            }
        }
        return FileInvalid;
    }

    FSState FileSystemClass::Bad() {
        return m_State = FSState::Bad;
    }

    bool FileSystemClass::PathValid(FilePath path) const {
        const u32 length = strlen(path);
        if (length > MaxFileName) return false;
        if (*path != '/') return false;
        return true;
    }

    FilePath Extend(FilePath) {

    }

    bool FileSystemClass::IdValid(FileID id) const {
        if (id-- == FileInvalid) return false;
        if (id >= MaxOpenFiles) return false;
        return m_Impl.FileNames[id][0] != '\0';
    }

    FileID FileSystemClass::GetFileID(FilePath path) {
        if (PathValid(path)) return NewFileID(path);
        else return FileInvalid;
    }

    bool FileSystemClass::FileExists(FilePath path) const {
        if (!PathValid(path)) return false;
        return SD.exists(path);
    }

    FSResult FileSystemClass::FreeID(FileID id) {
        if (!IdValid(id--))
            return FSResult::NotFound;
        m_Impl.FileNames[id][0] = '\0';
        if (m_Impl.Files[id]) m_Impl.Files[id].close();
        return FSResult::Ok;
    }

    u32 FileSystemClass::FileSize(FilePath path) const {
        if (!PathValid(path)) return 0U;
        File t_File=SD.open(path);
        if (!t_File) return 0U;
        u32 t_siz = t_File.size();
        t_File.close();
        return t_siz;
    }

    u32 FileSystemClass::FileSize(FileID id) const {
        if (IdValid(id--))
            return m_Impl.Files[id].size();
        else return 0U;
    }

    FSResult FileSystemClass::ReadFile(FileID id, void* dest, u32 size, const u32 offset) {
        if (!IdValid(id--)) return FSResult::NotFound;
        if (!size) size = offset ? m_Impl.Files[id].size() - offset : m_Impl.Files[id].size();
        if (!m_Impl.Files[id].available()) return FSResult::IOError;
        m_Impl.Files[id].seek(offset);
        m_Impl.Files[id].read((u8*)dest, size);
        return FSResult::Ok;
    }

    FSResult FileSystemClass::WriteFile(FileID id, const void* data, u32 size, u32 offset) {
        if (!IdValid(id--)) return FSResult::NotFound;
        if (!size) size = offset ? m_Impl.Files[id].size() - offset : m_Impl.Files[id].size();
        if (size+offset > MaxFileSize) return FSResult::NoSpace;
        m_Impl.Files[id].close();
        m_Impl.Files[id] = SD.open(m_Impl.FileNames[id], FILE_WRITE);
        // error checks
        if (!m_Impl.Files[id]) return FSResult::IOError;
        if (!m_Impl.Files[id].availableForWrite()) {
            m_Impl.Files[id].close();
            Bad();
            return FSResult::IOError;
        }
        FSResult res = FSResult::Ok;
        m_Impl.Files[id].seek(offset);
        if (m_Impl.Files[id].write((const u8*)data, size) != size) {
            Logger.Error("(FileSys) Write Error '%d'", m_Impl.Files[id].getWriteError());
            res = FSResult::IOError;
        }

        m_Impl.Files[id].close();
        m_Impl.Files[id] = SD.open(m_Impl.FileNames[id]);
        return m_Impl.Files[id] ? res : FSResult::IOError;
    }

    FSResult FileSystemClass::RenameFile(FileID id, FilePath path) {
        if (!IdValid(id--)) return FSResult::NotFound;
        if (!SD.rename(m_Impl.FileNames[id], path)) {
            Bad();
            return FSResult::IOError;
        }
        
        strcpy(m_Impl.FileNames[id], path);

        return FSResult::Ok;
    }

    FSResult FileSystemClass::DeleteFile(FileID id) {
        if (!IdValid(id--)) return FSResult::NotFound;
        if (m_Impl.Files[id]) m_Impl.Files[id].close();

        SD.remove(m_Impl.FileNames[id]);

        return FreeID(++id);
    }

    FileHandle& FileSystemClass::GetFile(FilePath path) {
        const FileID newid = GetFileID(path);
        if (newid == FileInvalid) return InvalidFile;
        return *New<FileHandle>(*this, newid);
    }

    FileHandle& FileSystemClass::CreateFile(FilePath path, u32 size) {
        /* This is an invalid opperation! */
        if (SD.exists(path)) return InvalidFile;
        const auto newid = GetFileID(path);
        if (newid == FileInvalid) return InvalidFile;
        return *New<FileHandle>(*this, newid);
    }
    
}