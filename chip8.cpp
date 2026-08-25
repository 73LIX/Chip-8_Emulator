#include <fstream>
#include <chrono>
#include <random>
#include <cstring>
#include "chip8.hpp"

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_SIZE = 85;
const unsigned int FONT_START_ADDRESS = 0x50;

uint8_t fontset[FONT_SIZE] = 
{
    0xF0, 0X90, 0X90, 0X90, 0XF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80, //F
    0xF0, 0x80, 0xB0, 0x90, 0xF0  //G
};

void Chip8::LoadROM(char const* filename){
    // open the file as a stream of binary and move the file pointer at the end(ate)
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        //get the size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg(); //get the size of file
        char* buffer = new char[size]; //allocate a buffer to hold the contents

        //go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg); //go back to the beginning
        file.read(buffer, size); //fill the buffer
        file.close(); //close the file to avoid memory leaks

        //load the rom contents into the chip8's memory, starting from 0x200
        for(long i = 0; i < size; i++){
            memory[START_ADDRESS + i] == buffer[i];
        }

        //free the buffer
        delete[] buffer;
    }
    
}

//constructor
Chip8::Chip8():randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    //initialize program_countr
    pc = START_ADDRESS;

    for(unsigned int i = 0; i < FONT_SIZE; ++i){
        memory[FONT_START_ADDRESS + i] = fontset[i];
    }

    //initialize rng
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

//clear screen
void Chip8::OP_00E0(){
    //setting the entire video buffer to zero
    memset(video, 0, sizeof(video));
}

//return
void Chip8::OP_00EE(){
    --sp;
    pc = stack[sp];
}