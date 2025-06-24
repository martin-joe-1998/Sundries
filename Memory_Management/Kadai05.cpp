#include <iostream>

class TrackedClass {
public:
	int data;

	TrackedClass() {
		std::cout << "TrackedClass constructor\n";
	}

	~TrackedClass() {
		std::cout << "TrackedClass destructor\n";
	}

	// メモリ使用量を追跡する静的変数
	static size_t totalAllocated;

	// new演算子のオーバーロード
	static void* operator new(std::size_t size) {
		totalAllocated += size; // メモリ使用量を更新
		std::cout << "Overload new[] operator called, Allocating " << size << " bytes." << std::endl;
		void* ptr = std::malloc(size);
		if (!ptr) {
			throw std::bad_alloc();
		}
		return ptr;
	}

	// delete演算子のオーバーロード
	static void operator delete(void* ptr) noexcept {
		if (ptr) {
			totalAllocated -= sizeof(TrackedClass); // メモリ使用量を更新
			std::cout << "Overload delete[] operator called, free " << sizeof(TrackedClass) << " bytes." << std::endl;
			std::free(ptr);
		}
	}

	// 現在のメモリ使用量を表示
	static void showMemoryUsage() {
		std::cout << "Total memory allocated: " << totalAllocated << " bytes\n";
	}
};

// 静的変数の初期化
size_t TrackedClass::totalAllocated = 0;

int main() {
	TrackedClass* obj1 = new TrackedClass();
	TrackedClass* obj2 = new TrackedClass();
	TrackedClass::showMemoryUsage();

	delete obj1;
	TrackedClass::showMemoryUsage();

	delete obj2;
	TrackedClass::showMemoryUsage();

	return 0;
}
