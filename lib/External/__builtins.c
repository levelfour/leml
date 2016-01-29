#include <stdio.h>
#include <math.h>

void print_int(int i) {
	printf("%d", i);
}

void print_float(double f) {
	printf("%lf", f);
}

int truncate(double f) {
	return trunc(f);
}
