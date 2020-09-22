#include "ctcpu.h"

int main()
{
	constexpr std::array<op, 6U> code{
		op { 0x07, REG_A, 0x00 },   // MOV REG_A 0x00   ; Move 0 in to Register A
		op { 0x07, REG_B, 0x7F },   // MOV REG_B 0x7F   ; Move 127 in to Register B
		op { 0x0A, REG_A, REG_A },  // STA REG_A REG_A  ; Store contents of Register A at address stored in Register A
		op { 0x08, REG_A },         // INC REG_A        ; Increment contents of register A
		op { 0x05 },                // CMP              ; Compare Register A and Register B and store result in Register C
		op { 0x03, 0x02 }           // JNZ 0x02         ; Jump to 0x02 if Register C != 0U
	};
	
    constexpr auto c = run(code, cpu{});
    return c.memory.ram[0x33];
}