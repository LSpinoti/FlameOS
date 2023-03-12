char* strcpy(char *dest, const char *src) {
	while ((*dest = *src) != '\0') {
		++dest;
		++src;
	}
	return dest;
}
