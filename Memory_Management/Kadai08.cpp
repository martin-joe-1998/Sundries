#include <cstdio>
#include <string>

enum class ErrorCode : int
{
    kSuccess = 0,
    kNotFound,
    kError1,
    kError2,
    kError3
};

struct DummyData
{
    int x = 0;
};

ErrorCode LoadDataDummy(DummyData& /*o_data*/, const char* /*i_filePath*/)
{
    static int s_dummyError = 0;
    return static_cast<ErrorCode>(s_dummyError++);
}

const char* GetErrorMessage(ErrorCode i_code)
{
    switch (i_code)
    {
    case ErrorCode::kSuccess:
        return "Success";
    case ErrorCode::kNotFound:
        return "File not found";
    default:
    {
        static char buffer[30];
        snprintf(buffer, std::size(buffer), "Error %i",
            static_cast<int>(i_code) - static_cast<int>(ErrorCode::kError1) + 1);
        return buffer;
    }
    }
}

int main()
{
    static constexpr unsigned kTestCount = 3;
    static constexpr const char* kDataFile[kTestCount] = { "data0.dat", "data1.dat", "data2.dat" };
    static constexpr const char* kDataFallbackFile[kTestCount] = {
        "fallback/data0.dat", "fallback/data1.dat", "fallback/data2.dat"
    };
    DummyData data[kTestCount];
    for (unsigned i = 0; i < std::size(kDataFile); ++i)
    {
        const ErrorCode code1 = LoadDataDummy(data[i], kDataFile[i]);
        if (code1 != ErrorCode::kSuccess)
        {
            const ErrorCode code2 = LoadDataDummy(data[i], kDataFallbackFile[i]);
            if (code2 != ErrorCode::kSuccess)
            {
				// GetErrorMessage()では、固定アドレスの static char が再利用されているため、二回目の call で上書きされる。
                // 一旦キャッシュすることで解決
                const char* msg1 = GetErrorMessage(code1);
                const char* msg2 = GetErrorMessage(code2);
                printf("Failed to load data %u, from %s %s and %s %s\n", i, kDataFile[i], msg1,
                    kDataFallbackFile[i], msg2);
            }
        }
    }
    std::getchar(); // コンソールウィンドウを開いたままにする
	return 0;
}

//-------------------------------------------------------------------------
// 期待される出力:
// Failed to load data 1, from data1.dat File not found and fallback/data1.dat Error 1
// Failed to load data 2, from data2.dat Error 2 and fallback / data2.dat Error 3
//
// 注意: last error is Error "3"

