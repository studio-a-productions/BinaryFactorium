# Binary Factorium

This project is a Factorio-inspired game for the Fri3d Badge 2024 (and maybe 2026). It is currently in-development.

It'll be free for all to use, learn, and install.


# Development & Maintenance

## StudA

### Profile

**Role**: *General Project Manager* (GPM) & *Backend*

### TODO LIST

| task | status |
| :--- | :----: |
| Create `Engine::Renderer` backend | Working |
| Add primitive `Game::World` logic | Working |

## WHY_youLOokingAT_NaMe

### Profile

**Role**: *Game Code* & *Design*

### TODO LIST

| task | status |
| :--- | :----: |
| Create basic chunk generation | NoS |
| Implement camera/player movement | NoS |


## meanings 

Status:
- Done: task has been completed and implemented
- Working: task is actively being worked on
- Pending: task will be developed later
- OoS: out of service; task will not be developed for now
- NoS: not a status; not assigned a status (yet)

# Code Rules

## Constant

When a variable is just data (for example a small config) and can just be the number itself, use `constexpr` over macros (`#define ...`) for type-and namespace safety.

If it is an array of data/assets that is dynamically read (and never written to), try to use `const` to ensure that it's either inlined at compile time or stored in `Flash Memory`.

## Objects

In general, avoid `class`es as they often add memory overhead. Only add objects when using them actually improves code quality, and you're not storing an array of them (prefer to use/create a special class for that instead or don't use objects at all).

The codebase also allows `structs`, but doesn't allow for **AoS** (*arrays of structs*), again, to prevent memory overhead.

## Namespace

When creating a file for *BinF*, it should either belong to `Game` or `Engine`, and thus use the namespace it is a part of. A file on its own shouldn't become its own namespace, unless you're using folders.

## Documentation

While documentation is important, we'll not concern ourselves with it constantly. In the event that something is unclear, you may open up a `GitHub Issue` using the "question" or "doc-req" tag. 


# Memory Overview

Mem/stor consumption overview

## const

| component | memory |
| :-------- | :----: |
| colour palette | 512 bytes |
| tile sprites | 16.4 kb |
| key metadata | 16 bytes |

## runtime

| component | memory |
| :-------- | :----: |
| chunk | 512 bytes |

