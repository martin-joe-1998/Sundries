#include <iostream>

void heapArrayExample() {
	// ここに記述してください
	const int size = 5;
	// ヒープ領域に配列を確保
	int* heapArray = new int[size]; 

	for (int i = 0; i < size; ++i) {
		heapArray[i] = i;
	}

	for (int i = 0; i < size; ++i) {
		std::cout << "heapArray[" << i << "] = " << heapArray[i] << std::endl;
	}

	// ヒープ領域の配列を解放
	delete[] heapArray;
}

int main() {
	heapArrayExample();
	return 0;
}
