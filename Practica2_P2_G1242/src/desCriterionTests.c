#include "../includes/des.h"

enum test_type {CLAVE, ENTRADA};
enum criterion {BIC, SAC};

void showNRoundsBIC(int rounds, uint64_t data, uint64_t key, int test) {
	
	int	i, j, k, count;
	uint8_t* bkey;
	uint8_t* d;
	uint8_t* o;
	uint64_t bword, base_cipher, output, diff;
	double ic[64];	//chances of changing for each bit 
	memset(ic, 0, 64 * sizeof(double));
	double mean;
	
	//as parameter type is int, transform in blocks of ints and concatenate
	uint8_t a;
	
	bkey = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (bkey == NULL) {
		perror(NULL);
		return;
	}
	d = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (d == NULL) {
		perror(NULL);
		free(bkey);
		return;
	}
	o = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (o == NULL) {
		free(bkey);
		free(d);
		perror(NULL);
		return;
	}
	
	for(i = 0; i < 64; i++) {
		a = (key >> i) & 0x01;
		intncpy(bkey + i, &a, 1);
		 
		a = (data >> i) & 0x01;
		intncpy(d + i, &a, 1);
	}
	
	//get cipher when nothing has changed
	cipherNRounds(rounds, d, o, bkey);
	
	bits_to_printable(o, (char*)o);
	base_cipher = strtoul((char*)o, NULL, 2);
	
	double** cc = (double**) malloc(64 * sizeof(double*));
	if (cc == NULL) {
		perror(NULL);
		free(d);
		free(bkey);
		free(o);
		return;
	}
	for (i = 0; i < 64; i++) {
		cc[i] = (double*) calloc(64, sizeof(double));
		if (cc[i] == NULL) {
			perror(NULL);
			for (i--; i >= -1; i--) {
				free(cc[i]);
			}
			free(cc);
			free(d);
			free(bkey);
			free(o);
			return;
		}
	}
	
	//then get cipher for changed parameter
	for (i = 0, mean = 0; i < 64; i++) {
		if (test == CLAVE) {
			bword = invertNthbit(key, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(bkey + j, &a, 1);
			}
		} else {
			bword = invertNthbit(data, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(d + j, &a, 1);
			}
		}
		cipherNRounds(rounds, d, o, bkey);
		
		output = 0;
		bits_to_printable(o, (char*)o);
		output = strtoul((char*)o, NULL, 2);
		diff = (output ^ base_cipher);
		for (j = 0, count = 0; j < 8; j++) {
			count  += countSetBits(((uint8_t*)(&diff))[j]);
		}
		
		for(j = 0; j < 64; j++) {
			if (diff & (1 << j)) {
				ic[j]++;
			}
		}
		
		for (j = 0; j < 64; j++) {
			if (diff & (1 << j)) {
				for (k = 0; k < 64; k++) {
					if (diff & (1 << k)) {
						cc[j][k]++;
					}
				}
			}
		}
	}
	
	//normalize
	for (j = 0; j < 64; j++) {
		ic[j] /= 64;
		for (k = 0; k < 64; k++) {
			cc[j][k] /= 64;
		}
	}
	
	double abs;
	for (i = 0, mean = 0; i < 64; i++) {
		for (j = i + 1; j < 64; j++) {
			abs = cc[i][j] - ic[i] * ic[j];
			if (abs < 0) {
				abs = -abs;
			}
			mean += abs;
		}
	}
	mean /= 64 * (64 + 1) / 2;
	
	printf("Para esta prueba la probabilidad de cambio de un bit en la entrada es %.3lf\n", 1/64.);
	printf("Para el cifrado de %i ronda%s, al cambiar un bit de entrada\
 la probabilidad de cambio de dos bits difiere de esa misma probabilidad\
 siendo los cambios independientes en %.3lf\n", rounds,
			(i == 1 ? "":"s"), mean);
	
	double variance = 0;
	for (i = 0; i < 64; i++) {
		for (j = i + 1; j < 64; j++) {
			abs = cc[i][j] - ic[i] * ic[j];
			if (abs < 0) {
				abs = -abs;
			}
			variance += (abs - mean) * (abs - mean);
		}
	}
	variance /= 64 * (64 + 1) / 2;
	printf("La varianza para estas diferencias es %.2lf\n",
			variance);
	for (i = 0; i < 64; i++) {
		free(cc[i]);
	}
	free(cc);
	
	free(bkey);
	free(d);
	free(o);
}

