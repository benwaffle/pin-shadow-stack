#include <iostream>

struct thing {
	virtual int getInt() = 0;

	void unsafe() {
		throw 1234;
	}
};

struct item : thing {
	int getInt() {
		return 4;
	}
};

int main() {
	std::cout << item().getInt() << "\n";
	item().unsafe();
}
