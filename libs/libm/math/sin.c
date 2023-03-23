#include <math.h>

double sin(double x) {
	x = fmod(x, 2.0 * PI);

	return x -
			x * x * x / 6.0 +
			x * x * x * x * x / 120.0 -
			x * x * x * x * x * x * x / 5040.0 +
			x * x * x * x * x * x * x * x / 362880.0 -
			x * x * x * x * x * x * x * x * x / 39916800.0;
}
