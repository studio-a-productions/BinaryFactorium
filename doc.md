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

A file's name is also its path. Thus moving a file is the same as renaming it, which is why there's no *move* instruction. This is a unix-style part of the file system.
**Importantly**: a file's path/name must always start with a "/". It is also prefered to place your files within the `binf` folder (BinF FileSystem).

**Any file** stored within this file system has the extension `.bff` (BinF File). If you add your own extension, for example we'll use `.dat`, you'll get `/file-name.dat.bff`, not *file-name.dat*. You must add these yourself.

### File-Access

To access a file, you must always first (*attempt to*) open it. The BFSF (BinF FileSystem) has a lot of restrictions on opening files.
First and foremost, any file that is not within the BFSF table shall **not** be accessed, this means that you're not able to change "unregistered" files.
Secondly, filenames must end with .bff, so that any and all files owned by the BinF application can be tracked and easily found. (if this is for security and if it matters, is left to how interpretation)
Lastly, BinF only allows a max "open files" of **16**, these are tracked via the ID, thus it is up to the client to "free" these accordingly. (Destroy<>() or FreeID())

### File-Handles

To access a file, it is recommended to use `FileHandle` objects, as they make code easier to follow. If you don't desire to use FileHandles, you may instead use FileIDs, which are indeces of an array of currently "handled" files. You must free the ID after use manually, or risk running out of handles.
