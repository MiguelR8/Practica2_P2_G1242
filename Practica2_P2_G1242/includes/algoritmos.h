#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_STR 512
#define ALPHA_SIZE 26
#define DES_K_SIZE 64 
#define MODULO_POLYNOMIAL 0x11B

typedef enum MODO {EQUIPROBABLE, DISPAR, CIFRAR, DESCIFRAR} modo;

extern char *optarg;
extern int optind, opterr, optopt;

void toUpperOnly(char* src);
void toUpperAndNumbersOnly(char* src);
int add_n_padding (const char* src, char* dst, int n_to_add);

double average(int how_many, double* vals);
double* getAlphabetProbabilities(char* text, int len);
double** getIntersectionAlphabetProbabilities (char* plaintext, long len, char* ciphertext);

// permutation functions
int getRandomLessN(int n);
int getRandomFromMAddN(int m, int n);
void makePermutation(char* permutation, int n);
void fitArray(char* array, int posEle, int arraySize);

//some useful byte operations
uint64_t countSetBits(uint64_t byte);
uint64_t invertNthbit (uint64_t word, uint8_t bit);
uint64_t nBitFilter (uint8_t len);
uint64_t rotateNBits (uint64_t word, uint8_t len, int8_t positions);

//byte polynomial functions
uint8_t degreeOf(uint16_t x);
uint16_t polyGDC(uint16_t a, uint16_t b);
void polyDiv(uint16_t n, uint16_t d, uint16_t* q, uint16_t* r);
uint8_t xtime (uint8_t x, uint8_t a, uint16_t m);
uint8_t polyMul(uint8_t a, uint8_t b, uint16_t m);
uint16_t polyMulInv(uint16_t a, uint16_t m);

uint32_t wordPolyMul(uint32_t a, uint32_t b);	//assumes x⁴+1 as the modulo

