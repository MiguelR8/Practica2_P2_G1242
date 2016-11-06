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

int generate_k(char* k);

int pc_1(const uint8_t* src, uint8_t* c, uint8_t* d);
int pc_2(const uint8_t* src, uint8_t* dst);
int key_generator(const uint8_t* k, uint8_t** ks);
int expansion(const uint8_t* src, uint8_t* dst);
int permutation(const uint8_t* src, uint8_t* dst);
int function_f(const uint8_t* r, const uint8_t* k, uint8_t* output);
int initial_permutation(const uint8_t* src, uint8_t* l, uint8_t* r);
int initial_permutation_inv(const uint8_t* l, const uint8_t* r, uint8_t* dst);

int cipher(uint8_t* input, uint8_t* output, uint8_t* k);
int decipher(uint8_t* input, uint8_t* output, uint8_t* k);