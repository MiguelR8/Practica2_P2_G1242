#include "../includes/algoritmos.h"

#define SAMPLE_SIZE 150
#define BIASED_P 0.7

char* getRandomKey(int size);

int calcularProbabilidadesTexto(char* text, int len, FILE* outfile, modo m);

int calcularProbabilidadesFichero(FILE* infile, FILE* outfile, modo m);

int calcularProbabilidades(char* text, int len, modo m,
		double probCifrado[ALPHA_SIZE],
		double probConjunto[ALPHA_SIZE][ALPHA_SIZE]);

int main (int argc, char* argv[]) {
	int c;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	char strbuf[MAX_STR];
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"P", no_argument, 0, 'P'},
		   {"I", no_argument, 0, 'I'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+PIi:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'I':
				modo = DISPAR;
				break;
			case 'P':
				modo = EQUIPROBABLE;
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
				printf("Uso: %s {-I|-P} [-i filein] [-o fileout]\n", argv[0]);
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
		printf("Se debe proporcionar la opcion -P (claves equiprobables)\
 o -I (no equiprobables)\n");
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
	
	int len;
	char* file_text = NULL;
	
	if (fin == stdin) {
		while(!feof(fin) && !ferror(fin)) {
			
			fgets(strbuf, MAX_STR, fin);
			if(feof(fin)) {
				break;
			}
			len = strlen(strbuf);
			if (strbuf[len-1] == '\n') {
				strbuf[len-1] = '\0';
				len--; 			
			}
						
			toUpperOnly(strbuf);
			len = strlen(strbuf);

			calcularProbabilidadesTexto(strbuf, len, fout, modo);
		}
	} else {
		calcularProbabilidadesFichero(fin, fout, modo);
	}
	
	fclose(fin);
	fclose(fout);
	return 0;
}

char* getRandomKey(int size) {
	char alpha_perm[ALPHA_SIZE];
	
	char* key = (char *) calloc (size, sizeof(char));
	if (key == NULL) {
		return NULL;
	}
	
	makePermutation(key, size);
	
	int i;
	for (i = 0; i < size; i++) {
		key[i] = (key[i] % 26) + 'A';
	}
	
	return key;
}


int calcularProbabilidades(char* text, int len, modo m,
		double probCifrado[ALPHA_SIZE],
		double probConjunto[ALPHA_SIZE][ALPHA_SIZE]) {
	double* cipherProbabilities;
	double** intersectionProbabilities;
	char* key;
	char* basekey;
	
	memset(probCifrado, 0, ALPHA_SIZE * sizeof(double));
	memset(probConjunto, 0, ALPHA_SIZE * ALPHA_SIZE * sizeof(double));
	
	char* ciphertext = (char*)malloc(len * sizeof(char));
	
	if (ciphertext == NULL) {
		perror("Al reservar memoria para cifrar el texto");
		return -1;
	}
	
	basekey = getRandomKey(len);
	if (basekey == NULL) {
		perror("Al generar clave para simular generacion dispar");
		free(ciphertext);
		return -1;
	}
	
	//As during random generation any key character can coincide
	//incidentally with the corresponding basekey character
	//bias must be adjusted to account for it so that chance
	//of coincidence trully is the provided bias
	const double adjusted_bias = 1
			- ((ALPHA_SIZE - 1)/(ALPHA_SIZE)) * (1 - BIASED_P);
			
	int i, j, k;
	
	
	for (i = 0; i < SAMPLE_SIZE; i++) {
		//generate key
		key = getRandomKey(len);
		if (key == NULL) {
			perror("Al obtener una clave para cifrar");
			free(ciphertext);
			free(basekey);
		}
		if (m == DISPAR) {
			for (j = 0; j < len; j++) {
				if (getRandomLessN(101)/100.0 < adjusted_bias) {
					key[j] = basekey[j];
				}
			}
		}
		
		//cipher
		for (j = 0; j < len; j++) {
			ciphertext[j] = text[j] - 'A' + key[j] - 'A';
			ciphertext[j] %= ALPHA_SIZE;
			ciphertext[j] += 'A';
		}
		free(key);
		
		//calculate ciphered probabilities
		cipherProbabilities = getAlphabetProbabilities(ciphertext, len);
		if (cipherProbabilities == NULL) {
			perror("Al reservar memoria para el texto plano");
			
			fprintf(stderr, "Clave: %s\n", key);
			free(ciphertext);
			free(basekey);
			return -1;
		}
		
		for (j = 0; j< ALPHA_SIZE; j++) {
			probCifrado[j] += cipherProbabilities[j];
		}
		free(cipherProbabilities);
		
		//calculate intersection probabilities
		intersectionProbabilities = getIntersectionAlphabetProbabilities(text, len, ciphertext); 
		for (j = 0; j < ALPHA_SIZE; j++) {
			for (k = 0; k < ALPHA_SIZE; k++) {
				probConjunto[j][k] += intersectionProbabilities[j][k];
			}
			free(intersectionProbabilities[j]);
		}
		free (intersectionProbabilities);
	}
	free(ciphertext);
	free(basekey);
	
	for (i = 0; i < ALPHA_SIZE; i++) {
		probCifrado[i] /= SAMPLE_SIZE;
		for (j = 0; j < ALPHA_SIZE; j++) {
			probConjunto[i][j] /= SAMPLE_SIZE;
		}
	}
}

