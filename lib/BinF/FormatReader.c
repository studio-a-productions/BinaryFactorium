/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include "FormatReader.h"


static BinFCallback bffr_ecb            = NULL;
static BFFR_CursorFunc bffr_cursorf     = NULL;
static BFFR_ReadFunc bffr_readf         = NULL;
static BinFAlloc bffr_allocf            = NULL;

/* keyword registry */
static BFFR_KeywordFallback bffr_kwf[BFFR_KEYWORD_MAX] = { NULL };
static char bffr_keywords[BFFR_KEYWORD_MAX][BFFR_KEYWORD_SIZE+1U] = { '\0' };
static BFFR_Type bffr_kwtypes[BFFR_KEYWORD_MAX];

static char bffr_kwbuffer[BFFR_KEYWORD_SIZE+1U] = { '\0' };
static char bffr_stringbuffer[BFFR_STRING_SIZE] = { '\0' };

static uint32_t bffr_kwcount   = 0U;

static inline uint32_t BFFR_FindKeyword() {
    for (uint32_t i = 0U; i < bffr_kwcount; i++) {
        if (strcmp(bffr_kwbuffer, bffr_keywords[i]) == 0) return i;
    }
    return BFFR_KEYWORD_MAX;
}

static inline bool BFFR_KeywordExists(const char* kw_name) {
    for (uint32_t i = 0U; i < bffr_kwcount; i++) {
        if (strcmp(kw_name, bffr_keywords[i]) == 0) return true;
    }
    return false;
}

static inline size_t BFFR_GetSize(BFFR_Type kw_type) {
    switch (kw_type) {
    case BFFR_TYPE_DEFAULT:     return BFFR_SIZE_DEFAULT;
    case BFFR_TYPE_GREATER:     return BFFR_SIZE_GREATER;
    case BFFR_TYPE_LONG:        return BFFR_SIZE_LONG;
    case BFFR_TYPE_WORD:        return BFFR_SIZE_WORD;
    case BFFR_TYPE_HALFWORD:    return BFFR_SIZE_HALFWORD;
    case BFFR_TYPE_BYTE:        return BFFR_SIZE_BYTE;
    default:                    return BFFR_SIZE_DEFAULT;
    }
}


void BFFR_SetErrorFallback(BinFCallback func) {
    bffr_ecb = func;
}

void BFFR_SetFileCursorer(BFFR_CursorFunc func) {
    bffr_cursorf = func;
}

void BFFR_SetFileReader(BFFR_ReadFunc func) {
    bffr_readf = func;
}
void BFFR_SetLargeBufferAlloc(BinFAlloc func) {
    bffr_allocf = func;
}


void BFFR_AddKeyword(const char* name, const uint32_t size, const BFFR_Type type, BFFR_KeywordFallback func) {
    if (bffr_kwcount >= BFFR_KEYWORD_MAX) { 
        bffr_ecb(BFFR_EC_LOGICAL);
        return;
    }
    const size_t length = strlen(name);
    if (strlen(name) > BFFR_KEYWORD_SIZE) bffr_ecb(BFFR_EC_KEYWORD_TOO_LONG);

    if (type != BFFR_TYPE_SIZED && type != BFFR_TYPE_SCOPED && type != BFFR_TYPE_STRING && type != BFFR_TYPE_ERROR)
    switch (type) {
    case BFFR_TYPE_DEFAULT:
        if (size != BFFR_SIZE_DEFAULT) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    case BFFR_TYPE_GREATER:
        if (size != BFFR_SIZE_GREATER) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    case BFFR_TYPE_LONG:
        if (size != BFFR_SIZE_LONG) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    case BFFR_TYPE_WORD:
        if (size != BFFR_SIZE_WORD) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    case BFFR_TYPE_HALFWORD:
        if (size != BFFR_SIZE_HALFWORD) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    case BFFR_TYPE_BYTE:
        if (size != BFFR_SIZE_BYTE) {
            bffr_ecb(BFFR_EC_KEYWORD_MISMATCH);
            return;
        } else break;
    default:
        bffr_ecb(BFFR_EC_LOGICAL);
        return;
    }


    if (BFFR_KeywordExists(name)) {
        bffr_ecb(BFFR_EC_KEYWORD_OVERLOAD);
        return;
    }

    memcpy(&bffr_keywords[bffr_kwcount], name, length+1U);
    bffr_kwf[bffr_kwcount] = func;
    bffr_kwtypes[bffr_kwcount] = type;
    bffr_kwcount++;
}


