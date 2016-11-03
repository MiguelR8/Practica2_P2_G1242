#include "../tables/DES_tables.c"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 64

int intlen(const uint8_t* array) {

	if (!array)
		return -1;

	int i;

	for (i = 0; array[i] != 2; i++);

	return i;
}

int intncpy(uint8_t* dst, const uint8_t* src, int n) {

	if (!dst || !src)
		return -1;

	int i;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return 0;
}

int countSetBits(uint8_t* word) {
	int i, c;
	for (i = 0, c = 0; word[i] != 2; i++) {
		if (word[i] == 1) {
			c++;
		}
	}
	return c;
}

int xor(uint8_t* dst, const uint8_t* a, const uint8_t* b) {

	if (!dst || !a || !b)
		return -1;

	int i;

	/* 	Permite que dst y a o dst y b sean la misma variable al
	 	llamar a la funcion, p.ej. xor(a, a, b) */
	if (dst == a) {
		uint8_t aux_a[intlen(a) + 1];

		if (intcpy(aux_a, a) < 0)
			return -1;

		for (i = 0; i < intlen(aux_a); i++) {
			dst[i] = aux_a[i] ^ b [i];
		}

	} else if (dst == b) {
		uint8_t aux_b[intlen(b) + 1];

		if (intcpy(aux_b, b) < 0)
			return -1;

		for (i = 0; i < intlen(aux_b); i++) {
			dst[i] = a[i] ^ aux_b [i];
		}
	}

	dst[i] = 2;
	return 0;
}

int convertBinaryToDecimal(const uint8_t* n) {

	if (!n)
		return -1;

	int i;
	int j;
	int remainder;
	int decimalNumber = 0;

    for (i = (intlen(n) - 1), j = 0; i >= 0; i--, j++) {
        decimalNumber += n[i] * pow(2, j);
    }

    return decimalNumber;
}

int convertDecimalToBinary(const int dec, uint8_t* bits, int word_length) {

	if (!bits)
		return -1;

	int i;
	int tempDec = dec;

	for (i = (word_length - 1); i >= 0; --i) {
		if (tempDec) {
			bits[i] = tempDec  % 2;
			tempDec /= 2;
		} else
			bits[i] = 0;
	}

	bits[word_length] = 2;
	return 0;
}

int char_to_bits(const char c, uint8_t* bits) {

	return convertDecimalToBinary(c, bits, 8);
}

int bits_to_char(const uint8_t* bits) {

	return convertBinaryToDecimal(bits);
}

int string_to_bits(const char* string, uint8_t* bits) {

	if (!string || !bits)
		return -1;

	int len;
	int i;
	int j;
	int index;
	uint8_t aux_bits[9];

	len = strlen(string);
	index = 0;
	for (i = 0; i < len; i++) {
		if (char_to_bits(string[i], aux_bits) < 0)
			return -1;

		for (j = 0; j < 8; j++, index++) {
			bits[index] = aux_bits[j];
		}
	}

	bits[index] = 2;

	return 0;
}

int bits_to_string(const uint8_t* bits, char* string) {

	if (!string || !bits)
		return -1;

	int i;
	int j;
	int index = 0;
	int len = intlen(bits);
	uint8_t aux_bits[9];

	for (i = 0, j = 0; i < len; i++) {
		aux_bits[j] = bits[i];
		j++;

		if (i && !(i % 7)) {
			aux_bits[8] = 2;
			string[index] = bits_to_char(aux_bits);
			j = 0;
			index++;
		}
	}
}


