#include "../tables/AES_tables.c"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 64

typedef enum {CIFRAR, DESCIFRAR} modo;

uint8_t areCoprime(uint8_t a, uint8_t b) {
	uint8_t max, min;
	if (a < 2 || b < 2) {
		return 0xFF;
	}
	if (a > b) {
		max = a;
		min = b;
	} else {
		max = b;
		min = a;
	}
	uint8_t mod = max % min;
	if (mod > 1) {
		return areCoprime(min, mod);
	}
	return mod;
}

uint8_t countSetBits(uint8_t byte) {
	int c;
	
	for (c = 0; byte; byte >>= 1) {
		c += byte & 0x01;
	}
	
	return c;
}

uint8_t SBOX_hash (uint8_t byte, modo m) {
	uint8_t row = (byte >> 2) & 0x03;
	uint8_t column = byte & 0x03;

	if (m == CIFRAR) {
		return DIRECT_SBOX[row][column];
	}
	return INVERSE_SBOX[row][column];
}

void showConsecutiveDifferences(uint8_t max_n, modo m) {
	
	uint8_t i, last;
	uint8_t ic[ROWS_PER_SBOX + 1];
	double mean, variance;
	
	uint8_t counts[0xFF];
	
	for (i = 1, last = SBOX_hash(0, m); 1; last = SBOX_hash(i++, m)) {
		counts[i - 1] = countSetBits(last ^ SBOX_hash(i, m));
		if (i == max_n) {
			break;
		}
	}
	//generate statistics (mean and variance of count and difference between succesives)

	memset(ic, 0, ROWS_PER_SBOX + 1);
	for (i = 0, mean = 0 ; i < max_n - 1; i++) {
		mean += counts[i];
		ic[counts[i]]++;
	}
	mean /= max_n - 1.0;
	
	printf("\tRecuento de diferencias usando n y n+1: ");
	for(i = 0; i < ROWS_PER_SBOX + 1; i++) {
		if (ic[i] != 0) {
			printf("%d (%d) ", i, ic[i]);
		}
	}
	putchar('\n');
	
	printf("\tMedia de diferencias usando n y n+1: %lf (%.2lf%%)\n",
			mean, mean / ROWS_PER_SBOX * 100);
	
	for (i = 0, variance = 0; i < max_n - 1; i++) {
		variance += (mean - counts[i]) * (mean - counts[i]);
	}
	
	variance /= max_n - 1.0;
	
	printf("\tVarianza de diferencias usando n y n+1: %.3lf\n", variance);
	
	for (i = 1; i < max_n -1; i++) {
		//absolute value of the difference
		if (counts[i - 1] > counts[i]) {
			counts[i - 1] -= counts[i];
		} else {
			counts[i - 1] = counts[i] - counts[i - 1];
		}
	}
	
	for (i = 0, mean = 0; i < max_n - 2; i++) {
		mean += counts[i];
	}
	mean /= max_n - 2.0;
	
	printf("\tMedia de diferencias entre diferencias consecutivas: %.3lf\n",
			mean);
	
	for (i = 0, variance = 0; i < max_n - 2; i++) {
		variance += (mean - counts[i]) * (mean - counts[i]);
	}
	variance /= max_n - 2.0;
	
	printf("\tVarianza de diferencias entre diferencias consecutivas: %.3lf\n",
			variance);
}

void showLinearDependences(uint8_t max_n, modo m) {
	uint8_t i, j;
	uint32_t count;
		
	for (count = 0, i = max_n; i > 0; i--) {
		for (j = i - 1; j > 0; j--) {
			if (areCoprime(i, j) == 0) {
				if (areCoprime(SBOX_hash(i, m), SBOX_hash(j, m)) == 0) {
					count++;
				}
			}
		}
	}
	
	//all possible pairings = (n-1)*(n+2)/2
	double p = (2.0 * count * 100) / (0xFF * 0x102);
	
	printf("Se encontraron %hi (%lf%%) parejas de valores con dependencias lineales\n",
				count, p);
}

int main (int argc, char* argv[]) {
	int c;
	modo m = -1;
	uint32_t n = 0, d = 0;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"n", required_argument, 0, 'n'},
		   {"d", required_argument, 0, 'd'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "CDn:d:",
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
			case 'n':
				n = atoi(optarg);
				if (n > 0xFF || n < 1) {
					printf("n debe ser menor que 256 y mayor que 1\n");
					return EXIT_FAILURE;
				}
				break;
			case 'd':
				d = atoi(optarg);
				if (d > 0xFF || d < 2) {
					printf("d debe ser menor que 256 y mayor que 2\n");
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C | -D} [-n num_pruebas] [-d num_pruebas]\n", argv[0]);
				puts("n para probar cambios en entradas consecutivas");
				puts("d para probar la independencia lineal");
				return EXIT_FAILURE;
		}
	}
	
	if ((n == 0 && d == 0) || m == -1) {
		printf("Uso: %s {-C | -D} [-n num_pruebas] [-d num_pruebas]\n", argv[0]);
		puts("n para probar cambios en entradas consecutivas");
		puts("d para probar independencia lineal");
		return EXIT_FAILURE;
	}
	
	if (n != 0)
		showConsecutiveDifferences(n, m);
	if (d > 1)
		showLinearDependences(d, m);
	return EXIT_SUCCESS;;
}
