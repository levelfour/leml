#include <stdio.h>
#include <math.h>

void print_int(int i) {
	printf("%d", i);
}

void print_float(double f) {
	printf("%.10lf", f);
}

void print_newline() {
	printf("\n");
}

double fabs(double f);

double abs_float(double f) {
	return fabs(f);
}

int truncate(double f) {
	return trunc(f);
}

double sin(double f);

double cos(double f);

double sqrt(double f);

int int_of_float(double f) {
	if(f >= 0) {
		return floor(f);
	} else {
		return -floor(-f);
	}
}

double float_of_int(int i) {
	return i;
}
