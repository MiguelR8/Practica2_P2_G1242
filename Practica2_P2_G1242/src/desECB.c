#include "../includes/des.h"

int main (int argc, char* argv[]) {
	int c;
	int i;
	int j;
	int len;
	int modo = -1;
	int n_to_add;
	FILE* fin = NULL;
	FILE* fout = NULL;
	uint8_t* k = NULL;
	uint8_t* input;
	uint8_t* output;
	char aux_k[9];
	char strbuf[MAX_STR];
	
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

	if (modo == DESCIFRAR && !k) {
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

		if (modo == CIFRAR) {
			// 64 bits y el numero 2 como fin de cadena
			k = (uint8_t *) malloc ((DES_K_SIZE + 1) * sizeof(uint8_t));
			if (!k) {
				printf("Error al reservar memoria.\n");
				return EXIT_FAILURE;
			}

			generate_k(aux_k);

			printf("La clave utilizada es: %s\n", aux_k);

			if (string_to_bits(aux_k, k) < 0) {
				printf("Error al pasar la cadena a bits\n");
				free(k);
				return EXIT_FAILURE;
			}

			printf("Read %d bytes\n", len);

			//~ toUpperAndNumbersOnly(strbuf);
			//~ len = strlen(strbuf);

			// Añadimos padding en caso de que strbuf no sea multiplo de 64bits
			if ((len % 8) != 0) {
				if (len < 8) {
					if (add_n_padding(strbuf, strbuf, 8 - len) < 0) {
						free(k);
						return EXIT_FAILURE;
					}
				} else {
					n_to_add = len / 8;
					n_to_add = (8 * (n_to_add + 1)) - len;
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

			for (i = 0; i < intlen(output); ++i) {
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
