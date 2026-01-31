# CHIP-8 Emulator

A CHIP-8 interpreter written in C with SDL2 for graphics rendering.

## About CHIP-8

CHIP-8 is an interpreted programming language developed in the mid-1970s for early microcomputers. It's commonly used as a first emulator project due to its simple instruction set and minimal hardware requirements.

## Features

- Full CHIP-8 instruction set implementation
- 64x32 pixel monochrome display
- SDL2-based graphics rendering
- ROM loading from file

## Requirements

- macOS (tested on M2)
- SDL2 library
- GCC compiler

## Installation

Install SDL2 via Homebrew:
```bash
brew install sdl2
```

## Building

```bash
gcc main.c -o chip8 -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2
```

## Usage

```bash
./chip8 <path_to_rom>
```

Example:
```bash
./chip8 Pong\ \(1\ player\).ch8
```

## Controls

Keyboard controls map to the CHIP-8 hexadecimal keypad (0-F). 4x4 Keypad mappped to keyboard.

CHIP-8:
\n[1][2][3][C]
\n[4][5][6][D]
\n[7][8][9][E]
\n[A][0][B][F]

QWERTY Keyboard:
[1][2][3][4]
[Q][W][E][R]
[A][S][D][F]
[Z][X][C][V]


Press ESC or close the window to quit.

## Technical Details

- **Memory**: 4096 bytes
- **Display**: 64x32 pixels
- **Registers**: 16 8-bit registers (V0-VF)
- **Stack**: 16 levels
- **Timers**: Delay and sound timers at 60Hz
- **Program Counter**: Starts at 0x200

## Project Structure

```
chip8.h     - CHIP-8 emulator core and instruction set
main.c      - SDL2 wrapper and main loop
```

## Known Issues

Performance is not currently optimized.

## License

Free to use and modify.
