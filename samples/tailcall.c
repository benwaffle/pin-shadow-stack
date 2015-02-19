int get_num(int a, int b) {
	if (a == 0) return b;
	else return get_num(a-1, b+a);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		fprintf(stderr, "Usage: %s <number>\n", argv[0]);
		return 1;
	}
	printf("%d\n", get_num(atoi(argv[1]), 0));
}
