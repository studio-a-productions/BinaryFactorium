/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include "FSParser.hpp"

namespace BinF::Engine {

    /*
        * (start keyword)
        \ (end keyword)
        \n (scope terminator = newline character)
        \0 (end of file/all scopes, optional if all scopes are properly closed)

        keyword types: 
        - scope definers (start/end scope)
        - meta (metadata, non-scope, defined sizes of data they'll skip/read over)
        - scope-data (data of a certain scope, same rules of meta but need be within a scope exclusively)
        - unknown/newer (these always require the next byte to be read for its type, though that byte is secretly anywhere)
        max-keyword-size: 100 characters

        extra syntax: the first byte after a keyword signals the type, which MUST be char syntax. 
            For now, we only have:
                ':' meaning the it enters a scope which ends with \n (scope terminator, everything inside it gets ignored)
                '!' meaning the next byte gives the amount of bytes to skip/read over
                '%' meaning the data is string-format (a special kind of scope ending with \0)
                'l' meaning the space it takes up is 512 bits of data
                'd' meaning the rest of the data is 256 bits of data (default spacing)
                'g' meaning the rest of the data is 64 bits ("great" spacing)
                'w' meaning the rest of the data is 32 bits (word spacing)
                'h' meaning the rest of the data is 16 bits (halfword)
                'b' meaning the rest of the data is only a single byte
                'e' meaning that if the keyword is not recognised, you must quit
            prefered for new keywords: 
                either:
                    use l or d and duplicate the data within it to have copies/reserve
                    use ':' and then have keywords with defined sizes

        *fsv\ (file system version (bffs, BinF FS), this thingy, 32 bit number)
        *fls\ (files, 32 bit number)
        *dsp\ (disk space, 64 bit number)
        *dus\ (disk used space, 64 bit number)


        *ros\ (read-only scope, can be switched around to be placed into dic)
        *dic\ (enter dictionary scope) dictionary (scope's name)

        *fil\ (filescope, filename) \n (end file scope, automatically appends .bff)
        *fil\ name *dat\ (date, non-scope keyword, 256 bits due to it being ignored) \n (add date to file, unused in current impl)
        \n (end dictionary scope)
        \n (end read-only scope)
    */
    constexpr u32  MaxKeywordSize       = 100U;
    constexpr char KeywordStart         = '*';
    constexpr char KeywordEnd           = '\\';
    constexpr char ScopeEnd             = '\n';
    constexpr char StringEnd            = '\0';
    constexpr char EndOfFile            = '\0';
    // defined keywords
    constexpr char KeywordFSVersion[]   = "fsv"; // 32 bit
    constexpr char KeywordFiles[]       = "fls"; // 32 bit
    constexpr char KeywordSpace[]       = "dsp"; // 64 bit
    constexpr char KeywordUSpace[]      = "dus"; // 64 bit
    constexpr char KeywordReadOnly[]    = "ros"; // scopes
    constexpr char KeywordDictionary[]  = "dic"; // scopes
    constexpr char KeywordFile[]        = "fil"; // scopes
    constexpr char KeywordNameAttr[]    = "nam"; // string
    
    constexpr u32 s = sizeof(char);

    enum class BFFSType : char {
        Scope   = ':',
        String  = '%',
        Sized   = '!',
        Long    = 'l',
        Great   = 'g',
        Default = 'd',
        Word    = 'w',
        Halfword= 'h',
        Byte    = 'b',
        Error   = 'e'
    };

    enum class BFFKeyword : BinF::u8 {
        FSVersion,
        FileAmount,
        DiskSpace,
        DiskUsed,
        ReadOnly,
        Dictionary,
        File,
        AttrName,
        Unknown
    };

    File m_Table;
    char KeywordBuffer[MaxKeywordSize] = {'\0'};

BFFSDir* ParseDir(bool readonly);
    BFFSFile* ParseFile(bool readonly);
    void ParseDirContent(BFFSDir* dir, bool currentReadOnly);

    void AppendDir(BFFSDir*& head, BFFSDir* dir) {
        if (!head) {
            head = dir;
        } else {
            BFFSDir* curr = head;
            while (curr->next) curr = curr->next;
            curr->next = dir;
        }
    }

