#include "../includes/algoritmos.h"

void char_to_bits(const char c, int* bits);
void string_to_bits(const char* string, int* bits);
void add_n_padding(const char* src, char* dst, int n_to_add);

int main (int argc, char* argv[]) {
	int c;
	int len;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	int k[DES_K_SIZE];
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

void char_to_bits(const char c, int* bits) {

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

void string_to_bits(const char* string, int* bits) {

	if (!string || !bits) {
		bits = NULL;
		return;
	}

	int len;
	int i;
	int j;
	int index;
	int aux_bits[8];

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