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

void invShiftRows (uint8_t* state, uint8_t nb) {
	uint16_t i, j, k;
	uint8_t last;
	
	for (i = 1; i < 4; i++) {
		for(j = 0; j < i; j++) {
			last = state[4 * (nb - 1) + i];
			//move all row bytes once
			for (k = nb - 1; k > 0; k--) {
				state[4 * k + i] = state[4 * (k - 1) + i];
			}
			//and put the last byte first
			state[i] = last;
		}
	}
}

void mixColumns(uint8_t* state, uint8_t nb) {
	uint8_t i, j;
	uint32_t col;
	for (i = 0; i < nb; i++) {
		col = wordPolyMul(MIX_COLUMN_POLYNOMIAL, bytesToWord(state[4 * i + 3],
				state[4 * i + 2],
				state[4 * i + 1],
				state[4 * i]));
		for (j = 0; j < 4; j++) {
			state[4 * i + j] = col >> (j*8);
		}
	}
}

void invMixColumns (uint8_t* state, uint8_t nb) {
	uint8_t i, j;
	uint32_t col;
	for (i = 0; i < nb; i++) {
		col = wordPolyMul(INV_MIX_COLUMN_POLYNOMIAL, bytesToWord(state[4 * i + 3],
				state[4 * i + 2],
				state[4 * i + 1],
				state[4 * i]));
		for (j = 0; j < 4; j++) {
			state[4 * i + j] = col >> (j*8);
		}
	}
}

void addRoundKey (uint8_t* state, uint32_t* key, uint8_t nb) {
	uint16_t i = 0;
	for (i = 0; i < nb; i++) {
		((uint32_t*)state)[i] ^= key[i];
	}
}

uint32_t* generate_AES_k(uint8_t nk, char* savefile) {
	uint8_t i;
	FILE* f = NULL;
	char word[23];
	
	uint8_t* k = malloc(4 * nk * sizeof(uint8_t));
	if (k != NULL) {
		if (savefile != NULL) {
			f = fopen(savefile, "w");
		}
		
		for (i = 0; i < 4 * nk; i++) {
			k[i] = random();
			if (f != NULL && i > 0 && i % 4 == 0) {
				sprintf(word, "%u ", bytesToWord(k[i-1], k[i-2], k[i-3], k[i-4]));
				fwrite(word, sizeof(char), strlen(word), f);
			}
		}
		//write final word
		if (f != NULL) {
			if (i > 0 && i % 4 == 0) {
				sprintf(word, "%u \n", bytesToWord(k[i-1], k[i-2], k[i-3], k[i-4]));
				fwrite(word, sizeof(char), strlen(word), f);
			}
			fclose(f);
		}
	}
	return (uint32_t*)k;
}

uint32_t* getRoundKeys(uint8_t* key, uint8_t nk, uint8_t nb, uint8_t nr) {
	uint32_t* roundKeys = (uint32_t*) malloc(nk * (nr + 1) * sizeof(uint32_t));
	if (roundKeys == NULL) {
		return NULL;
	}
	//save space keeping only most significant byte
	uint8_t* rcon = (uint8_t*) calloc(nb * (nr + 1), sizeof(uint8_t));	
	if (rcon == NULL) {
		free(roundKeys);
		return NULL;
	}
	uint32_t tmp;
	uint8_t i;
	
	//see fips 197, page 24
	for(i = 0; i < nk; i++) {
		roundKeys[i] = bytesToWord(key[(nk - i - 1) * nb + 3],
				key[(nk - i - 1) * nb + 2],
				key[(nk - i - 1) * nb + 1],
				key[(nk - i - 1) * nb + 0]);
	}
	
	//rcon starts at 2^-1
	for(rcon[0] = 0x8D, i = 1; i < nb*(nr+1); i++) {
		rcon[i] = polyMul(rcon[i-1], 0x02, MODULO_POLYNOMIAL);
	}
	
	for (i = nk; i < nb*(nr+1); i++) {
		tmp = roundKeys[i-1];
		if ((i % nk) == 0) {
			//RotWrod
			tmp = rotateNBits(tmp, 32, -8);
			//SubWord
			tmp = bytesToWord(byteSub(tmp >> 24),
					byteSub(tmp >> 16), byteSub( tmp >> 8),
					byteSub(tmp));
			//xor Rcon
			tmp ^= bytesToWord(0, 0, 0, rcon[i/nk]);
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

int cipher(uint8_t* in_state, uint8_t* out_state, uint8_t* key, uint8_t nk,
		uint8_t nb, uint8_t nr) {
	uint8_t i, j;
	memcpy(out_state, in_state, 4 * nb * sizeof(uint8_t));  
	uint8_t* state = out_state;
	uint32_t* rks = getRoundKeys(key, nk, nb, nr);
	if (rks == NULL) {
		return -1;
	}
	
	addRoundKey(out_state, rks, nb);
	
	for (i = 1; i < nr; i++) {
		for (j = 0; j < 4 * nb; j++) {
			state[j] = byteSub(state[j]);
		}
		shiftRows(state, nb); 
		mixColumns(state, nb);
		addRoundKey(out_state, (rks + i * nb), nb);
	}
	
	for (j = 0; j < 4 * nb; j++) {
		state[j] = byteSub(state[j]);
	}
	shiftRows(state, nb);
	addRoundKey(out_state, (rks + nr*nb), nb);
	
	free(rks);
	return 0;
}
int decipher(uint8_t* in_state, uint8_t* out_state, uint8_t* key, uint8_t nk,
		uint8_t nb, uint8_t nr) {
	uint8_t i, j;
	memcpy(out_state, in_state, 4 * nb * sizeof(uint8_t));  
	uint8_t* state = out_state;
	uint32_t* rks = getRoundKeys(key, nk, nb, nr);
	if (rks == NULL) {
		return -1;
	}
	
	addRoundKey(out_state, (rks + nr*nb), nb);
	
	for (i = nr-1; i > 0; i--) {
		invShiftRows(state, nb);
		for (j = 0; j < 4 * nb; j++) {
			state[j] = invByteSub(state[j]);
		}
		addRoundKey(out_state, (rks + i * nb), nb);
		invMixColumns(state, nb);
	}
	
	invShiftRows(state, nb);
	for (j = 0; j < 4 * nb; j++) {
		state[j] = invByteSub(state[j]);
	}
	
	addRoundKey(out_state, rks, nb);
	
	free(rks);
	return 0;
}