int calcularProbabilidadesTexto(char* text, int len, FILE* outfile, modo m) {
	int i, j;
	
	double totalCipherProbabilities[ALPHA_SIZE];
	double totalIntersectionProbabilities[ALPHA_SIZE][ALPHA_SIZE];
	
	calcularProbabilidades(text, len, m, totalCipherProbabilities,
			totalIntersectionProbabilities);
	
	
	double* plainProbabilities = getAlphabetProbabilities(text, len);
	if (plainProbabilities == NULL) {
		perror("Al obtener la tabla de probabilidades de texto plano");
		return -1;
	}
	for (i = 0; i < ALPHA_SIZE; i++) {
		fprintf(outfile, "P(%c) = %.3lf\n", 'A' + i, plainProbabilities[i]);
	}
	free(plainProbabilities);
	
	for (i = 0; i < ALPHA_SIZE; i++) {
		for (j = 0; j < ALPHA_SIZE; j++) {
			if (totalCipherProbabilities[j] != 0) {
				fprintf(outfile, "P(%c|%c) = %.3lf ", 'A' + i, 'A' + j,
						totalIntersectionProbabilities[i][j]
								/ totalCipherProbabilities[j]);
			} else {
				fprintf(outfile, "P(%c|%c) = 0.000 ", 'A' + i, 'A' + j);
			}
		}
		fputc('\n', outfile);
	}
	return 0;
}

int calcularProbabilidadesFichero(FILE* infile, FILE* outfile, modo m) {
	char strbuf[MAX_STR];
	int len = 0;
	double plainProbTotal[ALPHA_SIZE];
	double cipherProbBuffer[ALPHA_SIZE];
	double cipherProbTotal[ALPHA_SIZE];
	double intersectProbBuffer[ALPHA_SIZE][ALPHA_SIZE];
	double intersectProbTotal[ALPHA_SIZE][ALPHA_SIZE];
	double* plainProbabilities;
	
	memset(plainProbTotal, 0, ALPHA_SIZE * sizeof(double));
	memset(cipherProbTotal, 0, ALPHA_SIZE * sizeof(double));
	memset(intersectProbTotal, 0, ALPHA_SIZE * ALPHA_SIZE * sizeof(double));
	
	int i, j, chars;
	
	for(chars = 0; !feof(infile) && !ferror(infile); chars += len) {
		
		memset(strbuf, 0, MAX_STR);
		fread(strbuf, sizeof(char), MAX_STR, infile);
		toUpperOnly(strbuf),
		len = strlen(strbuf);
		strbuf[len] = '\0';
		
		plainProbabilities = getAlphabetProbabilities(strbuf, len);
		if (plainProbabilities == NULL) {
			perror("Al obtener la tabla de probabilidades de texto plano");
			return -1;
		}
		calcularProbabilidades(strbuf, len, m, cipherProbBuffer,
				intersectProbBuffer);
				
		for (i = 0; i < ALPHA_SIZE; i++) {
			//probabilities should be later divided by the total text length
			plainProbTotal[i] += plainProbabilities[i] * len;
			cipherProbTotal[i] += cipherProbBuffer[i] * len;
			for (j = 0; j < ALPHA_SIZE; j++) {
				intersectProbTotal[i][j] += intersectProbBuffer[i][j] * len;
			}
		}
		
		free(plainProbabilities);
	}
	
	for (i = 0; i < ALPHA_SIZE; i++) {
		
		plainProbTotal[i] /= chars;
		
		cipherProbTotal[i] /= chars;
		for (j = 0; j < ALPHA_SIZE; j++) {
			intersectProbTotal[i][j] /= chars;
		}
	}
	
	for (i = 0; i < ALPHA_SIZE; i++) {
		fprintf(outfile, "P(%c) = %.3lf\n", 'A' + i, plainProbTotal[i]);
	}
	puts("|||");
	for (i = 0; i < ALPHA_SIZE; i++) {
		for (j = 0; j < ALPHA_SIZE; j++) {
			if (j != 0) {
				intersectProbTotal[i][0] += intersectProbTotal[i][j];
			}
			/*if (cipherProbTotal[j] != 0) {
				fprintf(outfile, "P(%c|%c) = %.3lf ", 'A' + i, 'A' + j,
						intersectProbTotal[i][j]
								/ cipherProbTotal[j]);
			} else {
				fprintf(outfile, "P(%c|%c) = 0.000 ", 'A' + i, 'A' + j);
			}*/
		}
		fprintf(outfile, "P(%c) = %.3lf ", 'A' + i,
					intersectProbTotal[i][0]);
		fputc('\n', outfile);
	}
	puts("------");
	return 0;
}
