#include "../tables/AES_tables.c"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 64

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
	for (i = degreeOf(n) - degreeOf(d); n > d; i = degreeOf(n) - degreeOf(d)) {
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
    
    int16_t us[3] = {0, 1, 0};
    int16_t vs[3] = {1, 0, 0};
    
    uint16_t r = 0;
    uint16_t q;

    while (r != 1) {
		
        // max = q*min + r
        polyDiv(max, min, &q, &r);
        printf("%hX = %hX * %hX + %hX\n", max, q, min, r);

        // Un = Un-2 - q*Un-1
        us[2] = us[0] - q * us[1];
        printf("%hX = %hX - %hX * %hX\n", us[2], us[0], q, us[1]);

        // Vn = Vn-2 - q*Vn-1
        vs[2] = vs[0] - q * vs[1];
        printf("%hX = %hX - %hX * %hX\n", vs[2], vs[0], q, vs[1]);

        // Reajustar valores
        max = min;
		min = r;
		memmove(us, us + 1, 2 * sizeof(uint16_t));
		memmove(vs, vs + 1, 2 * sizeof(uint16_t));
    }

	uint16_t inverse;
    if (vs[2] < 0) {
		inverse = maxAux + vs[2];
    } else {
        inverse = vs[2];
    }
    printf("Inverse is %hX\n", inverse);
    return inverse;
}

uint8_t byte_cipher(uint8_t byte, uint16_t m) {
	//uint8_t masks[8] = {0x8F, 0xC7, 0xE3, 0xF1, 0xF8, 0x7C, 0x3E, 0x1F};
	uint8_t masks[8] = {0xF8, 0x7C, 0x3E, 0x1F, 0x8F, 0xC7, 0xE3, 0xF1};
	uint8_t res = 0;
	uint8_t i;
	//in case of 0 most operations will have no effect
	if (byte != 0) {
		byte = polyMulInv(byte, m); //important, get multiplicative inverse first		 
		for (i = 0; i < 8; i++) {
			//apply mask, then add bits modulo 2 (same counting and getting LSB)
			//then place in the respective ith position
			res |= (countSetBits(byte & masks[i]) & 0x1) << i;
		}
	}
	res ^= 0x63;
	return res;
}

uint8_t byte_decipher(uint8_t byte, uint16_t m) {
	uint8_t masks[8] = {0x25, 0x92, 0x49, 0xA4, 0x52, 0x29, 0x94, 0x4A};
	uint8_t res = 0;
	uint8_t i;
	
	if (byte != 0) {
		for (i = 0; i < 8; i++) {	//multiply by mask matrix
			res |= (countSetBits(byte & masks[i]) & 0x1) << i;
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

int main (int argc, char* argv[]) {
	int c;
	uint32_t n = 0, d = 0;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"n", required_argument, 0, 'n'},
		   {"d", required_argument, 0, 'd'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "n:d:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'n':
				n = atoi(optarg);
				if (n > 0xFF || n < 1) {
					printf("n debe ser menor que 255 y mayor que 1\n");
					return EXIT_FAILURE;
				}
				break;
			case 'd':
				d = atoi(optarg);
				if (d > 0xFF || d < 2) {
					printf("d debe ser menor que 255 y mayor que 2\n");
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s [-n num_pruebas] [-d num_pruebas]\n", argv[0]);
				puts("n para probar cambios en entradas consecutivas");
				puts("d para probar la independencia lineal");
				return EXIT_FAILURE;
		}
	}
	
	//~ if (n == 0 && d == 0) {
		//~ printf("Uso: %s [-n num_pruebas] [-d num_pruebas]\n", argv[0]);
		//~ puts("n para probar cambios en entradas consecutivas");
		//~ puts("d para probar independencia lineal");
		//~ return EXIT_FAILURE;
	//~ }
	
	uint16_t a = 0x5F;
	//uint16_t b = 0x38;
	uint16_t m = 0x11B;
	//printf("mcd(%hX, %hX) = %hX\n", a, b, polyGDC(a, b));
	//printf("%hX^-1 %% %hX = %hX\n", a, m, polyMulInv(a, m));
	//printf("%hX^-1 %% %hX = %hX\n", b, m, polyMulInv(b, m));
	
	//printf("%hX * %hX %% %hX = %hhX\n", a, b, m, polyMul(a, b, m));
	for (a = 1; a < 0x100; a++) {
		if (polyMul(a, polyMulInv(a, m), m) != 1) {
			printf("%hhX * %hhX != 1\n", a, polyMulInv(a, m));
		}
	}
	return EXIT_SUCCESS;
}
