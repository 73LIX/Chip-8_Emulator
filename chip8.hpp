#pragma once

#include <cstdint>

const unsigned int REGISTER_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int STACK_LEVEL = 16;
const unsigned int KEY_COUNT = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;


class Chip8
{
public:

    Chip8();
    void LoadROM(char const* filename);

	uint8_t registers[REGISTER_COUNT]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[STACK_LEVEL]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[KEY_COUNT]{};
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
	uint16_t opcode{};

private:

    //clear screen
    void OP_00E0();

    //return (from a subroutine)
    void OP_00EE();

	//jumpt to a location (0xnnn).
	void OP_1NNN();

	//call instruction
	void OP_2NNN();

	//skip next instruction if Vx = kk
	void OP_3XKK();

	//skip the next instruction if Vx != KK
	void OP_4XKK();

	//skip the next instruction if Vx = Vy
	void OP_5XY0();

	//set Vx = KK
	void OP_6XKK();

	//add Vx = Vx + KK
	void OP_7XKK();

	//set Vx = Vy
	void OP_8XY0();

	//set Vx = Vx or Vy
	void OP_8XY1();

	//set Vx = Vx AND Vy
	void OP_8XY2();

	//set Vx = Vx XOR Vy
	void OP_8XY3();

	//set Vx = Vx + Vy, set VF = carry
	void OP_8XY4();

	//sets Vx = Vx - Vy, set VF = Borrow or not borrow
	void OP_8XY5();

	//shift right by 1 ~ VX 
	void OP_8XY6();

	//Check Vy > Vx if true VF -> 1 else VF -> 0 and then perform Vx = Vy - Vx
	void OP_8XY7();

	//Shift left by 1 ~ Vx or Vy
	void OP_8XYE();

	//SNE Vx, Vy
	void OP_9XY0();

	//ANNN - LD I, addr
	void OP_ANNN();

	//BNNN - JP V0, addr
	void OP_BNNN();

	//RND Vx, byte
	void OP_CXKK();

	//DRW Vx, Vy, nibble
	void OP_DXYN();

	//SKP Vx
	void OP_EX9E();

	//SKNP Vx
	void OP_EXA1();

	//LD Vx, DT
	void OP_FX07();

	//LD Vx, K
	void OP_FX0A();

	//LD DT, Vx
	void OP_FX15();

	//LD ST, Vx
	void OP_FX18();

	//ADD I, Vx
	void OP_FX1E();

	//LD F, Vx
	void OP_FX29();

	//LD B, Vx
	void OP_FX33();

	//LD [I], Vx
	void OP_FX55();

	//LD Vx, [I]
	void OP_FX65();

    std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	typedef void (Chip8::*Chip8Func)();
	Chip8Func table[0xF + 1];
	Chip8Func table0[0XE + 1];
	Chip8Func table8[0XE + 1];
	Chip8Func tableE[0XE + 1];
	Chip8Func tableF[0X65 + 1];
};