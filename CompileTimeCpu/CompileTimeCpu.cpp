#include <array>

using uchar_t = unsigned char;

constexpr char REG_A = 0x00;
constexpr char REG_B = 0x01;
constexpr char REG_C = 0x02;

struct op
{
	constexpr op(char _op, char _o1, char _o2) : opCode(_op), o1(_o1), o2(_o2) {}
	constexpr op(char _op, char _o1) : opCode(_op), o1(_o1), o2(0x00) {}
	constexpr op(char _op) : opCode(_op), o1(0x00), o2(0x00) {}

	char opCode, o1, o2;
};

struct cpu
{
	struct memory
	{
		char registers[3];
		char ram[128];
		struct
		{
			char carry : 1;
			char hlt : 1;
		}flags;
	}memory;

	uchar_t pc = 0U;
};

template<long unsigned int S>
constexpr cpu run(const std::array<op, S>& ops, cpu c)
{
	constexpr std::array<void(*)(char, char, cpu*), 256> op_solvers
	{
		[](char, char, cpu* c) constexpr { c->memory.registers[REG_C] = c->memory.registers[REG_A] + c->memory.registers[REG_B]; }, // 0x00, ADD => REG_C = REG_A + REG_B
		[](char, char, cpu* c) constexpr { c->memory.registers[REG_C] = c->memory.registers[REG_A] + c->memory.registers[REG_B]; }, // 0x01, SUB => REG_C = REG_A - REG_B
		[](char o1, char, cpu* c) constexpr { c->pc = (uchar_t)o1; }, // 0x02, JMP OA => PC = OA
		[](char o1, char, cpu* c) constexpr { if (c->memory.registers[REG_C] != 0U) c->pc = (uchar_t)o1; }, // 0x03, JNZ OA => PC = OA if REG_C != 0
		[](char o1, char, cpu* c) constexpr { if (c->memory.registers[REG_C] == 0U) c->pc = (uchar_t)o1; }, // 0x04, JZ OA => PC = OA if REG_C == 0
		[](char, char, cpu* c) constexpr { c->memory.registers[REG_C] = (c->memory.registers[REG_A] == c->memory.registers[REG_B]) ? 0x00 : 0x01; }, // 0x05, CMP => REG_C = REG_A == REG_B = 0 : 1
		[](char o1, char o2, cpu* c) constexpr { c->memory.ram[o2] = c->memory.registers[o1]; }, // 0x06, STO OA OB => RAM[OB] = OA
		[](char o1, char o2, cpu* c) constexpr { c->memory.registers[o1] = o2; }, // 0x07, MOV OA OB => REG[OA] = OB
		[](char o1, char, cpu* c) constexpr { c->memory.registers[o1]++; }, // 0x08, INC OA OB => REG[OA]++
		[](char o1, char, cpu* c) constexpr { c->memory.registers[o1]--; }, // 0x09, DEC OA OB => REG[OA]--
		[](char o1, char o2, cpu* c) constexpr { c->memory.ram[c->memory.registers[o1]] = c->memory.registers[o2]; }, // 0x0A, STA OA OB => RAM[REG[OA]] = REG[OB]
	};

	while (c.pc < ops.size())
	{
		const auto o = ops[c.pc++];
		op_solvers[o.opCode](o.o1, o.o2, &c);
	}

	return c;
}

int main()
{
	constexpr std::array<op, 6U> code{
		op { 0x07, REG_A, 0x00 },   // MOV REG_A 0x00   ; Move 0 in to Register A
		op { 0x07, REG_B, 0x7F },   // MOV REG_B 0x7F   ; Move 127 in to Register B
		op { 0x0A, REG_A, REG_A },  // STA REG_A REG_A  ; Store contents of Register A at address stored in Register A
		op { 0x08, REG_A },         // INC REG_A        ; Increment contents of register A
		op { 0x05 },                // CMP              ; Compare Register A and Register B and store result in Register C
		op { 0x03, 0x02 },          // JNZ 0x02         ; Jump to 0x02 if Register C != 0U
	};

	constexpr auto result = run(code, cpu{});
	return result.memory.ram[0x33];
}



