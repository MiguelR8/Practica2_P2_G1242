#include "../includes/algoritmos.h"
#include "../tables/AES_tables.c"

#define MIX_COLUMN_POLYNOMIAL 0x03010102L

uint8_t byteSub (uint8_t byte);
uint8_t invByteSub (uint8_t byte);
void shiftRows (uint8_t* state, uint8_t nb);		//testing needed
void invShiftRows (uint8_t* state, uint8_t nb);		//unimplemented
void invMixColumns (uint8_t* state, uint8_t nb);	//unimplemented

uint8_t* getRoundKeys(uint8_t* key, uint8_t nk, uint8_t nb, uint8_t nr);	//testing needed
