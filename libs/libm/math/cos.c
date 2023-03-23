#include <math.h>

double cos(double x) {
	x = fmod(x, 2.0 * PI);

	return 1 -
			x * x / 2.0 +
			x * x * x * x / 24.0 -
			x * x * x * x * x * x / 720.0 +
			x * x * x * x * x * x * x * x / 40320.0 -
			x * x * x * x * x * x * x * x * x * x / 3628800.0;
}
