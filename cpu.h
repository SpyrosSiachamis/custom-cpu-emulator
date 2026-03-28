#ifndef CPU_H
# define CPU_H
# include <stdint.h>
# include <stdbool.h>
#define MEMORY_SIZE 65536 // 64KB of memory for the 16bit CPU

typedef struct {
    uint16_t PC; // Program counter, used to keep track of the current instruction address, (PC - Program Counter)
    uint16_t regs[16]; // 16 general-purpose registers, each 16 bits wide, (RF - Register File)
    Control control; // Control signals for the current instruction, used to determine the operation to perform and how to manipulate the CPU state.
    // Memory is represented as an array of bytes, but instructions and data are accessed in 16-bit chunks.
    uint8_t memory[MEMORY_SIZE]; // 64KB of memory, byte-addressable, (MF - Memory File)

    // Control signals
    bool is_execute_phase;  // false = fetch, true = execute
    bool halted;            // To stop the loop
} CPU16;

typedef struct {
    uint8_t opcode; // [0:3] Opcode (4 bits)
    uint16_t instruction; // The full 16-bit instruction, used for decoding the operands and immediate values.
} Control;

/*  Instruction Opcodes 
    opcodes are 4 bits, allowing for 16 unique instructions. The instruction formats are as follow:
    R -Type: [0:3] Opcode (4 bits) | [4:7] rd (4 bits) | [8:11] rs1 (4 bits) | [12:15] rs2 (4 bits)
    I-Type: [0:3] Opcode (4 bits) | [4:7] rd (4 bits) | [8:11] rs1 (4 bits) | [12:15] imm (4 bits)
    S-Type: [0:3] Opcode (4 bits) | [4:7] rs1 (4 bits) | [8:11] rs2 (4 bits) | [12:15] imm (4 bits)
    B-Type: [0:3] Opcode (4 bits) | [4:7] rs1 (4 bits) | [8:11] rs2 (4 bits) | [12:15] imm (4 bits)
    J-Type: [0:3] Opcode (4 bits) | [4:7] rd (4 bits) | [8:15] imm (8 bits)
    U-Type: [0:3] Opcode (4 bits) | [4:7] rd (4 bits) | [8:15] imm (8 bits)
    SYS: [0:3] Opcode (4 bits) | [4:15] imm (12 bits)
*/
typedef enum {
    // R-Type Instructions
    OP_ADD  = 0x0,
    OP_SUB  = 0x1,
    OP_AND  = 0x2,
    OP_OR   = 0x3,
    OP_SLT  = 0x4,

    // I-Type Instructions
    OP_ADDI = 0x5,
    OP_LW   = 0x6,
    OP_LB   = 0x7,
    OP_JALR = 0x8,

    // S-Type Instructions
    OP_SW   = 0x9,
    OP_SB   = 0xA,

    // B-Type Instructions
    OP_BEQ  = 0xB,
    OP_BNE  = 0xC,

    // J-Type Instructions
    OP_JAL  = 0xD,
    OP_LUI  = 0xE,

    // System
    OP_SYS  = 0xF
} Opcode;

// Main Function prototypes
void init_cpu(CPU16 *cpu);

// Fetches the instruction at the given address from memory and returns it as a 16-bit value.
uint16_t fetch_instruction(CPU16 *cpu, uint16_t address);

// Decodes the given instruction and executes it, modifying the CPU state accordingly.
void execute_instruction(CPU16 *cpu, uint16_t instruction);

// Prints the current state of the CPU, including the program counter, register values, and memory contents. Developed for debugging purposes.
void print_cpu_state(CPU16 *cpu);
#endif