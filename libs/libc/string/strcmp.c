int strcmp(const char *str1, const char *str2) {
	while (*str1 == *str2) {
		if (*str1 == '\0') return 0;
		++str1;
		++str2;
	}
	if (*str1 > *str2) return 1;
	else return -1;
}
