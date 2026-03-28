# Opcode Reference

This document describes the 16-bit instruction encodings and the `Opcode` enum used by the emulator (see cpu.h).

- Instruction word: 16 bits
- Bit numbering: 0 = least-significant bit (LSB), 15 = most-significant bit (MSB)
- Registers: 16 registers numbered 0..15 (register 0 is treated as hardwired zero in the CPU implementation)

## Instruction formats

Common notation used below: `[hi:lo]` denotes bit positions (inclusive) where `hi` > `lo`.

### R-Type (register-register)

Layout (MSB -> LSB):

```
[15:12] rs2 | [11:8] rs1 | [7:4] rd | [3:0] opcode
```

Decode example:

```c
uint16_t inst = ...; // 16-bit instruction
uint8_t opcode = inst & 0xF;
uint8_t rd     = (inst >> 4) & 0xF;
uint8_t rs1    = (inst >> 8) & 0xF;
uint8_t rs2    = (inst >> 12) & 0xF;
```

### I-Type (register + immediate)

```
[15:12] imm4 | [11:8] rs1 | [7:4] rd | [3:0] opcode
```

Decode example:

```c
uint8_t imm4 = (inst >> 12) & 0xF; // sign-extend if instruction semantics require
uint8_t rs1  = (inst >> 8) & 0xF;
uint8_t rd   = (inst >> 4) & 0xF;
```

### S-Type (store)

```
[15:12] imm4 | [11:8] rs2 | [7:4] rs1 | [3:0] opcode
```

Decode example:

```c
uint8_t imm4 = (inst >> 12) & 0xF;
uint8_t rs2  = (inst >> 8) & 0xF; // value to store
uint8_t rs1  = (inst >> 4) & 0xF; // base pointer
```

### B-Type (branch)

```
[15:12] imm4 | [11:8] rs2 | [7:4] rs1 | [3:0] opcode
```

`imm4` is the branch offset (usually sign-extended); exact scaling (bytes vs instructions) is implementation-defined.

### J-Type / U-Type (jump / upper immediate)

```
[15:8] imm8 | [7:4] rd | [3:0] opcode
```

Decode example:

```c
uint8_t imm8 = (inst >> 8) & 0xFF;
uint8_t rd   = (inst >> 4) & 0xF;
```

### SYS (system / immediate-wide)

```
[15:4] imm12 | [3:0] opcode
```

Decode example:

```c
uint16_t imm12 = (inst >> 4) & 0x0FFF;
```

### Sign-extension helpers (examples)

```c
static inline int16_t sign_extend4(uint16_t v) {
    int16_t s = v & 0xF;
    if (s & 0x8) s |= 0xFFF0;
    return s;
}
static inline int16_t sign_extend8(uint16_t v) {
    int16_t s = v & 0xFF;
    if (s & 0x80) s |= 0xFF00;
    return s;
}
```

## Opcode table

The table below maps the `Opcode` enum from `cpu.h` to its hex value, instruction class, and a short description.

| Opcode     | Hex  | Type   | Description |
|------------|:----:|:------:|-------------|
| `OP_ADD`   | `0x0`| R-Type | Add: `rd = rs1 + rs2` |
| `OP_SUB`   | `0x1`| R-Type | Subtract: `rd = rs1 - rs2` |
| `OP_AND`   | `0x2`| R-Type | Bitwise AND: `rd = rs1 & rs2` |
| `OP_OR`    | `0x3`| R-Type | Bitwise OR: `rd = rs1 | rs2` |
| `OP_SLT`   | `0x4`| R-Type | Set less-than (signed): `rd = (rs1 < rs2) ? 1 : 0` |
| `OP_ADDI`  | `0x5`| I-Type | Add immediate: `rd = rs1 + imm4` (imm4 sign-extended when used as signed)
| `OP_LW`    | `0x6`| I-Type | Load word (16-bit) into `rd` from `rs1 + imm4` offset |
| `OP_LB`    | `0x7`| I-Type | Load byte into `rd` from `rs1 + imm4` (usually sign-extended)
| `OP_JALR`  | `0x8`| I-Type | Jump-and-link register: save return address in `rd`; `PC = rs1 + imm4` |
| `OP_SW`    | `0x9`| S-Type | Store word (16-bit) from `rs2` to memory at `rs1 + imm4` |
| `OP_SB`    | `0xA`| S-Type | Store byte from low byte of `rs2` to memory at `rs1 + imm4` |
| `OP_BEQ`   | `0xB`| B-Type | Branch if equal: `if (rs1 == rs2) PC += sign_extend(imm4)` |
| `OP_BNE`   | `0xC`| B-Type | Branch if not equal: `if (rs1 != rs2) PC += sign_extend(imm4)` |
| `OP_JAL`   | `0xD`| J-Type | Jump and link: save return address in `rd`; `PC += sign_extend(imm8)` |
| `OP_LUI`   | `0xE`| U-Type | Load upper immediate into `rd` (place immediate into the high part of `rd`) |
| `OP_SYS`   | `0xF`| System | System call / privileged operation; `imm12` typically encodes system operation/code |

Notes:

- The descriptions above use conventional semantics (e.g., `rd`, `rs1`, `rs2`, sign-extension for immediates). The emulator's implementation in `cpu.c` / `cpu.h` determines exact behavior (for example: how immediates are scaled or shifted for J/U types).
- Register `0` is typically treated as constant 0 by the CPU implementation — writing to it has no effect.

## Examples

- Encoding example (R-Type): `ADD x3, x1, x2`

  rd = 3, rs1 = 1, rs2 = 2, opcode = 0x0

  Instruction value = (rs2 << 12) | (rs1 << 8) | (rd << 4) | opcode

  Calculation: (2 << 12) | (1 << 8) | (3 << 4) | 0x0 = 0x2130

## See also

- `cpu.h` — source of truth for the `Opcode` enum and comment describing formats.

---

Generated for the custom-cpu-emulator repository.
