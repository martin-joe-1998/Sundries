#include <iostream>

void stackArrayExample() {
	// ここに記述してください
	const int SIZE = 5;
	int stack[SIZE];

	for (int i = 0; i < SIZE; ++i) {
		stack[i] = i;
	}

	for (int i = 0; i < SIZE; ++i) {
		std::cout << "stack[" << i << "] = " << stack[i] << std::endl;
	}
}

int main() {
    stackArrayExample();
    return 0;
}