    void AppendFile(BFFSFile*& head, BFFSFile* file) {
        if (!head) {
            head = file;
        } else {
            BFFSFile* curr = head;
            while (curr->next) curr = curr->next;
            curr->next = file;
        }
    }

    BFFKeyword ReadKeyword(BFFSType& outType) {
        u32 idx = 0;
        char c;
        while (m_Table.available()) {
            c = m_Table.read();
            if (c == KeywordEnd) break;
            if (idx < MaxKeywordSize - 1) {
                KeywordBuffer[idx++] = c;
            }
        }
        KeywordBuffer[idx] = '\0';

        if (m_Table.available()) {
            outType = static_cast<BFFSType>(m_Table.read());
        } else {
            outType = BFFSType::Error;
        }

        if (strcmp(KeywordBuffer, KeywordFSVersion) == 0) return BFFKeyword::FSVersion;
        if (strcmp(KeywordBuffer, KeywordFiles) == 0) return BFFKeyword::FileAmount;
        if (strcmp(KeywordBuffer, KeywordSpace) == 0) return BFFKeyword::DiskSpace;
        if (strcmp(KeywordBuffer, KeywordUSpace) == 0) return BFFKeyword::DiskUsed;
        if (strcmp(KeywordBuffer, KeywordReadOnly) == 0) return BFFKeyword::ReadOnly;
        if (strcmp(KeywordBuffer, KeywordDictionary) == 0) return BFFKeyword::Dictionary;
        if (strcmp(KeywordBuffer, KeywordFile) == 0) return BFFKeyword::File;
        if (strcmp(KeywordBuffer, KeywordNameAttr) == 0) return BFFKeyword::AttrName;
        
        return BFFKeyword::Unknown;
    }

    void SkipUnknown(BFFSType type) {
        switch (type) {
            case BFFSType::Scope: {
                while (m_Table.available()) {
                    if (m_Table.read() == ScopeEnd) break;
                }
                break;
            }
            case BFFSType::String: {
                while (m_Table.available()) {
                    if (m_Table.read() == StringEnd) break;
                }
                break;
            }
            case BFFSType::Sized: {
                if (m_Table.available()) {
                    u8 skipLen = m_Table.read();
                    m_Table.seek(m_Table.position() + skipLen);
                }
                break;
            }
            case BFFSType::Long:     m_Table.seek(m_Table.position() + 64); break; 
            case BFFSType::Default:  m_Table.seek(m_Table.position() + 32); break; 
            case BFFSType::Word:     m_Table.seek(m_Table.position() + 4);  break; 
            case BFFSType::Halfword: m_Table.seek(m_Table.position() + 2);  break; 
            case BFFSType::Byte:     m_Table.seek(m_Table.position() + 1);  break; 
            default:
                break;
        }
    }

    char* ReadInlineName(char firstChar) {
        char buf[256];
        int idx = 0;
        buf[idx++] = firstChar;
        
        while (m_Table.available()) {
            char peeked = m_Table.peek();
            if (peeked == KeywordStart || peeked == ScopeEnd || peeked == ' ' || peeked == '\r' || peeked == '\t') {
                break;
            }
            buf[idx++] = m_Table.read();
            if (idx >= 255) break;
        }
        buf[idx] = '\0';
        char* nameStr = new char[idx + 1];
        strcpy(nameStr, buf);
        return nameStr;
    }

    char* ReadStringAttr() {
        char buf[256];
        int idx = 0;
        while (m_Table.available()) {
            char c = m_Table.read();
            if (c == StringEnd) break;
            if (idx < 255) buf[idx++] = c;
        }
        buf[idx] = '\0';
        char* nameStr = new char[idx + 1];
        strcpy(nameStr, buf);
        return nameStr;
    }

    void CreateTableFile(FilePath path) {
        File createTable = SD.open(path, FILE_WRITE);
        if (createTable) {
            createTable.print("*fsv\\w");
            u32 defaultVer = 1;
            createTable.write((uint8_t*)&defaultVer, 4);
            createTable.close();
        }
    }

