#include <stdio.h>
#include <math.h>

void print_int(int i) {
	printf("%d", i);
	fflush(stdout);
}

void print_float(double f) {
	printf("%.10lf", f);
	fflush(stdout);
}

void print_char(int i) {
	printf("%c", (char)i);
	fflush(stdout);
}

void print_bool(int b) {
	printf(b ? "true" : "false");
	fflush(stdout);
}

void print_newline() {
	printf("\n");
}

int read_int() {
	int i;
	scanf("%d", &i);
	return i;
}

double read_float() {
	double d;
	scanf("%lf", &d);
	return d;
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

double atan(double f);

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
