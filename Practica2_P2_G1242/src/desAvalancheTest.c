#include "../includes/des.h"

enum test_type {CLAVE, ENTRADA};

void showNRoundsCipherChange(int rounds, uint64_t data, uint64_t key, int test) {
	
	int	i, j;
	uint8_t* k;
	uint8_t* d;
	uint8_t* o;
	uint64_t bword, base_cipher, output;
	uint8_t ic[64];
	double mean;
	
	//as parameter type is int, transform in blocks of ints and concatenate
	uint8_t a;
	
	k = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (k == NULL) {
		perror(NULL);
		return;
	}
	d = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (d == NULL) {
		perror(NULL);
		free(k);
		return;
	}
	o = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (o == NULL) {
		free(k);
		free(d);
		perror(NULL);
		return;
	}
	
	for(i = 0; i < 64; i++) {
		a = (key >> i) & 0x01;
		intncpy(k + i, &a, 1);
		 
		a = (data >> i) & 0x01;
		intncpy(d + i, &a, 1);
	}
	
	//get cipher when nothing has changed
	cipherNRounds(rounds, d, o, k);
	
	bits_to_printable(o, (char*)o);
	base_cipher = strtoul((char*)o, NULL, 2);
	
	//then get cipher for changed parameter
	for (i = 0, mean = 0; i < 64; i++) {
		if (test == CLAVE) {
			bword = invertNthbit(key, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(k + j, &a, 1);
			}
		} else {
			bword = invertNthbit(data, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(d + j, &a, 1);
			}
		}
		cipherNRounds(rounds, d, o, k);
		
		output = 0;
		bits_to_printable(o, (char*)o);
		output = strtoul((char*)o, NULL, 2);
		
		ic[i] = countSetBits(output ^ base_cipher);
		mean += ic[i];
	}
	
	mean /= i;
	
	printf("Para el cifrado de %i ronda%s, al cambiar un bit de entrada hay,\
 en promedio: %.2lf bits cambiados (%.2lf%% del bloque)\n\t", rounds,
		(i == 1 ? "":"s"), mean, mean / i * 100);
 
	double variance = 0;
	for (i = 0; i < 64; i++) {
		variance = (ic[i] - mean) * (ic[i] - mean);
	}
	variance /= i;
	
	printf("Con una varianza de %lf\n", variance);
	
	free(k);
	free(d);
	free(o);
}

void showRoundNCipherChange(int round, uint64_t data, uint64_t key, int test) {
	
	int	i, j;
	uint8_t* k;
	uint8_t* d;
	uint8_t* o;
	uint64_t bword, base_cipher, output;
	uint8_t ic[64];
	double mean;
	
	if (round > 16 || round < 1)
		return;
	
	//as parameter type is int, transform in blocks of ints and concatenate
	uint8_t a;
	
	k = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (k == NULL) {
		perror(NULL);
		return;
	}
	d = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (d == NULL) {
		perror(NULL);
		free(k);
		return;
	}
	o = (uint8_t*) malloc (65 * sizeof(uint8_t));
	if (o == NULL) {
		free(k);
		free(d);
		perror(NULL);
		return;
	}
	
	for(i = 0; i < 64; i++) {
		a = (key >> i) & 0x01;
		intncpy(k + i, &a, 1);
		 
		a = (data >> i) & 0x01;
		intncpy(d + i, &a, 1);
	}
	
	//get cipher when nothing has changed
	cipherRoundN(round, d, o, k);
	
	bits_to_printable(o, (char*)o);
	base_cipher = strtoul((char*)o, NULL, 2);
	
	//then get cipher for changed parameter
	for (i = 0, mean = 0; i < 64; i++) {
		if (test == CLAVE) {
			bword = invertNthbit(key, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(k + j, &a, 1);
			}
		} else {
			bword = invertNthbit(data, i);
			for(j = 0; j < 64; j++) {
				a = (bword >> j) & 0x01;
				intncpy(d + j, &a, 1);
			}
		}
		cipherRoundN(round, d, o, k);
		
		output = 0;
		bits_to_printable(o, (char*)o);
		output = strtoul((char*)o, NULL, 2);
		
		ic[i] = countSetBits(output ^ base_cipher);
		mean += ic[i];
	}
	
	mean /= i;
	
	printf("Para el cifrado con la ronda %i, al cambiar un bit de entrada hay,\
 en promedio: %.2lf bits cambiados (%.2lf%% del bloque)\n\t", round, mean,
		mean / i * 100);
 
	double variance = 0;
	for (i = 0; i < 64; i++) {
		variance = (ic[i] - mean) * (ic[i] - mean);
	}
	variance /= i;
	
	printf("Con una varianza de %lf\n", variance);
	
	free(k);
	free(d);
	free(o);
}

int main (int argc, char* argv[]) {
	int c;
	uint32_t n = 0;
	uint64_t k = 0;
	uint64_t d = 0;
	int tipo_prueba = -1;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"I", no_argument, 0, 'I'},
		   {"K", no_argument, 0, 'K'},
		   {"n", required_argument, 0, 'n'},
		   {"k", required_argument, 0, 'k'},
		   {"d", required_argument, 0, 'd'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "IKn:k:d:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
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
				printf("Uso: %s {-I | -K} {-n max_rondas} {-k clave} {-d dato_prueba}\n", argv[0]);
				return EXIT_FAILURE;
		}
	}
	
	if (n == 0 || tipo_prueba == -1) {
		printf("Uso: %s {-I | -K} {-n max_rondas} {-k clave} {-d dato_prueba}\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	uint8_t i;
	for (i = 1; i <= n; i++) {
		showRoundNCipherChange(i, d, k, tipo_prueba);
	}
	showNRoundsCipherChange(n, d, k, tipo_prueba);
	return EXIT_SUCCESS;
}