    BFFKeyword ParseToken(u32 offset) {
        m_Table.seek(offset);
        m_Table.read((uint8_t*)KeywordBuffer, MaxKeywordSize);
        if (strcmp(KeywordBuffer, KeywordFSVersion) == 0) return BFFKeyword::FSVersion;
        if (strcmp(KeywordBuffer, KeywordFiles) == 0) return BFFKeyword::FileAmount;
        if (strcmp(KeywordBuffer, KeywordSpace) == 0) return BFFKeyword::DiskSpace;
        if (strcmp(KeywordBuffer, KeywordUSpace) == 0) return BFFKeyword::DiskUsed;
        if (strcmp(KeywordBuffer, KeywordReadOnly) == 0) return BFFKeyword::ReadOnly;
        if (strcmp(KeywordBuffer, KeywordDictionary) == 0) return BFFKeyword::Dictionary;
        if (strcmp(KeywordBuffer, KeywordFile) == 0) return BFFKeyword::File;
        return BFFKeyword::Unknown;
    }

    // Parses contents inside a directory context frame
    void ParseDirContent(BFFSDir* dir, bool currentReadOnly) {
        while (m_Table.available()) {
            char c = m_Table.read();
            if (c == ScopeEnd) {
                break; // Safely breaks out of either the active 'ros' or 'dic' boundary frame
            }

            if (c == KeywordStart) {
                BFFSType type;
                BFFKeyword kw = ReadKeyword(type);
                switch (kw) {
                    case BFFKeyword::ReadOnly: {
                        if (type == BFFSType::Scope) {
                            // Recursively execute frame with explicit read-only configuration
                            ParseDirContent(dir, true); 
                        } else {
                            SkipUnknown(type);
                        }
                        break;
                    }
                    case BFFKeyword::Dictionary: {
                        BFFSDir* child = ParseDir(currentReadOnly);
                        AppendDir(dir->children, child);
                        break;
                    }
                    case BFFKeyword::File: {
                        BFFSFile* file = ParseFile(currentReadOnly);
                        AppendFile(dir->files, file);
                        break;
                    }
                    case BFFKeyword::AttrName: {
                        if (dir->name) delete[] dir->name;
                        dir->name = ReadStringAttr();
                        break;
                    }
                    case BFFKeyword::Unknown:
                        SkipUnknown(type);
                        break;
                    default:
                        break;
                }
            } else if (c != ' ' && c != '\r' && c != '\t') {
                if (!dir->name) {
                    dir->name = ReadInlineName(c);
                }
            }
        }
    }

    BFFSDir* ParseDir(bool readonly) {
        BFFSDir* dir = new BFFSDir();
        memset(dir, 0, sizeof(BFFSDir));
        dir->readonly = readonly;

        ParseDirContent(dir, readonly);
        return dir;
    }

    // Parses contents inside a file context frame
    void ParseFileContent(BFFSFile* file, bool currentReadOnly) {
        while (m_Table.available()) {
            char c = m_Table.read();
            if (c == ScopeEnd) {
                break;
            }

            if (c == KeywordStart) {
                BFFSType type;
                BFFKeyword kw = ReadKeyword(type);
                switch (kw) {
                    case BFFKeyword::ReadOnly: {
                        if (type == BFFSType::Scope) {
                            file->readonly = true;
                            ParseFileContent(file, true);
                        } else {
                            SkipUnknown(type);
                        }
                        break;
                    }
                    case BFFKeyword::AttrName: {
                        if (file->name) delete[] file->name;
                        file->name = ReadStringAttr();
                        break;
                    }
                    case BFFKeyword::Unknown:
                        SkipUnknown(type);
                        break;
                    default:
                        break;
                }
            } else if (c != ' ' && c != '\r' && c != '\t') {
                if (!file->name) {
                    file->name = ReadInlineName(c);
                }
            }
        }
    }

    BFFSFile* ParseFile(bool readonly) {
        BFFSFile* file = new BFFSFile();
        memset(file, 0, sizeof(BFFSFile));
        file->readonly = readonly;

        ParseFileContent(file, readonly);

        if (file->name) {
            size_t len = strlen(file->name);
            if (len < 4 || strcmp(file->name + len - 4, ".bff") != 0) {
                char* appendedName = new char[len + 5];
                strcpy(appendedName, file->name);
                strcat(appendedName, ".bff");
                delete[] file->name;
                file->name = appendedName;
            }
        }
        return file;
    }

