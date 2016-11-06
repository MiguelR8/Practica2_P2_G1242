#include "../includes/algoritmos.h"

void toUpperOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        }
    }
    src[j] = '\0';
}

void toUpperAndNumbersOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        } else if (src[i] >= 48 && src[i] <= 57) {
        	src[j] = src[i];
            j++;
        }
    }
    src[j] = '\0';
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

double average(int how_many, double* vals) {
	int i = 0;
	double sum = 0;
	
	for (i = 0; i < how_many; i++) {
		sum += vals[i];
	}
	return sum/how_many;
}

//much flatter random number distribution even on fast executions
int getRandomLessN(int n) {
	return random() % n;
}

int getRandomFromMAddN(int m, int n) {
	return random() % n + m;
}

void makePermutation(char* permutation, int n) {

	char* auxPerm;
	int i;
	int j = n;
	int randomValue;

	auxPerm = (char *) calloc ((n + 1), sizeof(char));
	if (!auxPerm) {
		permutation = NULL;
		return;
	}

	for (i = 0; i < n; i++) {
		auxPerm[i] = i + 1;
	}

	for (i = 0; i < n; i++) {
		randomValue = getRandomLessN(j);

		memcpy(&permutation[i], &auxPerm[randomValue], sizeof(char));

		fitArray(auxPerm, randomValue, n);

		j--;
	}

	free(auxPerm);
}

void fitArray(char* array, int posEle, int arraySize) {

	int i;
	int j = 0;

	for (i = (posEle + 1); i < arraySize; i++) {
		memcpy(&array[i - 1], &array[i], sizeof(char));
		j++;
	}

	return;
}

//disregarding all other characters
double* getAlphabetProbabilities(char* text, int len) {
	int i, total;
	double* l = (double*) calloc(ALPHA_SIZE, sizeof(double));
	if (l == NULL) {
		return NULL;
	}
	
	for (i = 0, total = 0; i < len; i++) {
		if (isalpha(text[i])) {
			l[toupper(text[i]) - 'A']++;
			total++;
		}
	}
	for (i = 0; i < ALPHA_SIZE; i++) {
		l[i] = l[i]/((double)total);
	}
	return l;
}

double** getIntersectionAlphabetProbabilities (char* plaintext, long len, char* ciphertext) {
	int i, j, total;
	
	double** ll = (double**) malloc(ALPHA_SIZE * sizeof(double*));
	
	if (ll == NULL) {
		return NULL;
	}
	for (i = 0; i < ALPHA_SIZE; i++) {
		ll[i] = (double*) calloc(ALPHA_SIZE, sizeof(double));
		if (ll[i] == NULL) {
			for (i--; i >= 0; i--) {
				free(ll[1]);
			}
			free(ll);
			return NULL;
		}
	}
	
	char x, y;
	
	for (i = 0, total = 0; i < len; i++) {
		x = toupper(plaintext[i]);
		y = toupper(ciphertext[i]);
		if (isalpha(x) && isalpha(y)) {
			ll[x - 'A'][y - 'A']++;
			total++;
		}
	}
	
	for(i = 0; i <  ALPHA_SIZE; i++) {
		for (j = 0; j < ALPHA_SIZE; j++) {
			ll[i][j] /= total;
		}
	}
	return ll;
}