void showNRoundsSAC(int rounds, uint64_t data, uint64_t key, int test) {
	int	i, j;
	uint8_t* bkey;
	uint8_t* d;
	uint8_t* o;
	uint64_t bword, base_cipher, output, diff;
	uint8_t ic[64];	//total changes for each bit
	memset(ic, 0, 64 * sizeof(uint8_t));
	double mean;
	
	//as parameter type is int, transform in blocks of ints and concatenate
	uint8_t a;
	
	bkey = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (bkey == NULL) {
		perror(NULL);
		return;
	}
	d = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (d == NULL) {
		perror(NULL);
		free(bkey);
		return;
	}
	o = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (o == NULL) {
		free(bkey);
		free(d);
		perror(NULL);
		return;
	}
	
	for(i = 0; i < 64; i++) {
		a = (key >> i) & 0x01;
		intncpy(bkey + i, &a, 1);
		 
		a = (data >> i) & 0x01;
		intncpy(d + i, &a, 1);
	}
	
	//get cipher when nothing has changed
	cipherNRounds(rounds, d, o, bkey);
	
	bits_to_printable(o, (char*)o);
	base_cipher = strtoul((char*)o, NULL, 2);
	
	//then get cipher for changed parameter
	for (i = 0, mean = 0; i < 64; i++) {
		if (test == CLAVE) {
			bword = invertNthbit(key, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(bkey + j, &a, 1);
			}
		} else {
			bword = invertNthbit(data, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(d + j, &a, 1);
			}
		}
		cipherNRounds(rounds, d, o, bkey);
		
		output = 0;
		bits_to_printable(o, (char*)o);
		output = strtoul((char*)o, NULL, 2);
		diff = (output ^ base_cipher);
		
		//count which bits have changed, for all input tests
		for (j = 0; j < 64; j++) {
			if (diff & (1 << j))
				ic[j]++;
		}
	}
	
	for (i = 0, mean = 0; i < 64; i++) {
		mean += ic[i];
	}
	mean /= i * 64;	//total tests and dword length
	
	printf("Para el cifrado de %i ronda%s, al cambiar un bit de entrada\
 la probabilidad de cambio promedio de un bit es %.3lf\n", rounds,
			(i == 1 ? "":"s"), mean);
	
	double variance = 0;
	for (i = 0; i < 64; i++) {
		variance += (mean - ic[i] / 64.) * (mean - ic[i] / 64.);
	}
	variance /= 64.;
	printf("La varianza para las probabilidades cambiando un bit es %.2lf\n",
			variance);
	
	free(bkey);
	free(d);
	free(o);
}


int main (int argc, char* argv[]) {
	int c;
	uint32_t n = 0;
	uint64_t k = 0;
	uint64_t d = 0;
	int tipo_prueba = -1;
	int criterio = -1;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"I", no_argument, 0, 'I'},
		   {"K", no_argument, 0, 'K'},
		   {"S", no_argument, 0, 'S'},
		   {"B", no_argument, 0, 'B'},
		   {"n", required_argument, 0, 'n'},
		   {"k", required_argument, 0, 'k'},
		   {"d", required_argument, 0, 'd'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "SBIKn:k:d:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'S':
				criterio = SAC;
				break;
			case 'B':
				criterio = BIC;
				break;
			case 'I':
				tipo_prueba = ENTRADA;
				break;
			case 'K':
				tipo_prueba = CLAVE;
				break;
			case 'n':
				n = atoi(optarg);
				if (n > 16 || n < 1) {
					printf("n debe estar entre 1 y 16\n");
					return EXIT_FAILURE;
				}
				break;
			case 'k':
				k = strtoul(optarg, NULL, 10);
				break;
			case 'd':
				d = strtoul(optarg, NULL, 10);
				break;
			default:
				printf("Uso: %s {-S | -B} {-I | -K} {-n max_rondas} {-k clave} {-d dato_prueba}\n", argv[0]);
				puts("-S realiza prueba de SAC, -B de BIC");
				puts("-I varia dato, -K varia solo clave, ambos no combinables");
				return EXIT_FAILURE;
		}
	}
	
	if (n == 0 || tipo_prueba == -1 || criterio == -1) {
		printf("Uso: %s {-S | -B} {-I | -K} {-n max_rondas} {-k clave} {-d dato_prueba}\n", argv[0]);
		puts("-S realiza prueba de SAC, -B de BIC");
		puts("-I varia dato, -K varia solo clave, ambos no combinables");
		return EXIT_FAILURE;
	}
	
	if (criterio == SAC) {
		showNRoundsSAC(n, d, k, tipo_prueba);
	} else {
		showNRoundsBIC(n, d, k, tipo_prueba);
	}
	return EXIT_SUCCESS;
}

