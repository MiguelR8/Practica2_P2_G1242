#include "../includes/algoritmos.h"

void char_to_bits  (const char c, uint8_t* bits);
void string_to_bits(const char* string, uint8_t* bits);
void add_n_padding (const char* src, char* dst, int n_to_add);
void left_shift_n  (const uint8_t* array, uint8_t* array_shift, int n);
void intcpy(uint8_t* dst, const uint8_t* src);
void intncpy(uint8_t* dst, const uint8_t* src, int n);
// Funciona con el numero 2 como fin de cadena
int intlen(const uint8_t* array);
void remove_parity_bits(uint8_t* src, uint8_t* dst);

int main (int argc, char* argv[]) {
	int c;
	int len;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	uint8_t k[DES_K_SIZE + 1]; // 64 bits y el numero 2 como fin de cadena
	char aux_k[MAX_STR];
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
				/*
					La longitud de la clave ha de ser de 64bits (8 char),
					en caso contrario, se realiza padding
				*/
				//TODO: distinguir entre mayor que 8 (truncar o reportar error)
				// y menor que 8 (añadir padding)
				if (strlen(aux_k) != 8) {
					add_n_padding(aux_k, aux_k, 8 - strlen(aux_k));

					if (!aux_k) {
						printf("Error al añadir padding\n");
						return EXIT_FAILURE;
					}
				}

				string_to_bits(aux_k, k);
				if (!k) {
					printf("Error al pasar la cadena a bits\n");
					return EXIT_FAILURE;
				}

				k[DES_K_SIZE + 1] = 2;
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

	return 0;
}

void char_to_bits(const char c, uint8_t* bits) {

	int i;
	int j;

	if (!bits) {
		bits = NULL;
		return;
	}

    for (i = 7, j = 0; i >= 0; --i, ++j) {
        bits[j] = ( (c & (1 << i)) ? 1 : 0 );
    }

}

void string_to_bits(const char* string, uint8_t* bits) {

	if (!string || !bits) {
		bits = NULL;
		return;
	}

	int len;
	int i;
	int j;
	int index;
	uint8_t aux_bits[8];

	len = strlen(string);
	index = 0;
	for (i = 0; i < len; i++) {
		char_to_bits(string[i], aux_bits);
		if (!aux_bits) {
			return;
		}

		for (j = 0; j < 8; j++, index++) {
			bits[index] = aux_bits[j];
		}
	}
}

//TODO: cambiar(por que el relleno es A?): la clave no tiene por que ser alfabetica
void add_n_padding(const char* src, char* dst, int n_to_add) {

	if (!src || !dst || (sizeof(dst) + n_to_add) < (sizeof(src) + n_to_add)) {
		dst = NULL;
		return;
	}

	int i;
	int len = strlen(src);

	strcpy(dst, src);

	for (i = len; i < (len + n_to_add); i++) {
		dst[i] = 'A';
	}

	dst[i] = '\0';
	return;
}

void left_shift_n(const uint8_t* array, uint8_t* array_shift, int n) {

	if (!array || !array_shift)
		return;

	uint8_t aux_array[intlen(array) + 1];

	// Copia a partir de la posicion n de array hasta el final en array_shift
	intcpy(aux_array, array + n);
	if (!aux_array)
		return;

	// Copia al final de array_shift los primeros n elementos de array
	intncpy(aux_array + intlen(aux_array), array, n);
	if (!aux_array)
		return;

	intcpy(array_shift, aux_array);
}

void intcpy(uint8_t* dst, const uint8_t* src) {

	if (!dst || !src) {
		dst = NULL;
		return;
	}

	int i;
	int len = intlen(src);

	for (i = 0; i < len; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return;
}

void intncpy(uint8_t* dst, const uint8_t* src, int n) {

	if (!dst || !src) {
		dst = NULL;
		return;
	}

	int i;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return;
}

int intlen(const uint8_t* array) {

	if (!array)
		return -1;

	int i;

	for (i = 0; array[i] != 2; i++);

	return i;
}

void remove_parity_bits(uint8_t* src, uint8_t* dst) {

	// 56bits cadena resultante + fin de cadena => 56 * sizeof(int) + sizeof(int)
	if (!src || !dst) {
		dst = NULL;
		return;
	}

	/* 	Los bits de paridad ocuparan las posiciones:
			8, 16, 24, 32, 40, 48, 56 y 64
	*/
	int i;
	int j;

	for (i = 0, j = 0; j < 64; i+=7, j+=8) {
		intncpy(dst+i, src+j, 7);
	}
}

