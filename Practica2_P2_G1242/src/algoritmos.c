#include "../includes/algoritmos.h"

void toUpperOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        }
    }
    src[j] = '\0';
}

void toUpperAndNumbersOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        } else if (src[i] >= 48 && src[i] <= 57) {
        	src[j] = src[i];
            j++;
        }
    }
    src[j] = '\0';
}

int add_n_padding(const char* src, char* dst, int n_to_add) {

	if (!src || !dst)
		return -1;

	int i;
	int len = strlen(src);

	if (len < 0)
		return -1; 

	memmove(dst, src, (len + 1) * sizeof(char));

	for (i = len; i < (len + n_to_add); i++) {
		dst[i] = 'A';
	}

	dst[i] = '\0';
	return 0;
}

double average(int how_many, double* vals) {
	int i = 0;
	double sum = 0;
	
	for (i = 0; i < how_many; i++) {
		sum += vals[i];
	}
	return sum/how_many;
}

//much flatter random number distribution even on fast executions
int getRandomLessN(int n) {
	return random() % n;
}

int getRandomFromMAddN(int m, int n) {
	return random() % n + m;
}

void makePermutation(char* permutation, int n) {

	char* auxPerm;
	int i;
	int j = n;
	int randomValue;

	auxPerm = (char *) calloc ((n + 1), sizeof(char));
	if (!auxPerm) {
		permutation = NULL;
		return;
	}

	for (i = 0; i < n; i++) {
		auxPerm[i] = i + 1;
	}

	for (i = 0; i < n; i++) {
		randomValue = getRandomLessN(j);

		memcpy(&permutation[i], &auxPerm[randomValue], sizeof(char));

		fitArray(auxPerm, randomValue, n);

		j--;
	}

	free(auxPerm);
}

void fitArray(char* array, int posEle, int arraySize) {

	int i;
	int j = 0;

	for (i = (posEle + 1); i < arraySize; i++) {
		memcpy(&array[i - 1], &array[i], sizeof(char));
		j++;
	}

	return;
}

//disregarding all other characters
double* getAlphabetProbabilities(char* text, int len) {
	int i, total;
	double* l = (double*) calloc(ALPHA_SIZE, sizeof(double));
	if (l == NULL) {
		return NULL;
	}
	
	for (i = 0, total = 0; i < len; i++) {
		if (isalpha(text[i])) {
			l[toupper(text[i]) - 'A']++;
			total++;
		}
	}
	for (i = 0; i < ALPHA_SIZE; i++) {
		l[i] = l[i]/((double)total);
	}
	return l;
}

double** getIntersectionAlphabetProbabilities (char* plaintext, long len, char* ciphertext) {
	int i, j, total;
	
	double** ll = (double**) malloc(ALPHA_SIZE * sizeof(double*));
	
	if (ll == NULL) {
		return NULL;
	}
	for (i = 0; i < ALPHA_SIZE; i++) {
		ll[i] = (double*) calloc(ALPHA_SIZE, sizeof(double));
		if (ll[i] == NULL) {
			for (i--; i >= 0; i--) {
				free(ll[1]);
			}
			free(ll);
			return NULL;
		}
	}
	
	char x, y;
	
	for (i = 0, total = 0; i < len; i++) {
		x = toupper(plaintext[i]);
		y = toupper(ciphertext[i]);
		if (isalpha(x) && isalpha(y)) {
			ll[x - 'A'][y - 'A']++;
			total++;
		}
	}
	
	for(i = 0; i <  ALPHA_SIZE; i++) {
		for (j = 0; j < ALPHA_SIZE; j++) {
			ll[i][j] /= total;
		}
	}
	return ll;
}

uint64_t countSetBits(uint64_t byte) {
	int c;
	
	for (c = 0; byte; byte >>= 1) {
		c += byte & 0x01;
	}
	
	return c;
}

uint64_t invertNthbit (uint64_t word, uint8_t bit) {
	if (bit > 63)
		return word;
	uint64_t nbit = word & (1L << bit);
	nbit = (~nbit) & (1L << bit); 	//preserve the mask, but invert the bit
	word &= ~(1L << bit);			//preserve all other bits
	return word | nbit;
}

uint64_t nBitFilter (uint8_t len) {
	return (1 << len) - 1;
}

uint64_t rotateNBits (uint64_t word, uint8_t len, int8_t positions) {
	if (positions > 64 || positions < -64) {
		positions %= 65;
	}
	if (len > 64) {
		len = 64;
	}
	
	uint8_t i, b, abs = positions > 0 ? positions : -positions;
	
	for (i = 0; i < abs; i++) {
		//rotate clockwise
		if (positions > 0) {
			b = (word >> (len - 1)) & 1;
			word = (word << 1) | b;
		} else {	//rotate counterclockwise
			b = word & 1;
			word = (word >> 1) | (b << (len - 1));
		}
	}
	return word & nBitFilter(len);
}

