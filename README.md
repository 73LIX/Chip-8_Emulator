# CHIP-8 Emulator
TLDR: Chip-8 was never a physical system rather a virtual machine i.e. The CHIP-8 system exists only as a set of rules, memory layouts, and instructions.

This repo consists of an implementation of chip-8 emulator which i wrote to learn more about emulation.

<img width="800" height="450" alt="demo" src="https://github.com/user-attachments/assets/15f73f47-fb71-423a-a209-ee3b56a67977" />

### Specs:
- Memory: 4kb of RAM
- Registers: 16 registers (V0 to VF)
- Display: 64x32px
- Opcodes: 35 (Instructions)

### Installation:
1. Clone the repo
```
git clone https://github.com/73LIX/Chip-8_Emulator
```
2. Build using Cmake
```
cd Chip-8_Emulator
cmake -S . -B build
cmake --build build
```

### Usage:
```
./chip8 <scale> <delay> <ROM>
#example:
./chip8 10 3 Tetris.ch8 
```

### Parameters:
- Scale: Controls the factor of scaling for the Chip-8 display window, since originally it's only 64x32 which we have to scale for our modern monitors.
- Delay: Controls the clock speed. The Chip-8 had no specified clock speed, so we'll use a delay to determine the time in milliseconds between cycles.
- ROM: The ROM file to load.

### Keyboard mapping:
```
CHIP-8 Keypad         Keyboard

1 2 3 C               1 2 3 4
4 5 6 D               Q W E R
7 8 9 E               A S D F
A 0 B F               Z X C V
```

### Documentation:
These are the documentations which i followed through out my process of learning and building the emulator:
- [Mattmikolay's Github Repo](https://github.com/mattmikolay/chip-8)
- [Thomas P. Greene's Documentation](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Austim Morlan's Article](https://austinmorlan.com/posts/chip8_emulator/#how-does-a-cpu-work)

### Games:
You can get games(ROM Files) to try on from [dmatlack's repo](https://github.com/dmatlack/chip8/tree/master/roms/games)
