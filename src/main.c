#include <stdio.h>
#include <omp.h>

#include "algorithm.h"

int main(void) {
	int i;
	char map[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F',
	};
	char hex[1001] = { '\0' };

#pragma omp parallel for schedule(static) shared(hex)
	for (i = 0; i < 1000; i++) {
		hex[i] = map[pi(i)];
	}
	printf("%s\n", hex);
	
	return 0;
}
