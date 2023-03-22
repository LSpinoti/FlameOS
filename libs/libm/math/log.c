#include <math.h>

double log(double x) {
	if (x <= 0) return NAN;

	return 1 +
			(x - E) / E -
			(x - E) * (x - E) / (2 * E * E) +
			(x - E) * (x - E) * (x - E) / (3 * E * E * E) -
			(x - E) * (x - E) * (x - E) * (x - E) / (4 * E * E * E * E);
}
