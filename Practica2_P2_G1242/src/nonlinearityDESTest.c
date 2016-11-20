#include "../tables/DES_tables.c"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 64

uint16_t max (uint16_t a, uint16_t b) {
	if (a > b) {
		return a;
	}
	return b;
}

uint16_t min (uint16_t a, uint16_t b) {
	if (a < b) {
		return a;
	}
	return b;
}

uint8_t areCoprime(uint8_t a, uint8_t b) {
	if (a < 2 || b < 2) {
		return 0xFF;
	}
	uint8_t mod = max(a,b) % min(a,b);
	if (mod > 1) {
		return areCoprime(min(a, b), mod);
	}
	return mod;
}

uint8_t countSetBits(uint8_t byte) {
	int c;
	
	for (c = 0; byte; byte = byte >> 1) {
		c += byte & 0x01;
	}
	
	return c;
}

uint32_t getMinorMatrix(uint8_t dimension, uint8_t row, uint64_t data) {
	uint64_t mask = (1 << (dimension * (dimension - 1))) - 1;
	uint32_t res = 0;
	data &= mask;	//take entire matrix, except first row
	uint64_t i, j;
	for (i = row, j = 1; i < data; i += dimension) {
		if (i == 0) {
			res |= data & ((1L << dimension) - 1L);
		} else {
			mask = ((1L << (dimension - 1)) - 1L) << dimension;
			res |= (data & mask) >> j;
			j++;
		}
	}
	return res;
}

//up to dimension 6

int8_t bitMatrixDeterminant(uint8_t dimension, uint64_t data) {
	uint64_t i;
	int8_t res, j;
	
	uint64_t mask = (1L << (dimension * dimension)) - 1L;
	data &= mask;	//take entire matrix, but no more
	
	if (dimension == 2) {
		return (data >> 3) * (data & 0x0001)
				- ((data & 0x0004) >> 2) * ((data & 0x0002) >> 1);
	} else if (dimension < 2) {
		return data & dimension;
	}
	
	i = data >> (dimension * (dimension-1));
	for (res = 0, j = 0; i != 0; i >>= 1L, j++) {
		if (i & 0x0001) {
			res += (j % 2 ? -1L:1L) * bitMatrixDeterminant(dimension -1,
					(uint16_t)getMinorMatrix(dimension, j, data));
		}
	}
	return res;
}
uint8_t SBOX_hash (uint8_t byte, int box_n) {
	if (byte > 0x3F)
		return 0xFF;
	
	//0x20 = 10_0000
	uint8_t row = ((byte >> 4) & 0x02) + (byte & 0x01);
	uint8_t column = ((byte >> 1) & 0x0F);
	
	return S_BOXES[box_n][row][column];
}

uint32_t SBOX_hash_complete(uint64_t word) {
	int i;
	uint32_t res, buff;
	for (i = 0, res = 0; i < NUM_S_BOXES; i++, word >>= 6) {
		buff = SBOX_hash(word & 0x3F, i) & 0x0000000F;
		res |= buff << (4 * i);
	}
	return res;
}

