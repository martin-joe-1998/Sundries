#include <iostream>

struct Data
{
    int x;
};

// Data dataはローカル変数なので、関数が終了すると破棄される。
// 本来の戻り値であるData& に存在するアドレスにアクセスすると、未定義動作になる。
// DummyFunc()は本来の戻り値のアドレスを上書きしてしまっているため、正しい値が出力されなかった。
Data CalcData(int input)
{
    Data data;
    data.x = input * 10;
    return data;
}

void DummyFunc()
{
    int dummy[100];
    for (int i = 0; i < 100; ++i)
    {
        dummy[i] = i;
    }
}

void Function1()
{
    Data data = CalcData(5);
    DummyFunc(); // この関数に変更しないでください、他の方法で修正してください。
    std::cout << data.x << std::endl;
}


int main()
{
    const int result = CalcData(5).x;
    std::cout << result << std::endl;
    Function1();
    std::getchar(); // コンソールウィンドウを開いたままにする
	return 0;
}

//-------------------------------------------------------------------------
// 期待される出力:
// 50
// 50


