#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#define MAX_STR 512
#define ALPHA_SIZE 26

typedef enum MODO {EQUIPROBABLE, DISPAR} modo;

extern char *optarg;
extern int optind, opterr, optopt;

void toUpperOnly(char* src);

double average(int how_many, double* vals);
double* getAlphabetProbabilities(char* text, int len);
double** getIntersectionAlphabetProbabilities (char* plaintext, long len, char* ciphertext);

// permutation functions
int getRandomLessN(int n);
void makePermutation(char* permutation, int n);
void fitArray(char* array, int posEle, int arraySize);
