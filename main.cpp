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

//controls the entire lifecycle of the emulator by continuously polling input, executing CPU cycles at a controlled speed and rendering the screen
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

    //calcs how many bytes are in a single horizontal row of your display buffer
    //Why SDL needs this? ~ to avoid skewing or warping the graphics when copying them to the screen
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = chrono::high_resolution_clock::now();
    bool quit = false;

    //emulation loop ~ call Cycle() continuously until quit = true
    while(!quit){
        quit = platform.ProcessInput(chip8.keypad);
        auto currentTime = chrono::high_resolution_clock::now();
        //calc delta time(dt) -- the exact time passed in ms since the last cycle
        float dt = chrono::duration<float, chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if(dt > cycleDelay){
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }
    return 0;
}