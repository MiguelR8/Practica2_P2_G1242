#include "../includes/des.h"

int generate_k(char* k, int size_k) {

	if (!k || size_k <= 0)
		return -1;

	int i;
	int random;

	for (i = 0; i < size_k; i++) {
		random = getRandomLessN(2);

		// Generacion de letra
		if (random) {
			k[i] = getRandomFromMAddN('A', 'Z' - 'A');

		} else { // Generacion de numero
			k[i] = getRandomFromMAddN('0', '9' - '0');
		}
	}

	k[i] = '\0';
	return 0;
}

int pc_1(const uint8_t* src, uint8_t* c, uint8_t* d) {

	if (!src || !c || !d)
		return -1;

	int i;
	int j = 0;

	for (i = 0; i < BITS_IN_PC1/2; i++, j++) {
		c[i] = src[PC1[j] - 1];
	}

	c[i] = 2;

	for (i = 0; i < BITS_IN_PC1/2; i++, j++) {
		d[i] = src[PC1[j] - 1];
	}

	d[i] = 2;

	return 0;
}

int pc_2(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;

	// La contraccion se realiza en el mismo bucle
	for (i = 0; i < BITS_IN_PC2; i++) {
		dst[i] = src[PC2[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int key_generator(const uint8_t* k, uint8_t** ks) {

	if (!k || !ks)
		return -1;

	int i;
	uint8_t c[29];
	uint8_t d[29];
	uint8_t cat_c_d[57];
	uint8_t aux_k[49];

	// C0 y D0
	if (pc_1(k, c, d) < 0)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		// Cn
		if (rotatory_left_shift_n(c, c, ROUND_SHIFTS[i]) < 0)
		 	return -1;

		// Dn
		if (rotatory_left_shift_n(d, d, ROUND_SHIFTS[i]) < 0)
		 	return -1;

		if (intcpy(cat_c_d, c) < 0)
			return -1;

		if (intcat(cat_c_d, d) < 0)
			return -1;

		// Kn
		if (pc_2(cat_c_d, aux_k) < 0)
			return -1;

		intcpy(ks[i], aux_k);
	}

	return 0;
}

int expansion(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;

	for (i = 0; i < BITS_IN_E; i++) {
		dst[i] = src[E[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int permutation(const uint8_t* src, uint8_t* dst) {

	if (!src || !dst)
		return -1;

	int i;
	uint8_t aux_src[intlen(src) + 1];

	/* 	Permite que src y dst sean la misma variable al
	 	llamar a la funcion, p.ej. permutation(src, src) */
	if (intcpy(aux_src, src) < 0)
		return -1;

	for (i = 0; i < BITS_IN_P; i++) {
		dst[i] = aux_src[P[i] - 1];
	}

	dst[i] = 2;
	return 0;
}

int function_f(const uint8_t* r, const uint8_t* k, uint8_t* output) {

	if (!r || !k || !output)
		return -1;

	int i;
	int j;
	int row;
	int column;
	int index;
	uint8_t bits[NUM_S_BOXES][6];
	uint8_t row_bits[4];
	uint8_t column_bits[6];
	uint8_t exp[50];
	uint8_t bits_per_box[NUM_S_BOXES][8];
	uint8_t after_sustitution[34];

	if (expansion(r, exp) < 0)
		return -1;

	if (xor(exp, exp, k) < 0)
		return -1;

	/* 	Divide el resultador de (exp XOR k) en bloques de 6 bits
		para realizar la sustitucion con las S-BOXES
	*/
	for (i = 0, j = 0; i < intlen(exp); i += 6, j++) {
		intncpy(bits_per_box[j], exp + i, 6);
	}

	// Realiza la sustitucion con las S-BOXES
	for (i = 0; i < NUM_S_BOXES; i++) {
		row_bits[0] = bits_per_box[i][0];
		row_bits[1] = bits_per_box[i][5];
		row_bits[2] = 2;

		column_bits[0] = bits_per_box[i][1];
		column_bits[1] = bits_per_box[i][2];
		column_bits[2] = bits_per_box[i][3];
		column_bits[3] = bits_per_box[i][4];
		column_bits[4] = 2;

		row = convertBinaryToDecimal(row_bits);
		column = convertBinaryToDecimal(column_bits);

		if (row < 0 || column < 0)
			return -1;

		if (convertDecimalToBinary(S_BOXES[i][row][column], bits[i], 4) < 0)
			return -1;
	}

	// Pasamos el resultado de la sustitucion a un unico array
	for (i = 0, index = 0; i < NUM_S_BOXES; ++i, index+=4) {
		intncpy(after_sustitution + index, bits[i], 4);
	}

	after_sustitution[index] = 2;

	if (permutation(after_sustitution, output) < 0)
		return -1;

	return 0;
}

int initial_permutation(const uint8_t* src, uint8_t* l, uint8_t* r) {

	if (!src || !l || !r)
		return -1;

	int i;
	int index = 0;

	for (i = 0; i < BITS_IN_IP/2; i++, index++) {
		l[i] = src[IP[index] - 1];
	}

	l[i] = 2;

	for (i = 0; i < BITS_IN_IP/2; i++, index++) {
		r[i] = src[IP[index] - 1];
	}

	r[i] = 2;
	return 0;
}

int initial_permutation_inv(const uint8_t* l, const uint8_t* r, uint8_t* dst) {

	if (!l || !r || !dst)
		return -1;

	int i;
	uint8_t src[66];

	if (intcpy(src, l) < 0)
		return -1;

	if (intcat(src, r) < 0)
		return -1;

	for (i = 0; i < BITS_IN_IP; i++) {
		dst[i] = src[IP_INV[i] - 1];
	}

	dst[i] = 2;	
	return 0;
}

int swap(uint8_t* a, uint8_t* b) {

	if (!a || !b)
		return -1;

	uint8_t aux[intlen(a) + 1];

	if (intcpy(aux, a) < 0)
		return -1;

	if (intcpy(a, b) < 0)
		return -1;

	if (intcpy(b, aux) < 0)
		return -1;

	return 0;
}

int cipher_des(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int i;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t output_f[36];
	uint8_t output_xor[36];

	output[0] = 2;

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	if (initial_permutation(input, l, r) < 0)
		return -1;

	if (function_f(r, ks[0], output_f)	< 0)
			return -1;

	if (xor(output_xor, l, output_f) < 0)
		return -1;

	if (swap(output_xor, r) < 0)
			return -1;

	for (i = 1; i < (ROUNDS - 1); i++) {
		if (function_f(r, ks[i], output_f)	< 0)
			return -1;

		if (xor(output_xor, output_xor, output_f) < 0)
			return -1;

		if (swap(output_xor, r) < 0)
			return -1;
	}

	if (function_f(r, ks[ROUNDS - 1], output_f)	< 0)
			return -1;

	if (xor(output_xor, output_xor, output_f) < 0)
		return -1;

	if (initial_permutation_inv(output_xor, r, output) < 0)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}

int decipher_des(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int i;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t output_f[36];
	uint8_t output_xor[36];

	output[0] = 2;

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	if (initial_permutation(input, l, r) < 0)
		return -1;

	if (function_f(r, ks[ROUNDS - 1], output_f)	< 0)
		return -1;

	if (xor(output_xor, l, output_f) < 0)
		return -1;

	if (swap(output_xor, r) < 0)
		return -1;

	for (i = (ROUNDS - 2); i >= 1; i--) {
		if (function_f(r, ks[i], output_f)	< 0)
			return -1;

		if (xor(output_xor, output_xor, output_f) < 0)
			return -1;

		if (swap(output_xor, r) < 0)
			return -1;
	}

	if (function_f(r, ks[0], output_f)	< 0)
			return -1;

	if (xor(output_xor, output_xor, output_f) < 0)
		return -1;

	if (initial_permutation_inv(output_xor, r, output) < 0)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}

int cipher_des_ecb(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int times;
	int index;
	int repets;
	uint8_t aux_input[66];
	uint8_t aux_output[66];

	output[0] = 2;

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (cipher_des(aux_input, aux_output, k) < 0)
			return -1;

		intcat(output, aux_output);
	}

	return 0;
}

int cipher_des_cbc(uint8_t* input, uint8_t* output, uint8_t* k, uint8_t* iv) {

	if (!input || !output || !k || !iv)
		return -1;

	int i;
	int times;
	int index;
	int repets;
	uint8_t keys[3][66];
	uint8_t aux_input[66];
	uint8_t aux_output[66];

	output[0] = 2;

	for (i = 0, index = 0; i < 3; i++, index+=64) {
		intcpy(keys[i], k + index);
	}

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (xor(aux_input, aux_input, iv) < 0)
			return -1;

		if (cipher_des(aux_input, aux_output, keys[0]) < 0)
			return -1;

		if (decipher_des(aux_output, aux_input, keys[1]) < 0)
			return -1;

		if (cipher_des(aux_input, aux_output, keys[2]) < 0)
			return -1;

		intcat(output, aux_output);
	}

	return 0;	
}

int decipher_des_ecb(uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k)
		return -1;

	int times;
	int index;
	int repets;
	uint8_t aux_input[66];
	uint8_t aux_output[66];

	output[0] = 2;

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (decipher_des(aux_input, aux_output, k) < 0)
			return -1;

		intcat(output, aux_output);
	}

	return 0;
}

int decipher_des_cbc(uint8_t* input, uint8_t* output, uint8_t* k, uint8_t* iv) {

	if (!input || !output || !k || !iv)
		return -1;

	int i;
	int times;
	int index;
	int repets;
	uint8_t aux_input[66];
	uint8_t aux_output[66];
	uint8_t keys[3][66];

	output[0] = 2;

	for (i = 0, index = 0; i < 3; i++, index+=64) {
		intcpy(keys[i], k + index);
	}

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (decipher_des(aux_input, aux_output, keys[2]) < 0)
			return -1;

		if (cipher_des(aux_output, aux_input, keys[1]) < 0)
			return -1;

		if (decipher_des(aux_input, aux_output, keys[0]) < 0)
			return -1;

		if (xor(aux_output, aux_output, iv) < 0)
			return -1;

		intcat(output, aux_output);
	}

	return 0;	
}

int cipherNRounds(int rounds, uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k || rounds > ROUNDS || rounds < 1)
		return -1;

	if (rounds == 1) {
		return cipherRoundN(1, input, output, k);
	}

	int i;
	int times;
	int index;
	int repets;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t aux_input[66];
	uint8_t aux_output[66];
	uint8_t output_f[36];
	uint8_t output_xor[36];

	output[0] = 2;

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (initial_permutation(aux_input, l, r) < 0)
			return -1;

		if (function_f(r, ks[0], output_f)	< 0)
				return -1;

		if (xor(output_xor, l, output_f) < 0)
			return -1;

		if (swap(output_xor, r) < 0)
				return -1;

		for (i = 1; i < (rounds - 1); i++) {
			if (function_f(r, ks[i], output_f)	< 0)
				return -1;

			if (xor(output_xor, output_xor, output_f) < 0)
				return -1;

			if (swap(output_xor, r) < 0)
				return -1;
		}

		if (function_f(r, ks[rounds - 1], output_f)	< 0)
				return -1;

		if (xor(output_xor, output_xor, output_f) < 0)
			return -1;

		if (initial_permutation_inv(output_xor, r, aux_output) < 0)
			return -1;

		intcat(output, aux_output);
	}

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}

