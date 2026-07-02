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