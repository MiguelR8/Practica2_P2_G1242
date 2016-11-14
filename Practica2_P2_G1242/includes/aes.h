#include "../includes/algoritmos.h"
#include "../tables/AES_tables.c"

#define MIX_COLUMN_POLYNOMIAL 0x03010102L

uint8_t byteSub (uint8_t byte);
uint8_t invByteSub (uint8_t byte);
void shiftRows(uint8_t* state, uint8_t nb);