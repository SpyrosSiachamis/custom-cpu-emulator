# RISC-V 16-bit CPU Simulator

A lightweight, educational CPU simulator built in C, inspired by the RISC-V architecture and instruction set. This project implements a custom 16-bit variant of the RISC design philosophy, featuring a reduced instruction set, multiple instruction formats, and a complete fetch-execute pipeline. This project is early in its implementation and currently does not run

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [Registers](#registers)
  - [Memory](#memory)
  - [Pipeline](#pipeline)
- [Instruction Set Architecture](#instruction-set-architecture)
  - [Instruction Formats](#instruction-formats)
  - [Opcode Table](#opcode-table)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Running](#running)
- [API Reference](#api-reference)
- [Inspiration](#inspiration)

---

## Overview

This simulator models a custom 16-bit CPU loosely modeled after the RISC-V ISA. All instructions are fixed-width at 16 bits. The design prioritizes simplicity and educational clarity: a small number of orthogonal instructions, a clean register file, and a straightforward two-phase fetch-execute loop.

Key characteristics:

- 16-bit address space and data width
- 64 KB of byte-addressable memory
- 16 general-purpose registers
- 16 unique opcodes across 6 instruction formats
- Two-phase control: fetch and execute
- Halt instruction via the SYS opcode

---

## Architecture

### Registers

The CPU exposes 16 general-purpose registers (`r0` through `r15`), each 16 bits wide. Conventions such as a zero register, return address register, or stack pointer register are left to the programmer or assembler layer.

| Name   | Width  | Description               |
| ------ | ------ | ------------------------- |
| PC     | 16 bit | Program counter           |
| r0-r15 | 16 bit | General-purpose registers |

### Memory

Memory is a flat, byte-addressable array of 65,536 bytes (64 KB). Instructions and data are read and written in 16-bit (2-byte) chunks. The full address space is accessible to all instructions.

```
Address Range   Usage
0x0000-0xFFFF   Unified instruction and data memory (64 KB)
```

### Pipeline

The simulator runs a simple two-phase loop controlled by the `is_execute_phase` flag:

1. **Fetch** - Read the 16-bit instruction located at the current value of the program counter. Advance PC by 2.
2. **Execute** - Decode the fetched instruction, identify its format and opcode, and carry out the corresponding operation on the register file or memory.

Execution halts when the `halted` flag is set, which is triggered by the `OP_SYS` instruction.

---

## Instruction Set Architecture

### Instruction Formats

All instructions are exactly 16 bits wide. The top 4 bits always encode the opcode. The remaining 12 bits are interpreted differently depending on the instruction format.

```
R-Type   [ opcode 4 | rd 4 | rs1 4 | rs2 4 ]
I-Type   [ opcode 4 | rd 4 | rs1 4 | imm 4 ]
S-Type   [ opcode 4 | rs1 4 | rs2 4 | imm 4 ]
B-Type   [ opcode 4 | rs1 4 | rs2 4 | imm 4 ]
J-Type   [ opcode 4 | rd 4 | imm 8        ]
U-Type   [ opcode 4 | rd 4 | imm 8        ]
SYS      [ opcode 4 | imm 12              ]
```

Immediate values are sign-extended where required (see `sign_extend` in `operations.h`).

### Opcode Table

| Opcode | Hex | Format | Mnemonic | Description                                       |
| ------ | --- | ------ | -------- | ------------------------------------------------- |
| 0      | 0x0 | R      | ADD      | rd = rs1 + rs2                                    |
| 1      | 0x1 | R      | SUB      | rd = rs1 - rs2                                    |
| 2      | 0x2 | R      | AND      | rd = rs1 & rs2                                    |
| 3      | 0x3 | R      | OR       | rd = rs1\| rs2                                    |
| 4      | 0x4 | R      | SLT      | rd = (rs1 < rs2) ? 1 : 0 (signed comparison)      |
| 5      | 0x5 | I      | ADDI     | rd = rs1 + sign_extend(imm, 4)                    |
| 6      | 0x6 | I      | LW       | rd = mem[rs1 + sign_extend(imm, 4)] (16-bit load) |
| 7      | 0x7 | I      | LB       | rd = mem[rs1 + sign_extend(imm, 4)] (8-bit load)  |
| 8      | 0x8 | I      | JALR     | rd = PC+2; PC = rs1 + sign_extend(imm, 4)         |
| 9      | 0x9 | S      | SW       | mem[rs1 + sign_extend(imm, 4)] = rs2 (16-bit)     |
| 10     | 0xA | S      | SB       | mem[rs1 + sign_extend(imm, 4)] = rs2 (8-bit)      |
| 11     | 0xB | B      | BEQ      | if (rs1 == rs2) PC += sign_extend(imm, 4)         |
| 12     | 0xC | B      | BNE      | if (rs1 != rs2) PC += sign_extend(imm, 4)         |
| 13     | 0xD | J      | JAL      | rd = PC+2; PC += sign_extend(imm, 8)              |
| 14     | 0xE | U      | LUI      | rd = sign_extend(imm, 8) << 8                     |
| 15     | 0xF | SYS    | SYS      | System call / halt (imm encodes the call number)  |

---

## Project Structure

```
.
├── cpu.h             # CPU struct, opcode enum, and main function prototypes
├── operations.h      # Helper function prototypes (sign extension, etc.)
├── cpu.c             # Core CPU logic: init, fetch, execute, debug print
├── operations.c      # Instruction implementations and helper functions
└── main.c            # Entry point and test programs
```

---

## API Reference

### `cpu.h`

```c
void     init_cpu(CPU16 *cpu);
uint16_t fetch_instruction(CPU16 *cpu, uint16_t address);
void     execute_instruction(CPU16 *cpu, uint16_t instruction);
void     print_cpu_state(CPU16 *cpu);
```

| Function                | Description                                                                   |
| ----------------------- | ----------------------------------------------------------------------------- |
| `init_cpu`            | Zeroes all registers, clears memory, and resets control signals               |
| `fetch_instruction`   | Reads a 16-bit instruction from the given address in memory                   |
| `execute_instruction` | Decodes and executes a single instruction, updating CPU state                 |
| `print_cpu_state`     | Dumps PC, all registers, and relevant memory contents to stdout for debugging |

### `operations.h`

```c
int16_t sign_extend(uint16_t value, int bits);
```

| Function        | Description                                                                              |
| --------------- | ---------------------------------------------------------------------------------------- |
| `sign_extend` | Sign-extends a `bits`-wide value stored in a `uint16_t` to a full signed `int16_t` |

---

## Inspiration

This project draws heavily from the [RISC-V ISA specification](https://riscv.org/technical/specifications/). The instruction formats (R, I, S, B, J, U) mirror the RISC-V convention but are compressed to 16 bits to fit a minimal address space. The goal is to capture the elegance of a reduced instruction set while remaining implementable and understandable in a few hundred lines of C.
