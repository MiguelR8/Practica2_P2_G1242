#include <stdio.h>
#include <stdlib.h>

typedef struct {unsigned int data : 4;} nibble;

int main () {
	nibble a = {3};
	printf("%hhx\n", a.data);
	return 0;
}
