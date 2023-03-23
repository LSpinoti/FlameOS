double ceil(double x) {
	if (x > 0.0) ++x;
	return (double)(int) x;
}
