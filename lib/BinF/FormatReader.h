/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/
#ifndef X_BINF_FREADER_INCLUDED
#define X_BINF_FREADER_INCLUDED

/*
    The BinF Format/File Reader for ES is single-threaded and has a global state.

    For almost any dataset, in our original case a VFS Table, you can use the BFFR and ensure forward compatibility.
    Inherently, we cannot support write opperations as the current parser doesn't "broadcast" the positions of keywords, 
        thus the client need create its own writer that complies with the format.


    [Tha Formato]
    *   > keyword starts
    \   > keyword ends
    \n  > terminates a scope
    \0  > ifn't in string-scope, closes any remaining scopes
    
    formated: *mykeyword\
    after the keyword, the next byte signals its behaviour if it wasn't added to the keyword reg.

    Q: what is a scope?
    A: a scope is mostly a keyword-activated action, which doesn't read data. It gets called two times, one on-enter, 
        giving a non-null pointer (data doesn't matter), second passes NULL. For example, you may use a dictionary 
        scope, and know that any other "file" keywords after the first call will be within it, and after the second
        will be outside it.

    Q: what if the read keyword doesn't match its original size?
    A: In our implementation, it doesn't matter, as the program keeps track of know keyword's types, though, for 
        standard's sake, it should probably throw an error!
    Q: Do I own the data from the fallback?
    A: No you do not, unless you are using sized. By default, all data types (even strings) are stored within a shared 
        buffer. Only sized types have an allocated buffer, which you need to free yourself.
*/


#include "Dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BFFR_KEYWORD_MAX    32U
#define BFFR_KEYWORD_SIZE   12U
#define BFFR_SCOPE_DEPTH    12U
#define BFFR_STRING_SIZE    64U

/* Logical Error */
#define BFFR_EC_LOGICAL             000U
/* Keyword was too long */
#define BFFR_EC_KEYWORD_TOO_LONG    001U
/* Keyword was not recognised and was of type 'error' */
#define BFFR_EC_KEYWORD_ERRORED     002U
/* Keyword already exists */
#define BFFR_EC_KEYWORD_OVERLOAD    003U
/* Keyword type mismatch (need only on add) */
#define BFFR_EC_KEYWORD_MISMATCH    004U
/* File Logical Error */
#define BFFR_EC_FLOGICAL            100U
/* File Buffer Error  */
#define BFFR_EC_FBUFFER             101U

/* optional type information for unknown keywords */
typedef enum BFFR_Type {
    /* scope ends with newline byte */
    BFFR_TYPE_SCOPED    = ':',
    /* scope ends with zero byte    */
    BFFR_TYPE_STRING    = '%',
    /* next byte gives size         */
    BFFR_TYPE_SIZED     = '!',
    /* 256 bits                     */
    BFFR_TYPE_DEFAULT   = 'd',
    /* 128 bits                     */
    BFFR_TYPE_GREATER   = 'g',
    /* 64 bits                      */
    BFFR_TYPE_LONG      = 'l',
    /* 32 bits                      */
    BFFR_TYPE_WORD      = 'w',
    /* 16 bits                      */
    BFFR_TYPE_HALFWORD  = 'h',
    /* 8 bits                       */
    BFFR_TYPE_BYTE      = 'b',
    /* error if not recognised      */
    BFFR_TYPE_ERROR     = 'e'
} BFFR_Type;

typedef enum BFFR_Token {
    BFFR_TOKEN_KWS      = '*',
    BFFR_TOKEN_KWE      = '\\',
    BFFR_TOKEN_STRE     = '\0',
    BFFR_TOKEN_TSCOPE   = '\n',
    BFFR_TOKEN_TFILE    = '\0'
} BFFR_Token;

#define BFFR_SIZE_DEFAULT   32U
#define BFFR_SIZE_GREATER   16U
#define BFFR_SIZE_LONG      8U
#define BFFR_SIZE_WORD      4U
#define BFFR_SIZE_HALFWORD  2U
#define BFFR_SIZE_BYTE      1U

/* Read function, which auto-moves cursor */
typedef void (*BFFR_ReadFunc)(uint8_t*, size_t);
/* Set cursor */
typedef void (*BFFR_CursorFunc)(int);
/* function called on-kw-event */
typedef void (*BFFR_KeywordFallback)(void*,size_t);

/* if any logic breaks, it will call this function with a code (optional) */
void BFFR_SetErrorFallback(BinFCallback);

void BFFR_SetFileCursorer(BFFR_CursorFunc);
void BFFR_SetFileReader(BFFR_ReadFunc);
void BFFR_SetLargeBufferAlloc(BinFAlloc);
/* Size and type must not match in the case: type == "Error" || "Scoped" || "String" || "Sized" */
void BFFR_AddKeyword(const char*, uint32_t, BFFR_Type, BFFR_KeywordFallback);

/* pass the file's size! */
void BFFR_ReadFile(size_t);


#ifdef __cplusplus
}
#endif

#endif