#include "../includes/algoritmos.h"
#include "../tables/DES_tables.c"
#include <math.h>

int add_n_padding (const char* src, char* dst, int n_to_add);

int intcpy(uint8_t* dst, const uint8_t* src);
int intncpy(uint8_t* dst, const uint8_t* src, int n);
// Funciona con el numero 2 como fin de cadena
int intlen(const uint8_t* array);
int intcat(uint8_t* dst, const uint8_t* src);

int rotatory_left_shift_n  (const uint8_t* array, uint8_t* array_shift, int n);
int char_to_bits  (const char c, uint8_t* bits);
int string_to_bits(const char* string, uint8_t* bits);
int xor(uint8_t* dst, const uint8_t* a, const uint8_t* b);
int convertBinaryToDecimal(const uint8_t* n);
int convertDecimalToBinary(const int dec, uint8_t* bits, int num_bits);
//int remove_parity_bits(const uint8_t* src, uint8_t* dst);

int pc_1(const uint8_t* src, uint8_t* c, uint8_t* d);
int pc_2(const uint8_t* src, uint8_t* dst);
//int div_2_28bits (const uint8_t* src, uint8_t* c, uint8_t* d);
int key_generator(const uint8_t* k, uint8_t** ks);
int expansion(const uint8_t* src, uint8_t* dst);
int permutation(const uint8_t* src, uint8_t* dst);
int sustitution(const uint8_t* r, const uint8_t* k, uint8_t* output);

int main (int argc, char* argv[]) {
	int c;
	int len;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	uint8_t* k; 
	char* aux_k;
	char strbuf[MAX_STR];
	char* file_text = NULL;

	uint8_t** ks;
	ks = (uint8_t **) malloc (17 * sizeof(uint8_t));
	for(int i = 0; i < 17; i++) {
		ks[i] = (uint8_t *)malloc(49 * sizeof(uint8_t));
	}
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"k", required_argument, 0, 'k'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "CDk:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'C':
				modo = CIFRAR;
				break;
			case 'D':
				modo = DESCIFRAR;
				break;
			case 'k':

				aux_k = (char *) malloc ((strlen(optarg) + 1) * sizeof(char));
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

				k[DES_K_SIZE + 1] = 2;
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
			case 'o':
				fout = fopen(optarg, "wb");
				if (fout == NULL) {
					if (fin != NULL) {
						fclose(fin);
					}
					printf("Error al abrir %s para escribir\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C|-D -k clave} [-i filein] [-o fileout]\n", argv[0]);
				if (fout != NULL) {
					fclose(fout);
				}
				if (fin != NULL) {
					fclose(fin);
				}
				return EXIT_FAILURE;
		}
	}
	
	//TODO: comprobar introduccion al programa de la clave
	if (modo == -1) {
		printf("Uso: %s {-C|-D -k clave} [-i filein] [-o fileout]\n", argv[0]);
		if (fout != NULL) {
			fclose(fout);
		}
		if (fin != NULL) {
			fclose(fin);
		}

		free(k);
		return EXIT_FAILURE;
	}
	
	if (fin == NULL) {
		fin = stdin;
	}
	if (fout == NULL) {
		fout = stdout;
	}

	while(!feof(fin) && !ferror(fin)) {
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
			len = fread(strbuf, sizeof(char), MAX_STR, fin);
			strbuf[len] = '\0';
		}

		printf("Read %d bytes", len);

		if (modo == CIFRAR) {
			toUpperOnly(strbuf);
			len = strlen(strbuf);
			//TODO: añadir funcion de cifrado

		} else {
			//TODO: añadir funcion de descifrado
		}

	}

	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	free(k);

	return EXIT_SUCCESS;;
}

int char_to_bits(const char c, uint8_t* bits) {

	int i;
	int j;

	if (!bits)
		return -1;

    for (i = 7, j = 0; i >= 0; --i, ++j) {
        bits[j] = ( (c & (1 << i)) ? 1 : 0 );
    }

    return 0;
}

