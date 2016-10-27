#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ALPHA_SIZE 26

char* getRandomKey(int size) {
	char alpha_perm[ALPHA_SIZE];
	
	char* key = (char *) calloc (size, sizeof(char));
	if (key == NULL) {
		return NULL;
	}
	
	for (i = 0; i < size; i += ALPHA_SIZE) {
		makePermutation(key + i, ALPHA_SIZE);
	}
	
	int i;
	for (i = 0; i < ALPHA_SIZE; i++) {
		key[i] += 'A';
	}
	
	return key;
}

int main (int argc; char** argv) {
	int c;
	int modo = -1;
	FILE* fin = NULL;
	FILE* fout = NULL;
	
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
				fin = fopen(filein, "rb");
				if (fin == NULL) {
					printf("Error al abrir %s para leer\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			case 'o':
				fout = fopen(optarg, "wb");
				if (fout == NULL) {
					printf("Error al abrir %s para escribir\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-I|-P} [-i filein] [-o fileout]\n", argv[0]);
				return -1;
		}
	}
	
	if (modo == -1) {
		printf("Se debe proporcionar la opcion -I (claves equiprobables)\
 o -P (no equiprobables)\n");
	}
	
	if (fin == NULL) {
		fin = stdin;
	}
	if (fout == NULL) {
		fout = stdout;
	}
	
	//read characters
	
	fclose(fin);
	fclose(fout);
	return 0;
}
