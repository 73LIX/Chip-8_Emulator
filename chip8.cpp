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

    //function pointer lookup table (jump table)
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1NNN;
    table[0x2] = &Chip8::OP_2NNN;
    table[0x3] = &Chip8::OP_3XKK;
    table[0x4] = &Chip8::OP_4XKK;
    table[0x5] = &Chip8::OP_5XY0;
    table[0x6] = &Chip8::OP_6XKK;
    table[0x7] = &Chip8::OP_7XKK;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9XY0;
    table[0xA] = &Chip8::OP_ANNN;
    table[0xB] = &Chip8::OP_BNNN;
    table[0xC] = &Chip8::OP_CXKK;
    table[0xD] = &Chip8::OP_DXYN;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    //table - 0,8 and E
    for(size_t i = 0; i <= 0xE; i++){
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    //table0
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    //table8
    table8[0x0] = &Chip8::OP_8XY0;
    table8[0x1] = &Chip8::OP_8XY1;
    table8[0x2] = &Chip8::OP_8XY2;
    table8[0x3] = &Chip8::OP_8XY3;
    table8[0x4] = &Chip8::OP_8XY4;
    table8[0x5] = &Chip8::OP_8XY5;
    table8[0x6] = &Chip8::OP_8XY6;
    table8[0x7] = &Chip8::OP_8XY7;
    table8[0xE] = &Chip8::OP_8XYE;

    //tableE
    tableE[0x1] = &Chip8::OP_EXA1;
    tableE[0xE] = &Chip8::OP_EX9E;

    //tableF
    for(size_t i = 0; i <= 0x65; i++){
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_FX07;
    tableF[0x0A] = &Chip8::OP_FX0A;
    tableF[0x15] = &Chip8::OP_FX15;
    tableF[0x18] = &Chip8::OP_FX18;
    tableF[0x1E] = &Chip8::OP_FX1E;
    tableF[0x29] = &Chip8::OP_FX29;
    tableF[0x33] = &Chip8::OP_FX33;
    tableF[0x55] = &Chip8::OP_FX55; 
    tableF[0x65] = &Chip8::OP_FX65;
}

//router functions (calling the pointer)
void Chip8::Table0(){
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8(){
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE(){
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF(){
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL(){}

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

//Shift left by 1 ~ Vx or Vy
void Chip8::OP_8XYE(){
    
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    //Save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}

//Skip next instruction if Vx != Vy
void Chip8::OP_9XY0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] != registers[Vy]){
        pc += 2;
    }
}

//set I = NNN, I is the index here
//LD I, addr ~ load I (index) with addr
void Chip8::OP_ANNN(){
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}

//Jump to location NNN(addr) + V0
void Chip8::OP_BNNN(){

    uint8_t address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

//Set Vx = random byte &(bitwise AND operation) KK
void Chip8::OP_CXKK(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

//graphics rendering instruction
void Chip8::OP_DXYN(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //x and y starting coordinates for drawing on screen
    uint8_t height = opcode & 0x000Fu; //how many rows tall(1 to 15bytes) this sprite is

    //coordinate wrapping
    uint8_t xPos = registers[Vx] % VIDEO_HEIGHT;
    uint8_t yPos = registers[Vy] % VIDEO_WIDTH;

    registers[0xF] = 0; //initially collision 0

    //loops top to bottom for every row of the sprite(0 to height - 1)
    for(unsigned int row = 0; row < height; ++row){
        uint8_t spriteByte = memory[index + row]; //reads 1 byte of sprite pixel data from RAM starting at index

        for(unsigned int col = 0; col < 8; ++col){ //each byte contains 8 horizontal pixels
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            //calc targets array index
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if(spritePixel){
                if(*screenPixel == 0xFFFFFFFF){
                    registers[0xF] = 1;
                }
                //flips the screen pixel from on to off and off to on
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

//skip next instruction if a key with the value of Vx is pressed (registers[Vx] == keypad[key])
void Chip8::OP_EX9E(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if(keypad[key]){
        pc += 2;
    }
}

//skip next instruction if a key with the value of Vx is not pressed
void Chip8::OP_EXA1(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if(keypad[key]){
        pc += 2;
    }
}

//set Vx = delay timer value
void Chip8::OP_FX07(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

//Wait for a key press..., then store the value of the key in Vx
void Chip8::OP_FX0A(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if(keypad[0]){
        registers[Vx] = 0;
    }
    else if(keypad[1]){
        registers[Vx] = 1;
    }
    else if(keypad[2]){
        registers[Vx] = 2;
    }
    else if(keypad[3]){
        registers[Vx] = 3;
    }
    else if(keypad[4]){
        registers[Vx] = 4;
    }
    else if(keypad[5]){
        registers[Vx] = 5;
    }
    else if(keypad[6]){
        registers[Vx] = 6;
    }
    else if(keypad[7]){
        registers[Vx] = 7;
    }
    else if(keypad[8]){
        registers[Vx] = 8;
    }
    else if(keypad[9]){
        registers[Vx] = 9;
    }
    else if(keypad[10]){
        registers[Vx] = 10;
    }
    else if(keypad[11]){
        registers[Vx] = 11;
    }
    else if(keypad[12]){
        registers[Vx] = 12;
    }
    else if(keypad[13]){
        registers[Vx] = 13;
    }
    else if(keypad[14]){
        registers[Vx] = 14;
    }
    else if(keypad[15]){
        registers[Vx] = 15;
    } else {
        pc -= 2; //by doing this i cancel out the pc+=2 which is to go on the next instruction by the main loop
    }
}

//set delay timer = Vx
void Chip8::OP_FX15(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

//set sound timer = Vx
void Chip8::OP_FX18(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

//set I = I + Vx
void Chip8::OP_FX1E(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    index += registers[Vx];
}

//set index register I to point to the memory location of the font characters located at 0x50
void Chip8::OP_FX29(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    //digit got from registers[Vx] //any previous instruction stored a number into it. Our job is to set digit to that number.
    uint8_t digit = registers[Vx];

    //(we know)font characters are located at 0x50 and (we know) they're 5 bytes each, so by doing START+(5*digit) we try to find the starting byte index from where a particular character sprite begins in memory[]
    index = FONT_START_ADDRESS + (5 * digit);
    //ex: 0x50 + (5*0) = 0x50 contains character = 0, then 0x50 + (5*1) = 0x55 contains character = 1
}

//store BCD(Binary coded decimal) representation of Vx in mem locations I, I+1, I+2
void Chip8::OP_FX33(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    //ones place
    memory[index + 2] = value % 10; //extract the last digit
    value /= 10; //remove the last digit

    //tens place
    memory[index + 1] = value % 10;
    value /= 10;

    //hundreds place
    memory[index] = value % 10;
}

//store registers V0 through Vx in memory starting at location I(index register)
void Chip8::OP_FX55(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; i++){
        memory[index + i] = registers[i];
    }
}

//read registers V0 through Vx from memory starting at location I
void Chip8::OP_FX65(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; i++){
        registers[i] = memory[index + i];
    }
}