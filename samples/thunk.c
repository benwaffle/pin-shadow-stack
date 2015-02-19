typedef int (*func)();

func a(int i){
	int x = i*2;
	int b() {
		return x+i;	
	}
	return b;
}

int main() {
	func b = a(3);
	printf("%d\n", b());
}