int string_to_bits(const char* string, uint8_t* bits) {

	if (!string || !bits)
		return -1;

	int len;
	int i;
	int j;
	int index;
	uint8_t aux_bits[8];

	len = strlen(string);
	index = 0;
	for (i = 0; i < len; i++) {
		if (char_to_bits(string[i], aux_bits) < 0)
			return -1;

		for (j = 0; j < 8; j++, index++) {
			bits[index] = aux_bits[j];
		}
	}

	return 0;
}

int add_n_padding(const char* src, char* dst, int n_to_add) {

	if (!src || !dst)
		return -1;

	int i;
	int len = strlen(src);

	if (len < 0)
		return -1; 

	strcpy(dst, src);

	for (i = len; i < (len + n_to_add); i++) {
		dst[i] = 'A';
	}

	dst[i] = '\0';
	return 0;
}

int rotatory_left_shift_n(const uint8_t* array, uint8_t* array_shift, int n) {

	if (!array || !array_shift)
		return -1;

	uint8_t aux_array[intlen(array) + 1];

	// Copia a partir de la posicion n de array hasta el final en array_shift
	if (intcpy(aux_array, array + n) < 0)
		return -1;

	// Copia al final de array_shift los primeros n elementos de array
	if (intncpy(aux_array + intlen(aux_array), array, n) < 0)
		return -1;

	if (intcpy(array_shift, aux_array) < 0)
		return -1;

	return 0;
}

