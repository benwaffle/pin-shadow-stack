typedef int (*add_func)(int);

add_func adder(int a) {
	int adder2(int b) {
		return a+b;
	}
	return adder2;
}

int main() {
	add_func z = adder(3);
	printf("%d\n", z(4));
}
