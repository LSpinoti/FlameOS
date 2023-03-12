/*
 * sqrt() breaks down doubles into its exponent and fractional parts,
 * then uses the square root of the exponent as an initial guess.
*/

#define ITERATIONS 10

double sqrt(double x) {
	unsigned long long *ptr = (unsigned long long *) &x;
	unsigned long long n = (*ptr >> 52 & 0x7FF) - 1023;

	int p = n % 2;

	n = (n - p) / 2;

	double m = 1 << (n / (n % 2 + 1) + p);

	 for (int i = 0; i < ITERATIONS; ++i) {
		m = 0.5 * (m + x / m);
	 }

	 return m;
}
