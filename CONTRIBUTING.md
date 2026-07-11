# Code Rules

## Constant

When a variable is just data (for example a small config) and can just be the number itself, use `constexpr` over macros (`#define ...`) for type-and namespace safety.

If it is an array of data/assets that is dynamically read (and never written to), try to use `const` to ensure that it's either inlined at compile time or stored in `Flash Memory`.

## References

Using references is allowed (of course), but do know that it practically is still a pointer. In some situations, it may also just be preferable to use raw pointers.

## Objects

In general, avoid virtual `class`es as they often add memory overhead. Only add `virtual` when using them actually improves code/design quality.
The codebase also allows `structs`, but doesn't allow for **AoS** in cases where you need to loop over a single element of that object (*arrays of structs*), again, to prevent memory overhead.

## Namespace

When creating a file for *BinF*, it should either belong to `Game` or `Engine`, and thus use the namespace it is a part of. A file on its own shouldn't become its own namespace, unless you're using folders.

## Documentation

While documentation is important, we'll not concern ourselves with it constantly. In the event that something is unclear, you may open up a `GitHub Issue` using the "question" or "doc-req" tag. 
