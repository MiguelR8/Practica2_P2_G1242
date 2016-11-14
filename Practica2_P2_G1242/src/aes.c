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

uint8_t* getRoundKeys(uint8_t* key, uint8_t nk, uint8_t nb, uint8_t nr) {
	uint32_t* roundKeys = (uint32_t*) malloc(nk*(nr+1)*sizeof(uint32_t));
	if (roundKeys == NULL) {
		return NULL;
	}
	//save space keeping obly most significant byte
	uint8_t* rcon = (uint8_t*) calloc((nb*(nr+1)) * sizeof(uint8_t));	
	if (rcon == NULL) {
		free(roundKeys);
		return NULL;
	}
	uint32_t tmp;
	uint8_t i;
	
	//rcon starts at 1 or 2?
	for(rcon[0] = 0x01, i = 1; i < nb*(nr+1); i++) {
		rcon[i] = polyMul(rcon[i-1], 0x02, MODULO_POLYNOMIAL);
	}
	
	//see fips 197, page 24
	for(i = 0; i < nk; i++) {
		roundKeys[i] = bytesToWord(key[4*i + 3], key[4*i + 2], key[4*i + 1],
				key[4*i]);
	}
	
	for (i = nk; i < nb*(nr+1); i++) {
		tmp = roundKeys[i-1];
		if ((i % nk) == 0) {
			tmp = rotateNBits(tmp, 32, -8);
			tmp = bytesToWord(byteSub(tmp >> 24),
					byteSub(tmp >> 16), byteSub( tmp >> 8),
					byteSub(tmp));
			tmp ^= rcon[i/nk] << 24;
		} else if (nk > 6 && (i%nk) == 4) {
			tmp = bytesToWord(byteSub(tmp >> 24),
					byteSub(tmp >> 16), byteSub( tmp >> 8),
					byteSub(tmp));
		}
		roundKeys[i] = roundKeys[i - nk] ^ tmp;
	}
	
	free(rcon);
	return roundKeys;
}
