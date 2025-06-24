#include <iostream>

class MyArrayClass {
public:
	int value;

	MyArrayClass() {
		std::cout << "Constructor called\n";
	}

	~MyArrayClass() {
		std::cout << "Destructor called\n";
	}

	// 以下に記述してください

	// new[] 演算子のオーバーロード
	static void* operator new[](std::size_t size) {
		std::cout << "Overload new[] operator called, size = " << size << " bytes.\n";
		void* ptr = std::malloc(size);
		if (!ptr) {
			throw std::bad_alloc();
		}
		return ptr;
	}

	// delete[] 演算子のオーバーロード
	static void operator delete[](void* ptr) noexcept {
		std::cout << "Overload delete[] operator called. Free the memory.\n";
		std::free(ptr);
	}
};

int main() {
	MyArrayClass* arr = new MyArrayClass[3];
	delete[] arr;
	return 0;
}
