#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>

#define MAX_STR 512

enum MODO {EQUIPROBABLE, DISPAR};

extern char *optarg;
extern int optind, opterr, optopt;

void toUpperOnly(char* src);

double average(int how_many, double* vals);
double* getAlphabetProbabilities(char* text);

// permutation functions
int getRandomLessN(int n);
void makePermutation(char* permutation, int n);
void fitArray(char* array, int posEle, int arraySize);
