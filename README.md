# CHIP-8 Emulator

A simple CHIP-8 interpreter/emulator implemented in C++ using SDL2 for graphics, input, and sound. 

## Building

### Prerequisites

- C++ compiler with C++20 support
- CMake (version 4.0 or higher)
- SDL2 development libraries

### Build Instructions

```bash
mkdir -p build
cd build
cmake ..
make
```

The executable will be created as `build/app`.

## Running Programs

To run a CHIP-8 program, provide the path to a `.ch8` file:

```bash
./build/app programs/ibm-Logo.ch8
./build/app programs/test_opcode.ch8
./build/app programs/br8kout.ch8
```

### Controls

The CHIP-8 keypad is mapped to the following keyboard keys:

```
CHIP-8 Keypad     Keyboard
----------        --------
1 2 3 C           1 2 3 4
4 5 6 D           Q W E R
7 8 9 E           A S D F
A 0 B F           Z X C V
```

- Press **ESC** to exit the emulator

