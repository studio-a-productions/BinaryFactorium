# BinF Documentation

# Engine

Documentation of functionality within the `BinF::Engine` namespace

## Memory

To abstract away memory allocation functions from code, we added `Malloc`, `Calloc`, `Free` (and more) functions; which can be thought of as type and memory-aware variants of C's *malloc*, *calloc*, and *free*. These functions are "memory aware", which means they can allocate different kinds of memory. By default, it uses the standard memory.

The different `MemType`s (memory types) you can use are:

- `GFX`: graphics/dma capable RAM,
- `Internal`: internal RAM,
- `External`: external RAM like PSRAM,
- `Fast`: the fastest RAM (could be Internal, depends on platform config),
- `STD`: the standard/default option

## File System

The engine includes its own "virtual" file system, which can manage files and data. However, there are some semantics you'll need to know.

### File-Naming

A file's name is also its path. Thus moving a file is the same as renaming it, which is why there's no *move* instruction. This is a unix-style part of the file system. (... may be implemented idk)

**Importantly**: a file's path/name must always start with a "/". It is also prefered to place your files within the `binf/` folder. Note that SD access needs `/sd` first.

### File-Access

To access a file, you must always first (*attempt to*) open it. The BFSF (BinF FileSystem) has some restrictions on files.

BinF only allows a max "open files" of **16**, these are tracked via the ID, thus it is up to the client to "free" these accordingly. (Destroy<>() or FreeID())

### File-Handles

To access a file, it is recommended to use `FileHandle` objects, as they make code easier to follow. These objects abstract away the manual overhead of file ids.

If you don't desire to use FileHandles, you may instead use FileIDs, which are indeces of an array of currently "handled" files. You must free the ID after use manually, or risk running out of handles.
