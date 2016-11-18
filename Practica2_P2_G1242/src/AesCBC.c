#include "../includes/aes.h"

int main (int argc, char* argv[]) {
	int c, i, len;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	FILE* kfile = NULL;
	uint32_t* k = (uint32_t*) calloc(4, sizeof(uint32_t));
	if (k == NULL) {
		perror("Al reservar memoria para la clave");
		return EXIT_FAILURE;
	}
	
	char strbuf[MAX_STR + CIPHER_BLOCK_SIZE];
	char strout[MAX_STR + CIPHER_BLOCK_SIZE];
	uint8_t IV[CIPHER_BLOCK_SIZE];
	memset(IV, 0, CIPHER_BLOCK_SIZE * sizeof(uint8_t));
	char* p;
	
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
				kfile = fopen(optarg, "r");
				if (kfile == NULL) {
					printf("Error al abrir %s para leer", optarg);
					if (fout != NULL) {
						fclose(fout);
					}
					if (fin != NULL) {
						fclose(fin);
					}
					free(k);
					return EXIT_FAILURE;
				}
				fgets(strbuf, MAX_STR, kfile);
				fclose(kfile);
				
				for (i = 0, p = strbuf; i < 4; i++) {
					k[i] = strtoul(p, &p, 16); 
					//nothing was read or end of input was read before getting all words
					if (p == strbuf || (*p == '\0' && i != 3)) {
						break;
					}
				}
				if (i != 4) {
					printf("Se leyeron %i palabras, se necesitan 4\n", i+1);
					if (fout != NULL) {
						fclose(fout);
					}
					if (fin != NULL) {
						fclose(fin);
					}
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
					free(k);
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
					free(k);
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
				free(k);
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
		free(k);
		return EXIT_FAILURE;
	}

	if (modo == DESCIFRAR && kfile == NULL) {
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
	
	if (modo == CIFRAR) {
		free(k);
		k = (uint32_t*)generate_AES_k(NK, "clave.txt");
		if (!k) {
			printf("Error al reservar memoria.\n");
			fclose(fin);
			fclose(fout);
			return EXIT_FAILURE;
		}
		
		printf("La clave utilizada es: ");
		for (i = 0; i < 4; i++) {
			printf("%08x ", k[i]);
		}
		putchar('\n');
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
		}
		
		if (modo == CIFRAR) {
			if (cipher_aes_cbc(strbuf, len, strout, IV, (uint8_t*)k, NK) < 0) {
				perror("Al cifrar");
				free(k);
				fclose(fin);
				fclose(fout);
				return EXIT_FAILURE;
			}
		} else {
			if (decipher_aes_cbc(strbuf, len, strout, IV, (uint8_t*)k, NK) < 0) {
				perror("Al descifrar");
				free(k);
				fclose(fin);
				fclose(fout);
				return EXIT_FAILURE;
			}
		}
		//round up to nearest block size multiple
		if (len % CIPHER_BLOCK_SIZE) {
			len += CIPHER_BLOCK_SIZE - (len % CIPHER_BLOCK_SIZE);
		}
		fwrite(strout, sizeof(uint8_t), len, fout);
	}

	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	free(k);

	return EXIT_SUCCESS;
}
