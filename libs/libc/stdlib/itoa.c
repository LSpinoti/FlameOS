unsigned int getdigits(unsigned long long num, unsigned int base) {
	if (num == 0)
		return 1;
	int counter = 0;
	while (num) {
		num /= base;
		++counter;
	}
	return counter;
}

int itoa(unsigned long long num, char *out, unsigned int base) {
	const int digits = getdigits(num, base);
	int count = digits;
	char digit[] = { '0', '1', '2', '3', '4', '5', '6', '7',
					 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
					 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
					 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
					 'W', 'X', 'Y', 'Z'};
	while (count) {
		out[count - 1] = digit[num % base];
		num /= base;
		--count;
	}
	out[digits] = '\0';
	return digits;
}