int cipherRoundN(int round, uint8_t* input, uint8_t* output, uint8_t* k) {

	if (!input || !output || !k || round > ROUNDS || round < 1)
		return -1;

	int i;
	int times;
	int index;
	int repets;
	uint8_t l[34];
	uint8_t r[34];
	uint8_t** ks;
	uint8_t aux_input[66];
	uint8_t aux_output[66];
	uint8_t output_f[36];
	uint8_t output_xor[36];

	output[0] = 2;

	ks = (uint8_t **) malloc ((ROUNDS + 1) * sizeof(uint8_t *));
	if (!ks)
		return -1;

	for (i = 0; i < ROUNDS; i++) {
		ks[i] = (uint8_t *) malloc (50 * sizeof(uint8_t));
		if (!ks[i])
			return -1;
	}

	if (key_generator(k, ks) < 0)
		return -1;

	times = intlen(input) / 64;
	for (repets = 0, index = 0; repets < times; repets++, index += 64) {
		intncpy(aux_input, input + index, 64);

		if (initial_permutation(aux_input, l, r) < 0)
			return -1;

		if (function_f(r, ks[round - 1], output_f)	< 0)
				return -1;

		if (xor(output_xor, l, output_f) < 0)
			return -1;

		if (initial_permutation_inv(output_xor, r, aux_output) < 0)
			return -1;

		intcat(output, aux_output);
	}

	for (i = 0; i < ROUNDS; i++) {
		free(ks[i]);
	}

	free(ks);
	return 0;
}
