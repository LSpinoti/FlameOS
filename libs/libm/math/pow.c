#include <math.h>

#define ITERATIONS 10

double pow(double base, double exponent) {
	if (exponent == 0.0) {
		if (base < 0.0) return INFINITY;
		else return 1.0;
	}

	return exp(exponent * log(base));
}
