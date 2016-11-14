#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include "../tables/AES_tables.c"
#include "../includes/algoritmos.h"


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

int main (int argc, char* argv[]) {
	int c;
	uint8_t i, j;
	modo m = 0xFF;
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
	
	if (m != CIFRAR && m != DESCIFRAR) {
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
