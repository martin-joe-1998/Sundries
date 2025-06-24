#include <iostream>
#include <chrono>

const int dataSize = 1000000;

void stackAllocation() {
	auto start = std::chrono::high_resolution_clock::now();

	// ここに記述してください
	// スタック上に配列を確保する
	int stackArray[dataSize];
	for (int i = 0; i < dataSize; ++i) {
		stackArray[i] = i;
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Stack allocation time: " << duration.count() << " seconds" << std::endl;
}

void heapAllocation() {
	auto start = std::chrono::high_resolution_clock::now();

	// ここに記述してください
	// ヒープ上に配列を確保する
	int* heapArray = new int[dataSize];
	for (int i = 0; i < dataSize; ++i) {
		heapArray[i] = i;
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Heap allocation time: " << duration.count() << " seconds" << std::endl;

	// メモリ解放
	delete[] heapArray;
}

int main() {
	std::cout << "Comparing stack and heap allocation performance..." << std::endl;

	stackAllocation();
	heapAllocation();

	return 0;
}
