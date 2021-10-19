# CHIP-8 Interpreter
This is a [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter, written in C++17, using SFML. Thanks to [Tobias Langhoff's guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

The interpreter accepts any CHIP-8 program (`.c8`), which can be obtained from the following archives:
- https://github.com/JohnEarnest/chip8Archive
- https://github.com/kripod/chip8-roms

## Current features

All CHIP-8 instructions are implemented. When instructions are ambiguous, the variant used in modern interpreters is used. 

The CHIP-8 specification specifies that the input device must be a 4x4 keypad with the following hexadecimal keys.

```
123C
456D
789E
A0BF
```

By convention, the following keyboard keys are used for the corresponding inputs.
```
1234
QWER
ASDF
ZXCV
```

### To-do (in no particular order)
- Proper GUI using [Dear ImGUI](https://github.com/ocornut/imgui)
- Configurable instruction sets
- Customizable display and sound
- Savestates
- Debugger and memory viewer
- Use CMake instead of a custom Makefile. Currently, the Makefile only supports building on Windows. 

## Dependencies
- SFML version 2.5.x. (https://github.com/SFML/SFML)
- Native File Dialog Extended (https://github.com/btzy/nativefiledialog-extended)

