#include <iostream>
#include <vector>
#include <chrono>

// 需外界确保输入为 1, 8, 64, 512
inline int HashToIdx(int page_num) {
	return ((page_num >> 3) & 1u) + (((page_num >> 6) & 1u) << 1) + (((page_num >> 9) & 1u) * 3);
}

inline int HashToIdx_B(uint32_t n) {
	//if (n == 0) return -1;
	//if (n & (n - 1)) return -1;   // 需为 2 的幂

	// 计算 z = ctz(n)
// #if defined(__GNUC__) || defined(__clang__)
// 	unsigned z = __builtin_ctz(n);
// #else
	// De Bruijn 法（只用乘法+位运算）
	static const unsigned char lut[32] = {
		0,1,28,2,29,14,24,3,30,22,20,15,25,17,4,8,
		31,27,13,23,21,19,16,7,26,12,18,6,11,5,10,9
	};
	unsigned idx = ((n & -n) * 0x077CB531u) >> 27;
	unsigned z = lut[idx];
// #endif

	// if (z % 3 != 0) return -1;    // 必须是 8 的幂（z 为 3 的倍数）

	// 无除法的“/3”：k = (z * 0xAAAAAAAB) >> 33  （用 64 位中间结果）
	uint64_t k = (uint64_t)z * 0xAAAAAAABull;
	return (int)(k >> 33);        // 1->0, 8->1, 64->2, 512->3
}


struct Timer {
public:
    std::chrono::high_resolution_clock::time_point start;
    Timer() : start(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        int64_t ns = duration;
        std::cout << "Elapsed time: " << duration << " microseconds (" << ns << " ns)" << std::endl;
    }
};

int main() {
    std::vector<int> vec {1, 8, 64, 512};

    long long repeat = 1ll << 30;
    long long sum_1 = 0;
    {
        
        Timer t;
        for (int i = 0; i < repeat; i++) {
            for (int j = 0; j < 4; j++) {
                sum_1 += HashToIdx(vec[j]);
            }
        }
    }
    printf("sum1 = %lli\n", sum_1);

    long long sum_2 = 0;
    {
        
        Timer t;
        for (int i = 0; i < repeat; i++) {
            for (int j = 0; j < 4; j++) {
                sum_2 += HashToIdx_B(vec[j]);
            }
        }
    }
    printf("sum2 = %lli\n", sum_2);
}