uint8_t degreeOf(uint16_t x) {
	uint8_t i;
	for (i = 0; x > 1; i++, x >>= 1);
	return i;
}

//Euclid algorithm
uint16_t polyGDC(uint16_t a, uint16_t b) {
	uint16_t max = a > b ? a : b;
	uint16_t min = a < b ? a : b;
	
	uint16_t r = max ^ (min << (degreeOf(max) - degreeOf(min)));
	if (r == 0) {
		return min;
	}
	return polyGDC(min, r);
}

//all pointers are optional
void polyDiv(uint16_t n, uint16_t d, uint16_t* q, uint16_t* r) {
	if (q == NULL && r == NULL) {
		return;
	}
	int16_t i = 0;
	if (q != NULL) {
		*q = 0;
	}
	for (i = degreeOf(n) - degreeOf(d); i >= 0; i = degreeOf(n) - degreeOf(d)) {
		n ^= (d << i);			//substract divisor multiplied by quotient
		if (q != NULL) {
			*q |= (1 << i);		//build q as it would be in a division on paper
		}
	}
	if (r != NULL) {
		*r = n;
	}
}

uint8_t xtime (uint8_t x, uint8_t a, uint16_t m) {
	uint16_t r;
	if (x) {
		//multiplication by x would need mod m
		if (a & 0x80) {
			polyDiv(0x100, m, NULL, &r);
			a = (a << 1) ^ r;
		} else {
			a <<= 1;
		}
	}
	return a;
}

//intended for m of at most degree 8, it would otherwise overflow
uint8_t polyMul(uint8_t a, uint8_t b, uint16_t m) {
	if (a > b) {	//swap for efficiency, perform xtime on smallest integer
		a = a + b;
		b = a - b;
		a = a - b;
	}
	
	uint8_t results[8] = {b,0,0,0,0,0,0,0};	//b is base case, multiplication by 1
	uint8_t res = 0;
	int8_t i, j;
	
	for (i = 0; i < 8; i++) {
		if (a & (1 << i)) {
			if (results[i] == 0) {
				//find last calculated xtime
				for (j = i - 1; results[j] == 0; j--);
				for (; j < i; j++) {
					results[j + 1] = xtime(1, results[j], m);
				}
			}
			res ^= results[i];
		}
	}
	return res;
}

//Extended Euclid algorithm used to find multiplicative inverse
uint16_t polyMulInv(uint16_t a, uint16_t m) {
	uint16_t max, min;
	
	if (a > m) {
        max = a;
        min = m;
    } else {
        max = m;
        min = a;
    }
	
	if (min == 0 || polyGDC(a, m) != 1) {
        return 0;
    } else if (min == 1) {
		return 1;
	}
    uint16_t maxAux = max;
    
    uint16_t us[3] = {1, 0, 0};
    uint16_t vs[3] = {0, 1, 0};
    
    uint16_t r = 0;
    uint16_t q;

    while (r != 1) {
		
        // max = q*min + r
        polyDiv(max, min, &q, &r);

        // Un = Un-2 - q*Un-1
        us[2] = us[0] ^ polyMul(q, us[1], m);

        // Vn = Vn-2 - q*Vn-1
        vs[2] = vs[0] ^ polyMul(q, vs[1], m);

        // Reajustar valores
        max = min;
		min = r;
		memmove(us, us + 1, 2 * sizeof(uint16_t));
		memmove(vs, vs + 1, 2 * sizeof(uint16_t));
    }

	uint16_t inverse;
    if (vs[2] > maxAux) {
		inverse = maxAux ^ vs[2];
    } else {
        inverse = vs[2];
    }
    return inverse;
}

uint32_t wordPolyMul(uint32_t a, uint32_t b) {
	uint8_t d[4];
	uint8_t i, j;
	
	uint32_t am[4];
	
	//a1 a2 a3 a0 (little-endian)
	am[0] = (((a >> 8) & 0xFF) << 24)
			| (((a >> 16) & 0xFF) << 16)
			| ((a >> 24) << 8)
			| (a & 0xFF);
	for (i = 1; i < 4; i++) {
		am[i] = rotateNBits(am[0], 32, 8 * i);
	}
	
	for (i = 0; i < 4; i++) {
		d[i] = 0;
		for (j = 0; j < 64; j += 8) {
			// x & 0xFF implicit in conversion to uint8_t
			d[i] ^= polyMul(am[i] >> j, b >> j, MODULO_POLYNOMIAL);
		}
	}
	return (d[3] << 24) | (d[2] << 16) | (d[1] << 8) | d[0];
}