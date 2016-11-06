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
int bits_to_char(const uint8_t* bits);
int string_to_bits(const char* string, uint8_t* bits);
int bits_to_string(const uint8_t* bits, char* string);
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
int function_f(const uint8_t* r, const uint8_t* k, uint8_t* output);
int initial_permutation(const uint8_t* src, uint8_t* l, uint8_t* r);
int initial_permutation_inv(const uint8_t* l, const uint8_t* r, uint8_t* dst);

int cipher(uint8_t* input, uint8_t* output, uint8_t* k);
int decipher(uint8_t* input, uint8_t* output, uint8_t* k);

int main (int argc, char* argv[]) {
	int c;
	int i;
	int j;
	int len;
	int modo = -1;
	int n_to_add;
	FILE* fin = NULL;
	FILE* fout = NULL;
	uint8_t* k;
	uint8_t* input;
	uint8_t* output;
	char aux_k[9];
	char strbuf[MAX_STR];
	char* file_text = NULL;
	
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
				strcpy(aux_k, optarg);

				// 64 bits y el numero 2 como fin de cadena
				k = (uint8_t *) malloc ((DES_K_SIZE + 1) * sizeof(uint8_t));
				if (!k) {
					printf("Error al reservar memoria.\n");
					return EXIT_FAILURE;
				}

				if (string_to_bits(aux_k, k) < 0) {
					printf("Error al pasar la cadena a bits\n");
					free(k);
					return EXIT_FAILURE;
				}
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

		if (!k)
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

		if (modo == CIFRAR) {
			printf("Read %d bytes\n", len);

			toUpperOnly(strbuf);
			len = strlen(strbuf);

			// Añadimos padding en caso de que strbuf no sea multiplo de 64bits
			if ((len % 8) != 0) {
				if (len < 8) {
					if (add_n_padding(strbuf, strbuf, 8 - len) < 0) {
						free(k);
						return EXIT_FAILURE;
					}
				} else {
					n_to_add = len / 8;
					n_to_add = len - (8 * n_to_add);
					if (add_n_padding(strbuf, strbuf, n_to_add) < 0) {
						free(k);
						return EXIT_FAILURE;
					}
				}

				len = strlen(strbuf);
			}

			input = (uint8_t *) malloc (((8 * (len)) + 2) * sizeof(uint8_t));
			if (!input) {
				free(k);
				return EXIT_FAILURE;
			}

			output = (uint8_t *) malloc (((8 * len) + 2) * sizeof(uint8_t));
			if (!output) {
				free(k);
				free(input);
				return EXIT_FAILURE;
			}

			if (string_to_bits(strbuf, input) < 0) {
				free(k);
				free(input);
				free(output);
				return EXIT_FAILURE;
			}
			
			if (cipher(input, output, k) < 0) {
				free(k);
				free(input);
				free(output);
				return EXIT_FAILURE;
			}

			/*
			if (bits_to_string(output, strbuf) < 0) {
				free(k);
				free(input);
				free(output);
				return EXIT_FAILURE;
			}

			fwrite(strbuf, len, sizeof(char), fout);
			*/

			for (int i = 0; i < intlen(output); ++i)
			{
				fprintf(fout, "%d ", output[i]);
			}

			fprintf(fout, "\n");

		} else {
			input = (uint8_t *) malloc ((len + 2) * sizeof(uint8_t));
			if (!input) {
				free(k);
				return EXIT_FAILURE;
			}

			for (i = 0, j = 0; i < len; i++) {
				if (strbuf[i] == '0' || strbuf[i] == '1') {
					input[j] = strbuf[i] - '0';
					j++;
				}
			}
			input[j] = 2;

			printf("Read %d bytes\n", intlen(input));

			output = (uint8_t *) malloc (len * sizeof(uint8_t));
			if (!output) {
				free(k);
				free(input);
				return EXIT_FAILURE;
			}

			if (decipher(input, output, k) < 0) {
				free(k);
				free(input);
				free(output);
				return EXIT_FAILURE;
			}

			if (bits_to_string(output, strbuf) < 0) {
				free(k);
				free(input);
				free(output);
				return EXIT_FAILURE;
			}

			fwrite(strbuf, strlen(strbuf), sizeof(char), fout);
			fwrite("\n", 1, sizeof(char), fout);
		}

		free(input);
		free(output);
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

	string[index] = '\0';
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

	} else {
		for (i = 0; i < intlen(a); i++) {
			dst[i] = a[i] ^ b [i];
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

	for (i = 0; i < ROUNDS; i++) {
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

		intcpy(ks[i], aux_k);
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

int function_f(const uint8_t* r, const uint8_t* k, uint8_t* output) {

	if (!r || !k || !output)
		return -1;

	int i;
	int j;
	int l;
	int row;
	int column;
	int index;
	uint8_t bits[NUM_S_BOXES][6];
	uint8_t row_bits[4];
	uint8_t column_bits[6];
	uint8_t exp[50];
	uint8_t bits_per_box[NUM_S_BOXES][8];
	uint8_t after_sustitution[34];

	if (expansion(r, exp) < 0)
		return -1;

	if (xor(exp, exp, k) < 0)
		return -1;

	/* 	Divide el resultador de (exp XOR k) en bloques de 6 bits
		para realizar la sustitucion con las S-BOXES
	*/
	for (i = 0, j = 0, l = 0; i < intlen(exp); i+=6, j++) {
		intncpy(bits_per_box[j], exp + i, 6);
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
	for (i = 0, index = 0; i < NUM_S_BOXES; ++i, index+=4) {
		intncpy(after_sustitution + index, bits[i], 4);
	}

	after_sustitution[index] = 2;

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

int initial_permutation(const uint8_t* src, uint8_t* l, uint8_t* r) {

	if (!src || !l || !r)
		return -1;

	int i;
	int index = 0;

	for (i = 0; i < BITS_IN_IP/2; i++, index++) {
		l[i] = src[IP[index] - 1];
	}

	l[i] = 2;

	for (i = 0; i < BITS_IN_IP/2; i++, index++) {
		r[i] = src[IP[index] - 1];
	}

	r[i] = 2;
	return 0;
}

int initial_permutation_inv(const uint8_t* l, const uint8_t* r, uint8_t* dst) {

	if (!l || !r || !dst)
		return -1;

	int i;
	uint8_t src[66];

	if (intcpy(src, l) < 0)
		return -1;

	if (intcat(src, r) < 0)
		return -1;

	for (i = 0; i < BITS_IN_IP; i++) {
		dst[i] = src[IP_INV[i] - 1];
	}

	dst[i] = 2;	
	return 0;
}

int swap(uint8_t* a, uint8_t* b) {

	if (!a || !b)
		return -1;

	uint8_t aux[intlen(a) + 1];

	if (intcpy(aux, a) < 0)
		return -1;

	if (intcpy(a, b) < 0)
		return -1;

	if (intcpy(b, aux) < 0)
		return -1;

	return 0;
}

int cipher(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int i;
	int j;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t output_f[36];
	uint8_t output_xor[36];

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	if (initial_permutation(input, l, r) < 0)
		return -1;

	if (function_f(r, ks[0], output_f)	< 0)
			return -1;

	if (xor(output_xor, l, output_f) < 0)
		return -1;

	if (swap(output_xor, r) < 0)
			return -1;

	for (i = 1; i < ROUNDS; i++) {
		if (function_f(r, ks[i], output_f)	< 0)
			return -1;

		if (xor(output_xor, output_xor, output_f) < 0)
			return -1;

		if (swap(output_xor, r) < 0)
			return -1;
	}

	if (initial_permutation_inv(output_xor, r, output) < 0)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}

int decipher(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int i;
	int j;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t output_f[36];
	uint8_t output_xor[36];

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	if (initial_permutation(input, l, r) < 0)
		return -1;

	if (function_f(r, ks[ROUNDS - 1], output_f)	< 0)
			return -1;

	if (xor(output_xor, l, output_f) < 0)
		return -1;

	if (swap(output_xor, r) < 0)
			return -1;

	for (i = (ROUNDS - 2); i >= 0; i--) {
		if (function_f(r, ks[i], output_f)	< 0)
			return -1;

		if (xor(output_xor, output_xor, output_f) < 0)
			return -1;

		if (swap(output_xor, r) < 0)
			return -1;
	}

	if (initial_permutation_inv(output_xor, r, output) < 0)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}