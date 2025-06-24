#include <crtdbg.h>
#include <iostream>

enum class ItemCategory : int {
	kWeapon = 0,
	kArmor,
	kHead,
	kFoot,
	kAccessory,
	kCount
};

void Function0()
{
	// メモリリークの検出を開始
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	// 各カテゴリごとのアイテム数
	constexpr int kCategoryCount = static_cast<int>(ItemCategory::kCount);// カテゴリの総数
	constexpr int kCountPerCategory[] = { 5, 4, 3, 2, 1 };// 各カテゴリに属するアイテム数
	static_assert(kCategoryCount == std::size(kCountPerCategory), "Sizes are not matching");

	// 二次元配列のメモリ確保
	int **ppItemCount = new int*[kCategoryCount];
	for (int row = 0; row < kCategoryCount; ++row) {
		ppItemCount[row] = new int[kCountPerCategory[row]];
	}

	// 配列にデータを代入
	for (int row = 0; row < kCategoryCount; ++row) {
		for (int elem = 0; elem < kCountPerCategory[row]; ++elem) {
			ppItemCount[row][elem] = 0xaaaaaaa;
		}
	}

	// メモリの解放（※この解放方法ではメモリリークが発生します）
	for (int row = 0; row < kCategoryCount; ++row) {
		delete[] ppItemCount[row]; // 各行の配列を解放
	}

	delete[] ppItemCount;

	// メモリリークのチェック結果を出力
	_CrtDumpMemoryLeaks();
}

int main()
{
	Function0();
	return 0;
}
