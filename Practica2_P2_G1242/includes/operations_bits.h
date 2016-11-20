#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

int char_to_bits  (const char c, uint8_t* bits);
int bits_to_char(const uint8_t* bits);
int string_to_bits(const char* string, uint8_t* bits);
int bits_to_string(const uint8_t* bits, char* string);
int bits_to_printable(const uint8_t* bits, char* string);

int rotatory_left_shift_n  (const uint8_t* array, uint8_t* array_shift, int n);

int intcpy(uint8_t* dst, const uint8_t* src);
int intncpy(uint8_t* dst, const uint8_t* src, int n);
// Funciona con el numero 2 como fin de cadena
int intlen(const uint8_t* array);
int intcat(uint8_t* dst, const uint8_t* src);
int get_odd_parity(const uint8_t* src);

int remove_parity_bits(const uint8_t* src, uint8_t* dst);

int xor(uint8_t* dst, const uint8_t* a, const uint8_t* b);
int div_2_28bits (const uint8_t* src, uint8_t* c, uint8_t* d);

int convertBinaryToDecimal(const uint8_t* n);
int convertDecimalToBinary(const int dec, uint8_t* bits, int num_bits);
