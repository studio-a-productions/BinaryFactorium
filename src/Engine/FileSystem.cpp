/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/
#include <BinF/Engine.hpp>
#include <BinF/Engine/FileSystem.hpp>
#include <Fri3dBadge_pins.h>
#include <SPI.h>
#include <SD.h>

#define ONE_KB 1024U

namespace BinF::Engine {
    constexpr FilePath TableFileName = "/table.bff";
    constexpr FileID MaxOpenFiles = 16U;
    constexpr u32 MaxFileName = FILENAME_MAX-24U;
    constexpr u16 MaxFiles = 100U;
    constexpr u32 TableBufferSize = 100U;
    constexpr u64 FSMinSize = ONE_KB*5000ULL;   /* ~ 5 MB  */
    constexpr u32 MaxFileSize = ONE_KB*10U;     /* ~ 10 KB */ 

    struct FileSysImpl {
        FileSystemTable Table;
        FilePath* FileNames;
        File* Files;
        char NameBuffer[FILENAME_MAX];
    };

    template<typename T>
    void WriteElement(u8*& ptr, const T& value) {
        memcpy(ptr, &value, sizeof(T));
        ptr += sizeof(T);
    }

    template<typename T>
    void ReadElement(u8*& ptr, T& value) {
        memcpy(&value, ptr, sizeof(T));
        ptr += sizeof(T);
    }

    class FileSystemTable {
    public:
        FileSystemTable() = default;
        FSResult Begin() {
            if (!m_Buffer) {
                m_Buffer = Calloc<u8>(TableBufferSize, MemType::External);
                if (!m_Buffer) {
                    Logger.Crit("(FileSys) Failed to allocate buffer of size %d (bytes)", sizeof(const char**)*MaxOpenFiles);
                    return FSResult::IOError;
                }
            } else Logger.Warn("(FileSys) Buffer Already Allocated");

            u8* p = m_Buffer;
            if (!SD.exists(TableFileName)) {
                m_Table = SD.open(TableFileName, FILE_WRITE, true);
                if (!m_Table.availableForWrite() || !m_Table.available()) {
                    m_Table.close();
                    Logger.Crit("(FileSys) Unable to write to SD");
                    return FSResult::IOError;
                }

                m_Space = SD.totalBytes();
                m_Used  = SD.usedBytes();
                m_Files = 0U;

                if (m_Space-m_Used < FSMinSize) return FSResult::NoSpace;

                
                WriteElement(p, m_Space);
                WriteElement(p, m_Used);
                WriteElement(p, m_Files);

                m_Table.write(m_Buffer, TableBufferSize);
                m_Table.close();

                if (!SD.mkdir("/binf")) Logger.Warn("(FileSys) Couldn't mkdir");
            } else {
                m_Table = SD.open(TableFileName, FILE_READ);

                if (!m_Table.available()) return FSResult::IOError;

                m_Table.readBytes((char*)m_Buffer, TableBufferSize);
                m_Table.close();

                ReadElement(p, m_Space);
                ReadElement(p, m_Used);
                ReadElement(p, m_Files);
                
            }
            return FSResult::Ok;
        }
    private:
        File m_Table;
        u8* m_Buffer = nullptr;
        u64 m_Space;
        u64 m_Used;
        u16 m_Files;
    };

    // FileHandle Impl ---------------------------------
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

    FileSystemClass::FileSystemClass() : m_State{FSState::Good} { }
    FileSystemClass::~FileSystemClass() { }

    FSState FileSystemClass::Begin() {
        m_Impl.FileNames = Calloc<FilePath>(MaxOpenFiles, MemType::External);
        if (!m_Impl.FileNames) {
            Logger.Crit("(FileSys) Failed to allocate buffer of size %zu (bytes)", sizeof(const char**)*MaxOpenFiles);
            return Bad();
        }
        m_Impl.Files = Calloc<File>(MaxOpenFiles, MemType::External);
        if (!m_Impl.Files) {
            Logger.Crit("(FileSys) Failed to allocate buffer of size %zu (bytes)", sizeof(File*)*MaxOpenFiles);
            return Bad();
        }


        if (!SD.begin(PIN_SDCARD_CS, SPI, SPI_FREQUENCY, "/sd", MaxFiles, true)) {
            Logger.Crit("(FileSys) Failed to init SD");
            return Bad();
        }

        if (m_Impl.Table.Begin() != FSResult::Ok) {
            Logger.Crit("(FileSys) Couldn't init FSTable");
            return Bad();
        }

        m_State = FSState::Good;
        return FSState::Good;
    }

    FSState FileSystemClass::State() const { return m_State; }

    FileID FileSystemClass::NewFileID(FilePath path) {
        if (path)
        for (u16 i = 0; i < MaxOpenFiles; i++) {
            if (!m_Impl.FileNames[i]) {
                m_Impl.FileNames[i] = path;
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
        if (length > 5U)
            return (*path == '/') && (strcmp(&path[length-5U], FSExtension) == 0);
        else return false;
    }

    FileID FileSystemClass::GetFileID(FilePath path) {
        if (PathValid(path)) return NewFileID(path);
        else return FileInvalid;
    }

    bool FileSystemClass::FileExists(FilePath path) const {
        if (!PathValid(path)) return false;
        return SD.exists(path);
    }

}