# Tier
![Tier Logo](assets/Logo.png)
\
Code in *Unity*.

Tier is a statically typed language inspired by C++ and Rust,
built around a **Tier**ed type system and a unique configuration
system using `#set` and `#enforce` directives.

## What does Tier aim to do?
- Clear and accurate diagnostics
- Focused on providing clarity and ease in flexibility.
- Tiered approach allows gradual learning, and flexibility.
- Enforce different coding standards for *all* code-bases.

## Tier is in it's early stages - WIP

## How to build:
- Go to the root.
- Make a `build` folder.
- Enter the `build` folder.
- Write in the terminal `cmake ..`.
- Wait for it to finish then write `make`.
- You will have a `tier` executable!

## How to test:
- Go to `tests` folder
- Write in terminal: `(python interpreter, ex: python3) load_tests.py (compiler path, ex: ../build/tier)`