void BFFR_ReadFile(const size_t filesize) {
    if (!bffr_cursorf || !bffr_readf || !filesize) { 
        bffr_ecb(BFFR_EC_LOGICAL);
        return;
    }

    BFFR_KeywordFallback bffr_scopes[BFFR_SCOPE_DEPTH] = { NULL };
    uint8_t bffr_dbuffer[BFFR_SIZE_DEFAULT] = { 0U };
    uint32_t bffr_depth     = 0U;
    size_t bffr_bytesread   = 0U;
    uint8_t bffr_byte       = 0;


    while (bffr_bytesread < filesize) {
        bffr_readf(&bffr_byte, BFFR_SIZE_BYTE);
        bffr_bytesread++;

        /* Terminate scope */
        if (bffr_byte == BFFR_TOKEN_TSCOPE) {
            if (bffr_depth > 0U) {
                bffr_depth--;
                if (bffr_scopes[bffr_depth]) bffr_scopes[bffr_depth](NULL, 0U);
            } else bffr_ecb(BFFR_EC_FLOGICAL);
            continue;
        } 
        
        /* Terminate All Scopes/File */
        if (bffr_byte == BFFR_TOKEN_TFILE) {
            if (!bffr_depth) {
                bffr_ecb(BFFR_EC_FLOGICAL);
                continue;
            }
            while (bffr_depth > 0U) {
                bffr_depth--;
                if (bffr_scopes[bffr_depth]) 
                    bffr_scopes[bffr_depth](NULL, 0U);
            }
            continue;
        }

        if (bffr_byte == BFFR_TOKEN_KWS) {
            BFFR_Type bffr_kwtype   = BFFR_TYPE_ERROR;
            uint32_t bffr_kwindx    = 0U;
            bool bffr_kwvalid       = false;

            /* read keyword */
            while (bffr_bytesread < filesize) {
                bffr_readf(&bffr_byte, BFFR_SIZE_BYTE);
                bffr_bytesread++;

                if (bffr_byte == BFFR_TOKEN_KWE) { 
                    bffr_kwvalid = bffr_kwindx != 0U;
                    break;
                }

                if (bffr_kwindx < BFFR_KEYWORD_SIZE)
                    bffr_kwbuffer[bffr_kwindx++] = (char)bffr_byte; 
                else break;
            }

            if (!bffr_kwvalid) {
                if (!bffr_kwindx) {
                    bffr_ecb(BFFR_EC_FLOGICAL);
                    continue;
                } else bffr_ecb(BFFR_EC_KEYWORD_TOO_LONG);
                break;
            }
            bffr_kwbuffer[bffr_kwindx] = '\0';
            uint32_t bffr_listindx = BFFR_FindKeyword();
            BFFR_KeywordFallback bffr_kwfunc = NULL;
            /* Need read at least 2 bytes after keyword end token */
            if (++bffr_bytesread >= filesize){ 
                bffr_bytesread--;
                break;
            }
            if (bffr_listindx < BFFR_KEYWORD_MAX) {
                bffr_kwfunc = bffr_kwf[bffr_listindx];
                bffr_kwtype = bffr_kwtypes[bffr_listindx];
                bffr_cursorf(1); /* skip o'er type */
            } else {
                bffr_readf(&bffr_byte, BFFR_SIZE_BYTE);
                bffr_kwtype = (BFFR_Type)bffr_byte;
            }

            switch (bffr_kwtype) {
            case BFFR_TYPE_SCOPED: 
                if (bffr_kwfunc && (bffr_depth < BFFR_SCOPE_DEPTH)) {
                    bffr_scopes[bffr_depth++] = bffr_kwfunc;
                    bffr_kwfunc(&bffr_byte, 1U);
                }
                break;
            case BFFR_TYPE_STRING:
                size_t bffr_strindx = 0U;
                while (bffr_bytesread < filesize) {
                    bffr_readf(&bffr_byte, BFFR_SIZE_BYTE);
                    bffr_bytesread++;
                    if (bffr_strindx < BFFR_STRING_SIZE-2U)
                        bffr_stringbuffer[bffr_strindx++] = (char)bffr_byte;
                    else {
                        // drain the rest of the string without storing it
                        while (bffr_bytesread < filesize) {
                            bffr_readf(&bffr_byte, BFFR_SIZE_BYTE);
                            bffr_bytesread++;
                            if (bffr_byte == BFFR_TOKEN_STRE) break;
                        }
                        break;
                    }
                    if (bffr_byte == BFFR_TOKEN_STRE) break;
                }
                
                if (bffr_kwfunc) bffr_kwfunc(bffr_stringbuffer, bffr_strindx);
                break;
            case BFFR_TYPE_SIZED: {
                uint8_t bffr_kwdsize = 0U;
                bffr_readf(&bffr_kwdsize, BFFR_SIZE_BYTE);
                bffr_bytesread++;
                
                if (filesize - bffr_bytesread < bffr_kwdsize) {
                    bffr_ecb(BFFR_EC_FLOGICAL);
                    return;
                }
                if (bffr_kwfunc) {
                    uint8_t* bffr_tbuffer = (uint8_t*)bffr_allocf(bffr_kwdsize);
                    if (!bffr_tbuffer) {
                        bffr_ecb(BFFR_EC_FBUFFER);
                        return;
                    }

                    bffr_readf(bffr_tbuffer, bffr_kwdsize);
                    bffr_kwfunc(bffr_tbuffer, bffr_kwdsize);
                }
                else bffr_cursorf(bffr_kwdsize);
                bffr_bytesread += bffr_kwdsize;

                break;
                }
            case BFFR_TYPE_ERROR:
                bffr_ecb(BFFR_EC_KEYWORD_ERRORED);
                return;
            default: {
                const size_t bffr_kwdsize = BFFR_GetSize(bffr_kwtype);
                if (filesize - bffr_bytesread < bffr_kwdsize) {
                    bffr_ecb(BFFR_EC_FLOGICAL);
                    return;
                }
                
                bffr_readf(bffr_dbuffer, bffr_kwdsize);
                bffr_bytesread += bffr_kwdsize;

                if (bffr_kwfunc) bffr_kwfunc(bffr_dbuffer, bffr_kwdsize);
                break;
                }
            }
        }
    }

    /* the file logically doesn't follow the convention, but for safety, we'll clean up */
    if (bffr_depth) {
        bffr_ecb(BFFR_EC_FLOGICAL);
        while (bffr_depth-- > 0U)
            if (bffr_scopes[bffr_depth]) 
                bffr_scopes[bffr_depth](NULL, 0U);
    }
}