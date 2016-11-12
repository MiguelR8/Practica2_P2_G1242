#include "../tables/AES_tables.c"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 64
#define MODULO_POLYNOMIAL 0x11B

enum modo {CIFRAR, DESCIFRAR};

uint64_t countSetBits(uint64_t byte) {
	int c;
	
	for (c = 0; byte; byte >>= 1) {
		c += byte & 0x01;
	}
	
	return c;
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

uint8_t byte_cipher(uint8_t byte, uint16_t m) {
	uint8_t bitOffsets[5] = {4, 5, 6, 7, 8};
	uint8_t res = 0, buf;
	uint8_t i, j;
	//in case of 0 most operations will have no effect
	if (byte != 0) {
		byte = polyMulInv(byte, m); //important, get multiplicative inverse first		 
		for (i = 0; i < 8; i++) {
			//b'i = b(i + 4 % 8) + b(i + 5 % 8) + b(i + 6 % 8) + b(i + 7 % 8) + b(i + 8 % 8)
			for (j = 0; j < 5; j++) {
				buf = byte & (1 << ((i + bitOffsets[j]) % 8));
				res ^= (buf ? 1:0) << i;
			}
		}
	}
	//b'i += 0x63
	res ^= 0x63;
	return res;
}

uint8_t byte_decipher(uint8_t byte, uint16_t m) {
	uint8_t bitOffsets[3] = {2, 5, 7};
	uint8_t res = 0, buf;
	uint8_t i, j;
	
	if (byte != 0) {
		for (i = 0; i < 8; i++) {
			//b'i = b(i + 2 % 8) + b(i + 5 % 8) + b(i + 7 % 8)
			for (j = 0; j < 3; j++) {
				buf = byte & (1 << ((i + bitOffsets[j]) % 8));
				res ^= (buf ? 1:0) << i;
			}
		}
	}
	res ^= 0x05;				//add integer
	if (res != 0)				//and get multiplicative inverse
		res = polyMulInv(res, m);	
	return res;
}

uint8_t SBOX_hash (uint8_t byte) {
	uint8_t row = (byte >> 4) & 0x0F;
	uint8_t column = byte & 0x0F;
	
	return DIRECT_SBOX[row][column];
}

uint8_t SBOX_unhash (uint8_t byte) {
	uint8_t row = (byte >> 4) & 0x0F;
	uint8_t column = byte & 0x0F;
	
	return INVERSE_SBOX[row][column];
}

int main (int argc, char* argv[]) {
	int c;
	uint8_t i, j;
	uint8_t m = 0xFF;
	FILE* fout = NULL;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'D'},
		   {"D", no_argument, 0, 'C'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "CDo:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'C':
				m = CIFRAR;
				break;
			case 'D':
				m = DESCIFRAR;
				break;
			case 'o':
				fout = fopen(optarg, "w");
				if (fout == NULL) {
					perror("Al abrir el archivo para escribir");
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C | -D} [-o fileout]\n", argv[0]);
				return EXIT_FAILURE;
		}
	}
	
	if (m == 0xFF) {
		printf("Uso: %s {-C | -D} [-o fileout]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (fout == NULL) {
		fout = stdout;
	}
	fputc('\t', fout);
	for (i = 0; i < 0x10; i++) {
		fprintf(fout, "%hhX\t", i);
	}
	fputc('\n', fout);
	
	for (i = 0; i < 0x10; i++) {
		fprintf(fout, "%hhX\t", i);
		for (j = 0; j < 0x10; j++) {
			if (m == CIFRAR) {
				fprintf(fout, "%02hhx\t", byte_cipher((i << 4) | j,
						MODULO_POLYNOMIAL));
			} else {
				fprintf(fout, "%02hhx\t", byte_decipher((i << 4) | j,
						MODULO_POLYNOMIAL));
			}
		}
		fputc('\n', fout);
	}
	
	fclose(fout);
	return EXIT_SUCCESS;
}
