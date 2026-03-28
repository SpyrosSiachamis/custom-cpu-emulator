#ifndef OPERATIONS_H
# define OPERATIONS_H
#include <stdint.h>
/*  This header file contains the operations the CPU will do based on the instruction.
    It includes CPU operations, helper functions, and any other necessary functions for the CPU to execute instructions.
*/
// Helper function prototypes
int16_t sign_extend(uint16_t value, int bits);
#endif // OPERATIONS_H