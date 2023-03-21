#define ITERATIONS 10

double cbrt(double x) {
	unsigned long long *ptr = (unsigned long long *) &x;
	unsigned long long n = (*ptr >> 52 & 0x7FF) - 1023;

	int p = n % 3;

	n = (n - p) / 3;

	double m = 1 << (n / (n % 3 + 1) + p);

	 for (int i = 0; i < ITERATIONS; ++i) {
		m = ((double) 1 / 3) * (2 * m + x / (m * m));
	 }

	 return m;
}
