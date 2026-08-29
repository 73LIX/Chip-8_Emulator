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

//jumpt to address 0xNNN
void Chip8::OP_1NNN(){
    uint16_t address = opcode & 0x0FFFu; // doing the bitwise and operation of opcode with a bitwise mask(0x0FFFu) to remove the 1 i.e. the command(opcode) and get the actual extracted address
    pc = address;
}

//call subroutine at address 0xNNN
void Chip8::OP_2NNN(){
    uint16_t address = opcode & 0x0FFFu; //extracting address ~ removing the 2 here

    stack[sp] = pc;
    ++sp;
    pc = address;
}

//skip next instruction if Vx[x is the index here] = KK[byte]
void Chip8::OP_3XKK(){
    //extract Vx [register index x] 
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //shift by 8 bits to the right
    //extract KK [byte]
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] == byte){
        pc = pc + 2; //every single instruction is exactly 2 bytes in chip 8, so pc += 2 and our main normal cpu execution will also do pc +=2 making a total of +4 jumps so the next instruction will be skipped.
    }
}

//skip next instruction if Vx != KK(similar to OP_3XKK)
void Chip8::OP_4XKK(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] != byte){
        pc = pc + 2;
    }
}

//skip next instruction if Vx = Vy
void Chip8::OP_5XY0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] == registers[Vy]){
        pc = pc + 2;
    }
}

//set Vx = KK
void Chip8::OP_6XKK(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

//add Vx with KK and store it in Vx(register)
void Chip8::OP_7XKK(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

//set Vx = Vy
void Chip8::OP_8XY0(){
    
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

//set Vx = Vx OR Vy
void Chip8::OP_8XY1(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy]; //bitwise OR(|) operation
}

//set Vx = Vx AND Vy
void Chip8::OP_8XY2(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy]; //bitwise AND(&) operation
}

//set Vx = Vx XOR Vy
void Chip8::OP_8XY3(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy]; //bitwise XOR(^) operation
}

//set Vx = Vx + Vy, set VF(flag register) = carry
void Chip8::OP_8XY4(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if(sum > 255U){
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0x00FFu; //masking of to get the 8-bit data(since registers are 8-bit only)
}

//sets Vx = Vx - Vy, set VF(flag register) = Borrow or not borrow
void Chip8::OP_8XY5(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] > registers[Vy]){
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

//shift right by 1
void Chip8::OP_8XY6(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    //save LSB in VF
    registers[0xF] = (registers[Vx] & 0x01u); //masking with 0x01 to extract the lsb and store it (0 or 1)

    registers[Vx] >>= 1; //shift right by 1
}

//Check Vy > Vx if true VF -> 1 else VF -> 0 and then perform Vx = Vy - Vx
void Chip8::OP_8XY7(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx]){
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}