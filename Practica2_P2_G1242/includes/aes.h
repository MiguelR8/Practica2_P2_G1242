/** Implementation for the AES 128 cipher*/

#include "../includes/algoritmos.h"
#include "../tables/AES_tables.c"

#define MIX_COLUMN_POLYNOMIAL 0x03010102
#define INV_MIX_COLUMN_POLYNOMIAL 0x0B0D090E
#define NK 4
#define NB 4
#define NR 10

uint8_t byteSub (uint8_t byte);
uint8_t invByteSub (uint8_t byte);
//operates on a matrix of nb columns and 4 rows (as per standard)
void shiftRows (uint8_t* state, uint8_t nb);
void mixColumns (uint8_t* state, uint8_t nb);
void addRoundKey (uint8_t* state, uint32_t* key, uint8_t nb);
void invShiftRows (uint8_t* state, uint8_t nb);		//unimplemented
void invMixColumns (uint8_t* state, uint8_t nb);	//unimplemented

uint32_t* generate_AES_k(uint8_t nk, char* savefile);
uint32_t* getRoundKeys(uint8_t* key, uint8_t nk, uint8_t nb, uint8_t nr);

int cipher(uint8_t* in_state, uint8_t* out_state, uint8_t* key, uint8_t nk,
		uint8_t nb, uint8_t nr);
int decipher(uint8_t* in_state, uint8_t* out_state, uint8_t* key, uint8_t nk,
		uint8_t nb, uint8_t nr);											//uimplemented
