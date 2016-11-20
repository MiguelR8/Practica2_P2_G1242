#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../includes/operations_bits.h"
#include "../includes/algoritmos.h"
#include "../tables/DES_tables.c"

#define BITS_28 30
#define BITS_32 34
#define BITS_48 50
#define BITS_56 58 
#define BITS_64 66
#define DES_ECB_K_SIZE 64 
#define DES_CBC_K_SIZE 192 

int generate_k(char* k, int size_k);

int pc_1(const uint8_t* src, uint8_t* c, uint8_t* d);
int pc_2(const uint8_t* src, uint8_t* dst);
int key_generator(const uint8_t* k, uint8_t** ks);
int expansion(const uint8_t* src, uint8_t* dst);
int permutation(const uint8_t* src, uint8_t* dst);
int function_f(const uint8_t* r, const uint8_t* k, uint8_t* output);
int initial_permutation(const uint8_t* src, uint8_t* l, uint8_t* r);
int initial_permutation_inv(const uint8_t* l, const uint8_t* r, uint8_t* dst);

int cipher_des(uint8_t* input, uint8_t* output, uint8_t* k);
int decipher_des(uint8_t* input, uint8_t* output, uint8_t* k);
int cipher_des_ecb(uint8_t* input, uint8_t* output, uint8_t* k);
int cipher_des_cbc(uint8_t* input, uint8_t* output, uint8_t* k, uint8_t* iv);
int decipher_des_ecb(uint8_t* input, uint8_t* output, uint8_t* k);
int decipher_des_cbc(uint8_t* input, uint8_t* output, uint8_t* k, uint8_t* iv);

int cipherNRounds(int rounds, uint8_t* input, uint8_t* output, uint8_t* k);
int cipherRoundN(int round, uint8_t* input, uint8_t* output, uint8_t* k);
