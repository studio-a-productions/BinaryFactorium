# Binary Factorium

This project is a Factorio-inspired game for the Fri3d Badge 2024 (and maybe 2026). It is currently in-development.

It'll be free for all to *use*, *learn*, and *install*, under the [licence](LICENCE) of this project.

## The Name

Binary Factorium uses the "shortname" `BinF` *(pronounced 'binf')* for almost all its writing. If this name were to be already in use, then know Binary Factorium is not trying to impersonate whatever uses it.

# Development & Maintenance

## StudA

**Role**: *General Project Manager/Maintainer* (GPM) & *Backend*


## WHY_youLOokingAT_NaMe

**Role**: *Game Code* & *Design*


# Memory Overview

Mem/stor consumption overview (this will be later moved to docs)

## const (FLASH)

| component | memory |
| :-------- | :----: |
| colour palette | 512 bytes |
| tile sprites | 16.4 kb |
| key metadata | 16 bytes |

## runtime (SRAM)

Do to our `BinF::Engine Renderer` using double buffering, more than **half** our fast memory budget is spent (to comply with DMA capable RAM).

In short, **284 kilobytes** of our 512 kilobyte budget is used by literally just this system. 

Why not use a single buffer? Well, screen tear is a big problem. If you want to know why, just search up why people use double buffering.

### BinF::Engine (remaining)
| component | memory | info |
| :-------- | :----: | :--- |
| KeyTask | ~ 8.2 kb | The space allocated for the InputTask task/thread, which does debouncing on key presses async from game/engine logic |
| Key data | 204 bytes | Not counting for atomics, this is around the amount of memory the arrays take up |



### World & Chunk

| component | memory | info |
| :-------- | :----: | :--- |
| Local ChunkData | 2304 bytes | Soring local tiles in one big array (currently 3x3 chunks), not accounting for any nodes. |



