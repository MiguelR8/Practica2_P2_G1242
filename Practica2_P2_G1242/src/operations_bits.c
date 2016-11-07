#include "../includes/operations_bits.h"

int char_to_bits(const char c, uint8_t* bits) {

	return convertDecimalToBinary(c, bits, 8);
}

int bits_to_char(const uint8_t* bits) {

	return convertBinaryToDecimal(bits);
}

int string_to_bits(const char* string, uint8_t* bits) {

	if (!string || !bits)
		return -1;

	int len;
	int i;
	int j;
	int index;
	uint8_t aux_bits[9];

	len = strlen(string);
	index = 0;
	for (i = 0; i < len; i++) {
		if (char_to_bits(string[i], aux_bits) < 0)
			return -1;

		for (j = 0; j < 8; j++, index++) {
			bits[index] = aux_bits[j];
		}
	}

	bits[index] = 2;
	return 0;
}

int bits_to_string(const uint8_t* bits, char* string) {

	if (!string || !bits)
		return -1;

	int i;
	int j;
	int index = 0;
	int len = intlen(bits);
	uint8_t aux_bits[9];

	for (i = 0, j = 0; i < len; i++) {
		aux_bits[j] = bits[i];

		if (j != 0 && (j % 7) == 0) {
			aux_bits[8] = 2;
			string[index] = bits_to_char(aux_bits);
			j = 0;
			index++;
		} else {
			j++;
		}
	}

	string[index] = '\0';
	return 0;
}

int bits_to_printable(const uint8_t* bits, char* string) {

	if (!string || !bits)
		return -1;

	int i;
	int len = intlen(bits);

	//simple transformation in case source and destination coincide 
	for (i = 0; i < len; i++) {
		string[i] = bits[i] + '0';
	}
	
	//swap (big-endian to little-endian)
	for (i = 0; i < len/2; i++) {
		string[i] 			= string[i] + string[len - 1 - i];	//x = x + y
		string[len - 1 - i] = string[i] - string[len - 1 - i];	//y = x - y
		string[i]			= string[i] - string[len - 1 - i];	//x = x - y
	}

	string[len] = '\0';
	return 0;
}


int rotatory_left_shift_n(const uint8_t* array, uint8_t* array_shift, int n) {

	if (!array || !array_shift)
		return -1;

	uint8_t aux_array[intlen(array) + 1];

	// Copia a partir de la posicion n de array hasta el final en array_shift
	if (intcpy(aux_array, array + n) < 0)
		return -1;

	// Copia al final de array_shift los primeros n elementos de array
	if (intncpy(aux_array + intlen(aux_array), array, n) < 0)
		return -1;

	if (intcpy(array_shift, aux_array) < 0)
		return -1;

	return 0;
}

int intcpy(uint8_t* dst, const uint8_t* src) {

	if (!dst || !src)
		return -1;

	int i;
	int len = intlen(src);

	if (len < 0)
		return -1;

	for (i = 0; i < len; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return 0;
}

int intncpy(uint8_t* dst, const uint8_t* src, int n) {

	if (!dst || !src)
		return -1;

	int i;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
	}

	dst[i] = 2;

	return 0;
}

int intlen(const uint8_t* array) {

	if (!array)
		return -1;

	int i;

	for (i = 0; array[i] != 2; i++);

	return i;
}

int intcat(uint8_t* dst, const uint8_t* src) {

	if (!dst || !src)
		return -1;

	if (intcpy(dst + intlen(dst), src) < 0)
		return -1;

	return 0;
}

int remove_parity_bits(const uint8_t* src, uint8_t* dst) {

	// 56bits cadena resultante + fin de cadena => 56 * sizeof(int) + sizeof(int)
	if (!src || !dst)
		return -1;

	/* 	Los bits de paridad ocuparan las posiciones:
			8, 16, 24, 32, 40, 48, 56 y 64
	*/
	int i;
	int j;

	for (i = 0, j = 0; j < 64; i+=7, j+=8) {
		if (intncpy(dst+i, src+j, 7) < 0)
			return -1;
	}

	return 0;
}

int xor(uint8_t* dst, const uint8_t* a, const uint8_t* b) {

	if (!dst || !a || !b)
		return -1;

	int i;

	/* 	Permite que dst y a o dst y b sean la misma variable al
	 	llamar a la funcion, p.ej. xor(a, a, b) */
	if (dst == a) {
		uint8_t aux_a[intlen(a) + 1];

		if (intcpy(aux_a, a) < 0)
			return -1;

		for (i = 0; i < intlen(aux_a); i++) {
			dst[i] = aux_a[i] ^ b [i];
		}

	} else if (dst == b) {
		uint8_t aux_b[intlen(b) + 1];

		if (intcpy(aux_b, b) < 0)
			return -1;

		for (i = 0; i < intlen(aux_b); i++) {
			dst[i] = a[i] ^ aux_b [i];
		}

	} else {
		for (i = 0; i < intlen(a); i++) {
			dst[i] = a[i] ^ b [i];
		}
	}

	dst[i] = 2;
	return 0;
}

int div_2_28bits (const uint8_t* src, uint8_t* c, uint8_t* d) {

	if (!src || !c || !d)
		return -1;

	if (intncpy(c, src, 28) < 0)
		return -1;

	if (intncpy(d, src + 28, 28) < 0)
		return -1;

	return 0;
}

int convertBinaryToDecimal(const uint8_t* n) {

	if (!n)
		return -1;

	int i;
	int j;
	int decimalNumber = 0;

    for (i = (intlen(n) - 1), j = 0; i >= 0; i--, j++) {
        decimalNumber += n[i] * pow(2, j);
    }

    return decimalNumber;
}

/* 	num_bits indica el numero de bits con los que se quiere representar
 	el numero decimal, p.ej: dec = 100 y num_bits = 16 => 
 	bits = 00000000 01100100
 */
int convertDecimalToBinary(const int dec, uint8_t* bits, int num_bits) {

	if (!bits)
		return -1;

	int i;
	int tempDec = dec;

	for (i = (num_bits - 1); i >= 0; --i) {
		if (tempDec) {
			bits[i] = tempDec  % 2;
			tempDec /= 2;
		} else
			bits[i] = 0;
	}

	bits[num_bits] = 2;
	return 0;
}
