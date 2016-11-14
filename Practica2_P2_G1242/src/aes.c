#include "../includes/aes.h"

uint8_t byteSub (uint8_t byte) {
	uint8_t row = (byte >> 4) & 0x0F;
	uint8_t column = byte & 0x0F;
	
	return DIRECT_SBOX[row][column];
}

uint8_t invByteSub (uint8_t byte) {
	uint8_t row = (byte >> 4) & 0x0F;
	uint8_t column = byte & 0x0F;
	
	return INVERSE_SBOX[row][column];
}

void shiftRows(uint8_t* state, uint8_t nb) {
	uint16_t i, j, k;
	uint8_t first;
	
	for (i = 1; i < 4; i++) {
		for(j = 0; j < i; j++) {
			first = state[i];
			//move all row bytes once
			for (k = 0; k < nb - 1; k++) {
				state[4 * k + i] = state[4 * (k + 1) + i];
			}
			//and put the first byte last
			state[4 * (nb - 1) + i] = first;
		}
	}
}

void mixColumns(uint8_t* state, uint8_t nb) {
	uint8_t i, j;
	uint32_t col;
	for (i = 0; i < nb; i++) {
		col = wordPolyMul(MIX_COLUMN_POLYNOMIAL, (state[4 * i + 3] << 24)
				| (state[4 * i + 2] << 16)
				| (state[4 * i + 1] << 8)
				| state[4 * i]);
		for (j = 0; j < 4; j++) {
			state[4 * i + j] = col >> (j*8);
		}
	}
}