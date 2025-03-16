# Compartment
Compartment is a library that is meant to replace common use cases of the C++ standard library.
It provides common features like custom allocators, a string type, and generic containers.

## Why use it
The C++ standard library is bloated, weird, and has a lot of legacy cruft. This library aims to
be an alternative that is smaller, simpler, and easier to understand.

## Some goals and non-goals
Goals include:
+ **Simple** - all in one file, no build step required.
+ **Small** - includes only essential features.
+ **Fast** - does not aim to be *blazingly* fast, but to have competitive performance at the very least.
+ **Modern** - should have features like generic containers, unicode strings, custom allocators,
  file system access, basic networking, and subprocess management.
+ **Exposed** - all of the types are plain structures, with all fields public.
+ **Cross-platfrom** - support most popular platforms.

Non-goals include:
+ Being a 100% drop-in replacement for the C++ standard library.
+ Being a completely from scratch library.
+ Supporting every platform in existence.

## Features
- [x] Allocator interface
- [ ] UTF-8 strings
- [ ] General-purpose allocator
- [ ] Filesystem API
- [ ] Network API
- [ ] Subprocess API
- [x] Linux support
- [ ] Windows support
- [ ] MAC support
