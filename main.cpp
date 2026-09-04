//call Cycle() continuously until exit, handle input & render with sdl
//three command-line args:
//1. Scale: The chip-8 video buffer is only 64x32, so we'll need a scale factor to be able to play on our higher resolution monitors.
//2. Delay: the chip-8 had no specified clock speed, so we'll use a delay to determine the time in milliseconds between cycles. Different games run best at different speeds, so we can control it here.
//3. ROM: The ROM file to load.

#include "chip8.hpp"
#include "platform.hpp"
#include <chrono>
#include <iostream>
using namespace std;

int main(int argc, char** argv){
    if(argc != 4){
        cerr << "Usage: " << argv[0] << "<Scale> <Delay> <Rom>\n";
        exit(EXIT_FAILURE);
    }

    int videoScale = stoi(argv[1]);
    int cycleDelay = stoi(argv[2]);
    char const* romFilename = argv[3];

    //application window
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

    //load rom
    Chip8 chip8;
    chip8.LoadROM(romFilename);
}