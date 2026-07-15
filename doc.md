# BinF Documentation

# Engine

Documentation of functionality within the `BinF::Engine` namespace

## Memory

To abstract away memory allocation functions from code, we added `Malloc`, `Calloc`, and `Free` functions; mostly just abstraction over the standard C functions *malloc*, *calloc*, and *free*. These functions are "memory aware", which means they can allocate different kinds of memory. By default, it uses the standard memory.

The different `MemType`s (memory types) you can use are:

- `GFX`: graphics/dma capable RAM,
- `Internal`: internal RAM,
- `External`: external RAM like PSRAM,
- `Fast`: the fastest RAM (could be Internal, depends on platform config),
- `STD`: the standard/default option

## File System 

The engine includes its own "virtual" file system, which can manage files and data. However, there are some semantics you'll need to know.

### File-Naming

A file's name is also its path. Thus moving a file is the same as renaming it, which is why there's no *move* instruction. This is a unix-style part of the file system. 
**Importantly**: a file's path/name must always start with a "/". It is also prefered to place your files within the `binf` folder (BinF FileSystem).

Any file stored within this file system has the extension `.bff` (BinF File). If you add your own extension, for example we'll use `.dat`, you'll get `/file-name.dat.bff`, not *file-name.dat*. You must add these yourself.

### File-Handles

To access a file, it is recommended to use `FileHandle` objects, as they make code easier to follow. If you don't desire to use FileHandles, you may instead use FileIDs, which are indeces of an array of currently "handled" files. You must free the ID after use manually, or risk running out of handles.