int intcpy(uint8_t* dst, const uint8_t* src) {

	if (!dst || !src)
		return -1;

	int i;
	int len = intlen(src);

	if (len < 0)
		return -1;

	for (i = 0; i < len; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return 0;
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

int intlen(const uint8_t* array) {

	if (!array)
		return -1;

	int i;

	for (i = 0; array[i] != 2; i++);

	return i;
}

int intcat(uint8_t* dst, const uint8_t* src) {

	if (!dst || !src)
		return -1;

	if (intcpy(dst + intlen(dst), src) < 0)
		return -1;

	return 0;
}

int remove_parity_bits(const uint8_t* src, uint8_t* dst) {

	// 56bits cadena resultante + fin de cadena => 56 * sizeof(int) + sizeof(int)
	if (!src || !dst)
		return -1;

	/* 	Los bits de paridad ocuparan las posiciones:
			8, 16, 24, 32, 40, 48, 56 y 64
	*/
	int i;
	int j;

	for (i = 0, j = 0; j < 64; i+=7, j+=8) {
		if (intncpy(dst+i, src+j, 7) < 0)
			return -1;
	}

	return 0;
}

int xor(uint8_t* dst, const uint8_t* a, const uint8_t* b) {

	if (!dst || !a || !b || intlen(a) != intlen(b))
		return -1;

	int i;

	/* 	Permite que dst y a o dst y b sean la misma variable al
	 	llamar a la funcion, p.ej. xor(a, a, b) */
	if (dst == a) {
		uint8_t aux_a[intlen(a) + 1];

		if (intcpy(aux_a, a) < 0)
			return -1;

		for (i = 0; i < intlen(a); i++) {
			dst[i] = aux_a[i] ^ b [i];
		}

	} else if (dst == b) {
		uint8_t aux_b[intlen(b) + 1];

		if (intcpy(aux_b, b) < 0)
			return -1;

		for (i = 0; i < intlen(a); i++) {
			dst[i] = a[i] ^ aux_b [i];
		}
	}

	dst[i] = 2;
	return 0;
}

int div_2_28bits (const uint8_t* src, uint8_t* c, uint8_t* d) {

	if (!src || !c || !d)
		return -1;

	if (intncpy(c, src, 28) < 0)
		return -1;

	if (intncpy(d, src + 28, 28) < 0)
		return -1;

	return 0;
}

int pc_1(const uint8_t* src, uint8_t* c, uint8_t* d) {

	if (!src || !c || !d)
		return -1;

	int i;
	int j = 0;

	for (i = 0; i < BITS_IN_PC1/2; i++, j++) {
		c[i] = src[PC1[j] - 1];
	}

	c[i] = 2;

	for (i = 0; i < BITS_IN_PC1/2; i++, j++) {
		d[i] = src[PC1[j] - 1];
	}

	d[i] = 2;

	return 0;
}

int pc_2(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;

	// La contraccion se realiza en el mismo bucle
	for (i = 0; i < BITS_IN_PC2; i++) {
		dst[i] = src[PC2[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int key_generator(const uint8_t* k, uint8_t** ks) {

	if (!k || !ks)
		return -1;

	int i;
	uint8_t c[29];
	uint8_t d[29];
	uint8_t cat_c_d[57];
	uint8_t aux_k[49];

	// C0 y D0
	if (pc_1(k, c, d) < 0)
		return -1;

	// C1
	if (rotatory_left_shift_n(c, c, ROUND_SHIFTS[0]) < 0)
	 	return -1;

	// D1
	if (rotatory_left_shift_n(d, d, ROUND_SHIFTS[0]) < 0)
	 	return -1;

	// Se concatenan c y d para pasar por PC2
	if (intcpy(cat_c_d, c) < 0)
		return -1;

	if (intcat(cat_c_d, d) < 0)
		return -1;

	// K1
	if (pc_2(cat_c_d, aux_k) < 0)
		return -1;

	ks[0] = aux_k;

	for (i = 1; i < ROUNDS; i++) {
		// Cn
		if (rotatory_left_shift_n(c, c, ROUND_SHIFTS[i]) < 0)
		 	return -1;

		// Dn
		if (rotatory_left_shift_n(d, d, ROUND_SHIFTS[i]) < 0)
		 	return -1;

		if (intcpy(cat_c_d, c) < 0)
			return -1;

		if (intcat(cat_c_d, d) < 0)
			return -1;

		// Kn
		if (pc_2(cat_c_d, aux_k) < 0)
			return -1;

		ks[i] = aux_k;
	}

	return 0;
}

int expansion(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;

	for (i = 0; i < BITS_IN_E; i++) {
		dst[i] = src[E[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int permutation(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;
	uint8_t aux_src[intlen(src) + 1];

	/* 	Permite que src y dst sean la misma variable al
	 	llamar a la funcion, p.ej. permutation(src, src) */
	if (intcpy(aux_src, src) < 0)
		return -1;

	for (i = 0; i < BITS_IN_P; i++) {
		dst[i] = aux_src[P[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int sustitution(const uint8_t* r, const uint8_t* k, uint8_t* output) {

	if (!r || !k || !output)
		return -1;

	int i;
	int j = 0;
	int row;
	int column;
	int index;
	uint8_t bits[NUM_S_BOXES][5];
	uint8_t row_bits[3];
	uint8_t column_bits[5];
	uint8_t exp[49];
	uint8_t bits_per_box[NUM_S_BOXES][7];
	uint8_t after_sustitution[33];

	if (expansion(r, exp) < 0)
		return -1;

	if (xor(exp, exp, k) < 0)
		return -1;

	/* 	Divide el resultador de exp XOR k en bloques de 6 bits
		para realizar la sustitucion con las S-BOXES
	*/
	for (i = 0; i < strlen(exp); i++) {
		bits_per_box[j][i] = exp[i];

		if (!(i % 6))
			j++;
	}

	// Realiza la sustitucion con las S-BOXES
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

	// Pasamos el resultado de la sustitucion a un unico array
	for (i = 0, index = 0; i < NUM_S_BOXES; ++i) {
		for (j = 0; j < 4; ++j, ++index) {
			after_sustitution[index] = bits[i][j];
		}
	}

	if (permutation(after_sustitution, output) < 0)
		return -1;

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

/* 	num_bits indica el numero de bits con los que se quiere representar
 	el numero decimal, p.ej: dec = 100 y num_bits = 16 => 
 	bits = 00000000 01100100
 */
int convertDecimalToBinary(const int dec, uint8_t* bits, int num_bits) {

	if (!bits)
		return -1;

	int i;
	int tempDec = dec;

	for (i = (num_bits - 1); i >= 0; --i) {
		if (tempDec) {
			bits[i] = tempDec  % 2;
			tempDec /= 2;
		} else
			bits[i] = 0;
	}

	bits[num_bits] = 2;
	return 0;
}

