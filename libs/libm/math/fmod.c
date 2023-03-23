double fmod(double numer, double denom) {
	return numer - (double)(int) (numer / denom) * denom;
}