void showIndividualConsecutiveDifferences(uint8_t max_n) {
	
	if (max_n > 0x3F) {
		max_n = 0x3F;
	}
	uint8_t i, j, last;
	uint8_t ic[5];
	double mean, variance;
	
	uint8_t counts[0x3F];
	
	double total_mean = 0;
	double total_variance = 0;
	
	for (j = 0; j < NUM_S_BOXES; j++) {
		last = SBOX_hash(0, j);
		
		for (i = 1; i <= max_n; last = SBOX_hash(i++, j)) {
			counts[i - 1] = countSetBits(last ^ SBOX_hash(i, j));
		}
		//generate statistics (mean and variance of count and difference between succesives)

		memset(ic, 0, 5);
		for (i = 0, mean = 0 ; i < max_n - 1; i++) {
			mean += counts[i];
			ic[counts[i]]++;
		}
		mean /= max_n - 1.0;
		
		printf("Para la caja S %d:\n", j + 1);
		printf("\tRecuento de diferencias usando n y n+1: ");
		for(i = 0; i < 5; i++) {
			if (ic[i] != 0) {
				printf("%d (%d) ", i, ic[i]);
			}
		}
		putchar('\n');
		
		printf("\tMedia de diferencias usando n y n+1: %lf (%.2lf%%)\n",
				mean, mean/4*100);
		total_mean += mean;
		
		for (i = 0, variance = 0; i < max_n - 1; i++) {
			variance += (mean - counts[i]) * (mean - counts[i]);
		}
		
		variance /= max_n - 1.0;
		
		printf("\tVarianza de diferencias usando n y n+1: %.3lf\n", variance);
		total_variance += variance;
		
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
	
	printf("Totales: media (%lf), varianza (%lf)\n",
			total_mean / NUM_S_BOXES,
			total_variance / NUM_S_BOXES);
}

void showTotalConsecutiveDifferences(uint64_t max_n) {
	if (max_n > 0x00FFFFFF) {
		max_n = 0x00FFFFFF;
	}
	
	uint64_t i;
	uint8_t j, byte;
	uint32_t last, current;
	
	uint8_t* counts = calloc(sizeof(uint8_t), 0x00FFFFFF);
	if (counts == NULL) {
		perror("Al reservar memoria para el conteo");
		return;
	}
	uint64_t ic[BITS_IN_P + 1];
	
	double mean, variance;
	double total_mean = 0;
	double total_variance = 0;
	
	last = SBOX_hash_complete(0);
	
	for (i = 1; i <= max_n; last = current, i++) {
		current = SBOX_hash_complete(i);
		for (j = 0; j < (32/8); j++) {
			byte = ((last ^ current) >> (8 * j)) & 0xFF;
			counts[i - 1] += countSetBits(byte);
		}
	}
	
	memset(ic, 0, (BITS_IN_P + 1) * sizeof(uint64_t));
	for (i = 0, mean = 0 ; i < max_n - 1; i++) {
		mean += counts[i];
		ic[counts[i]]++;
	}
	mean /= max_n - 1.0;
	
	int c;	//to print neatly in an 4-column matrix
	
	printf("Para la caja S completa:\n");
	printf("\tRecuento de diferencias usando n y n+1:\n\t\t");
	for(i = 0, c = 0; i < BITS_IN_P + 1; i++) {
		if (ic[i] != 0) {
			printf("%lu (%lu)\t", i, ic[i]);
			c++;
		}
		if (c == 4 && i != BITS_IN_P) {
			printf("\n\t\t");
			c = 0;
		}
	}
	putchar('\n');
	
	printf("\tMedia de diferencias usando n y n+1: %lf (%.2lf%%)\n",
			mean, mean/BITS_IN_P*100);
	total_mean += mean;
	
	for (i = 0, variance = 0; i < max_n - 1; i++) {
		variance += (mean - counts[i]) * (mean - counts[i]);
	}
	
	variance /= max_n - 1.0;
	
	printf("\tVarianza de diferencias usando n y n+1: %.3lf\n", variance);
	total_variance += variance;
	
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
	free(counts);
}

void showIndividualLinearDependences(uint8_t max_n) {
	uint8_t dim  = 0;
	uint8_t i, j, k;
	uint64_t matrix;
	uint16_t values[4];
	uint16_t counts[NUM_S_BOXES];
	memset(counts, 0, NUM_S_BOXES * sizeof(uint16_t));
		
	for (dim = 1; max_n > (1 << (dim-1)); dim++);	//get dimension
	
	
	
	for (; max_n > 0; max_n --) {
		//exit condition is integer overflow
		for (matrix = 0, j = max_n; ((j - dim) & 0xC0) == 0; j-=dim) {
			for (k = 0; k < dim; k++) {
				matrix |= (j - k) << (dim * k);
			}
			
			if (bitMatrixDeterminant(dim, matrix) == 0) {
				//find out if there's linear dependencies between results
				for (i = 0; i < NUM_S_BOXES; i++) {
					for (k = j-1; k > 0 && ((k & 0xC0) == 0); k--) {
						values[0] = SBOX_hash(j, i);
						values[1] = SBOX_hash(k, i);
						if (areCoprime(values[0], values[1]) == 0) {
							counts[i]++;
						}
					}
				}
			}
		}
	}
	
	for (i = 0; i < NUM_S_BOXES; i++) {
		printf("Para la caja S %hhx se encontraron %hi dependencias lineales\n",
				1 + i, counts[i]);
	}
}

void showTotalLinearDependence(uint8_t max_n) {
	uint8_t dim  = 0;
	uint8_t i, j, k, l;
	uint64_t matrix;
	uint16_t count;
		
	for (dim = 1; max_n > (1 << (dim-1)); dim++);	//get dimension
	
	for (count = 0; max_n > 0; max_n --) {
		//exit condition is integer overflow
		for (matrix = 0, j = max_n; ((j - dim) & 0xC0) == 0; j -= dim) {
			for (k = 0; k < dim; k++) {
				matrix |= (j - k) << (dim * k);
			}
			
			if (bitMatrixDeterminant(dim, matrix) == 0) {
				//find out if there's linear dependencies between results for all boxes
				for (i = 0, l = 0; i < NUM_S_BOXES; i++) {
					for (k = j-1; k > 0 && ((k & 0xC0) == 0); k--) {
						if (areCoprime(SBOX_hash(j, i), SBOX_hash(k, i))
								== 0) {
							l++;
						}
					}
				}
				if (l == NUM_S_BOXES) {
					count++;
				}
			}
		}
	}
	
	printf("Se encontraron %hi parejas de valores con dependencias\
 lineales para todas las cajas S\n", count);
}


void showTotalLinearXORDependence(uint64_t max_n) {
	
	if (max_n > 0x00FFFFFF) {
		max_n = 0x00FFFFFF;
	}
	
	uint64_t i, j;
	uint16_t count, ids;
		
	for (count = 0, ids = 0, i = 0; i < max_n; i++) {
		for (j = 0; j <= i; j++) {
			if (SBOX_hash_complete(i + j)
					== (SBOX_hash_complete(i) ^ SBOX_hash_complete(j))) {
				count++;
				if (i == j) {
					ids++;
				}
			}
		}
	}
	
	printf("Se encontraron %hi parejas de valores con dependencias\
 lineales para todas las cajas S, de las cuales %hi son parejas\
 del mismo numero\n", count, ids);
}

int main (int argc, char* argv[]) {
	int c;
	uint8_t n = 0;
	uint64_t k = 0;
	uint8_t d = 0;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"n", required_argument, 0, 'n'},
		   {"k", required_argument, 0, 'k'},
		   {"d", required_argument, 0, 'd'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "n:k:d:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'n':
				n = atoi(optarg);
				if (n > 0x3F || n < 1) {
					printf("n debe ser menor que 64 y mayor que 1\n");
					return EXIT_FAILURE;
				}
				break;
			case 'k':
				k = strtoul(optarg, NULL, 10);
				if (k > 0x0000FFFFFFFFFFFF || k < 1) {
					printf("k debe ser menor que %ld y mayor que 1\n", 0x0001000000000000);
					return EXIT_FAILURE;
				}
				break;
			case 'd':
				d = atoi(optarg);
				if (d > 0x3F || d < 2) {
					printf("d debe ser menor que 64 y mayor que 2\n");
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s [-n num_pruebas] [-k num_pruebas]\
 [-d num_pruebas]\n", argv[0]);
				puts("n y k para probar cambios en entradas consecutivas (n para cajas S individuales)");
				puts("k tambien para probar independencia lineal por XOR");
				puts("d para probar independencia lineal por determinantes de matrices");
				return EXIT_FAILURE;
		}
	}
	
	if (n == 0 && k == 0 && d == 0) {
		printf("Uso: %s [-n num_pruebas] [-k num_pruebas]\
 [-d num_pruebas]\n", argv[0]);
		puts("n y k para probar cambios en entradas consecutivas (n para cajas S individuales)");
		puts("k tambien para probar independencia lineal por XOR");
		puts("d para probar independencia lineal por determinantes de matrices");
		return EXIT_FAILURE;
	}
	
	if (n != 0)
		showIndividualConsecutiveDifferences(n);
	if (k != 0) {
		showTotalConsecutiveDifferences(k);
		showTotalLinearXORDependence(k);
	}
	if (d > 1) {
		showIndividualLinearDependences(d);
		//showTotalLinearDependence(d);
	}
	return EXIT_SUCCESS;;
}