    // Root table framing loop
    void ParseTableContent(BFFSTable* table, bool currentReadOnly, bool exitOnScopeEnd) {
        while (m_Table.available()) {
            char c = m_Table.read();
            if (c == ScopeEnd) {
                if (exitOnScopeEnd) break;
                continue;
            }

            if (c == KeywordStart) {
                BFFSType type;
                BFFKeyword kw = ReadKeyword(type);
                switch (kw) {
                    case BFFKeyword::FSVersion:
                        m_Table.read((uint8_t*)&table->version, 4);
                        break;
                    case BFFKeyword::FileAmount:
                        m_Table.read((uint8_t*)&table->filecount, 4);
                        break;
                    case BFFKeyword::DiskSpace:
                        m_Table.read((uint8_t*)&table->total, 8);
                        break;
                    case BFFKeyword::DiskUsed:
                        m_Table.read((uint8_t*)&table->used, 8);
                        break;
                    case BFFKeyword::ReadOnly: {
                        if (type == BFFSType::Scope) {
                            // Run a nested root parsing session that terminates on ScopeEnd (\n)
                            ParseTableContent(table, true, true);
                        } else {
                            SkipUnknown(type);
                        }
                        break;
                    }
                    case BFFKeyword::Dictionary: {
                        BFFSDir* dir = ParseDir(currentReadOnly);
                        AppendDir(table->dirs, dir);
                        break;
                    }
                    case BFFKeyword::File: {
                        BFFSFile* file = ParseFile(currentReadOnly);
                        AppendFile(table->files, file);
                        break;
                    }
                    case BFFKeyword::Unknown:
                        SkipUnknown(type);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    BFFSTable* ParseTable(FilePath path) {
        if (!SD.exists(path)) CreateTableFile(path);
        m_Table = SD.open(path, FILE_READ);
        if (!m_Table) return nullptr;

        BFFSTable* table = new BFFSTable();
        memset(table, 0, sizeof(BFFSTable));

        // Start table execution frame (do not exit on structural root scope increments)
        ParseTableContent(table, false, false);
        
        m_Table.close();
        return table;
    }

    void WriteDirRecursive(File& file, BFFSDir* dir) {
        if (!dir) return;
        if (dir->readonly) file.print("*ros\\:");
        
        file.print("*dic\\:");
        if (dir->name) {
            file.print(dir->name);
            file.print(" ");
        }
        
        BFFSDir* child = dir->children;
        while (child) {
            WriteDirRecursive(file, child);
            child = child->next;
        }

        BFFSFile* childFile = dir->files;
        while (childFile) {
            if (childFile->readonly && !dir->readonly) {
                file.print("*ros\\:");
            }
            file.print("*fil\\:");
            if (childFile->name) file.print(childFile->name);
            file.print("\n");
            if (childFile->readonly && !dir->readonly) {
                file.print("\n");
            }
            childFile = childFile->next;
        }

        file.print("\n"); 
        if (dir->readonly) file.print("\n"); 
    }

    bool WriteTable(FilePath path, BFFSTable table) {
        SD.remove(path);
        File file = SD.open(path, FILE_WRITE);
        if (!file) return false;

        file.print("*fsv\\w"); file.write((uint8_t*)&table.version, 4);
        file.print("*fls\\w"); file.write((uint8_t*)&table.filecount, 4);
        file.print("*dsp\\g"); file.write((uint8_t*)&table.total, 8);
        file.print("*dus\\g"); file.write((uint8_t*)&table.used, 8);

        BFFSDir* currDir = table.dirs;
        while (currDir) {
            WriteDirRecursive(file, currDir);
            currDir = currDir->next;
        }

        BFFSFile* currFile = table.files;
        while (currFile) {
            if (currFile->readonly) file.print("*ros\\:");
            file.print("*fil\\:");
            if (currFile->name) file.print(currFile->name);
            file.print("\n");
            if (currFile->readonly) file.print("\n");
            currFile = currFile->next;
        }

        file.close();
        return true;
    }
}