uint8_t* SBOX_hash (uint8_t* word) {
	const entry_length = BITS_IN_E/NUM_S_BOXES;
	
	if (word == NULL || intlen(word) != BITS_IN_E)
		return NULL;
	
	int word_length = intlen(exp);
	uint8_t bits_per_box[NUM_S_BOXES][entry_length + 1];
	
	for (i = 0; i < word_length; i += entry_length) {
		intncpy(bits_per_box[i / entry_length], exp + i);
	}

	uint8_t row_bits[3];
	uint8_t column_bits[5];
	int row, column;
	
	uint8_t bits[NUM_S_BOXES][5];
	
	for (i = 0; i < NUM_S_BOXES; i++) {
		row_bits[0] = bits_per_box[i][0];
		row_bits[1] = bits_per_box[i][5];
		row_bits[2] = 2;

		column_bits[0] = bits_per_box[i][1];
		column_bits[1] = bits_per_box[i][2];
		column_bits[2] = bits_per_box[i][3];
		column_bits[3] = bits_per_box[i][4];
		column_bits[4] = 2;

		row = convertBinaryToDecimal(row_bits);
		column = convertBinaryToDecimal(column_bits);

		if (row < 0 || column < 0)
			return -1;

		if (convertDecimalToBinary(S_BOXES[i][row][column], bits[i], 4) < 0)
			return -1;
	}
	
	uint8_t* after_sustitution = (uint8_t*)malloc((BITS_IN_P + 1) * sizeof(uint8_t));
	if (after_sustitution == NULL) {
		return NULL;
	}
	
	for (i = 0; i < NUM_S_BOXES; i) {
		intncpy(after_sustitution + 4*i, bits[i], 4);
	}
	
	return after_sustitution;
}

void showLinearDifferences(int max_rows, char* text) {
	
}

int main (int argc, char* argv[]) {
	int c;
	int len;
	int modo = -1;
	FILE* fin = NULL;
	uint8_t* k;
	uint8_t* input;
	uint8_t* output;
	char* aux_k;
	char strbuf[MAX_STR];
	char* file_text = NULL;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"k", required_argument, 0, 'k'},
		   {"i", required_argument, 0, 'i'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "k:i:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'k':
				aux_k = (char *) malloc (9 * sizeof(char));
				if (!aux_k) {
					printf("Error al reservar memoria\n");
					return EXIT_FAILURE;
				}

				strcpy(aux_k, optarg);
				/*
					La longitud de la clave ha de ser de 64bits (8 char),
					en caso contrario, se realiza padding
				*/
				if (strlen(aux_k) != 8) {
					if (add_n_padding(aux_k, aux_k, 8 - strlen(aux_k)) < 0) {
						printf("Error al añadir padding a la clave de entrada\n");
						free(aux_k);
						return EXIT_FAILURE;
					}
				}

				// 64 bits y el numero 2 como fin de cadena
				k = (uint8_t *) malloc ((DES_K_SIZE + 1) * sizeof(uint8_t));
				if (!k) {
					printf("Error al reservar memoria.\n");
					free(aux_k);
					return EXIT_FAILURE;
				}

				if (string_to_bits(aux_k, k) < 0) {
					printf("Error al pasar la cadena a bits\n");
					free(aux_k);
					free(k);
					return EXIT_FAILURE;
				}

				free(aux_k);
				break;
			case 'i':
				fin = fopen(optarg, "rb");
				if (fin == NULL) {
					if (fout != NULL) {
						fclose(fout);
					}
					printf("Error al abrir %s para leer\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-k clave} [-i filein]\n", argv[0]);
				if (fin != NULL) {
					fclose(fin);
				}
				return EXIT_FAILURE;
		}
	}
	
	if (k == NULL) {
		printf("Uso: %s {-k clave} [-i filein]\n", argv[0]);
		if (fin != NULL) {
			fclose(fin);
		}
		return EXIT_FAILURE;
	}
	
	if (fin == NULL) {
		fin = stdin;
	}
	
	int i;

	for(i = 0; !feof(fin) && !ferror(fin); i++) {
		if (fin == stdin) {
			fgets(strbuf, MAX_STR, fin);
			if(feof(fin)) {
				break;
			}
			len = strlen(strbuf);
			if (strbuf[len-1] == '\n') {
				strbuf[len-1] = '\0';
				len--; 			
			}

		} else {
			len = fread(strbuf, sizeof(char), (MAX_STR)/(8*sizeof(char)), fin);
			strbuf[len] = '\0';
		}

		printf("Bloque %d de %d bytes\n", i, len);
	}

	if (fin != NULL) {
		fclose(fin);
	}
	free(k);

	return EXIT_SUCCESS;;
}
