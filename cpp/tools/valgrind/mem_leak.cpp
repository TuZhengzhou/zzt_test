#include <iostream>
#include <cstring>
#include <cstdlib>

// g++ -g mem_leak.cpp -o mem_leak
// valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes -s ./mem_leak

// 1. Still reachable 演示（全局变量未释放）
int* global_ptr = new int[10];
int* possible;

// 内存覆盖演示结构体
struct Node {
    int* data;
    Node* next;
};

void create_memory_issues() {
    // ================= 1. 内存泄漏 =================
    // Definitely lost
    int* leak1 = new int[5]; // 未释放

    // Indirectly lost (node->data = new int[10] + node->next = new Node)
    // Directly lost (Node* node = new Node;)
    Node* node = new Node;
    node->data = new int[10];
    node->next = new Node; // 只释放了 node，未释放 node->data 和 node->next
    

    // ================= 2. 非法内存访问 =================
    int arr[3] = {1, 2, 3};
    std::cout << arr[5] << std::endl; // 越界访问

    // ================= 3. 使用未初始化值 =================
    int uninit;
    if (uninit > 0) { // 使用未初始化变量
        std::cout << "Uninitialized!" << std::endl;
    }

    // ================= 5. 内存覆盖 =================
    char src[10] = "abcdefghi";
    memcpy(src + 2, src, 5); // 内存重叠拷贝
    std::cout << "src: " << src << std::endl; // 输出可能不正确
}

// possibly_lost. 
void possibly_lost_demo() {
    int* p = new int[10];
    possible = p + 9; 
    // 当 p + x 中的 x \in [1,9] 时，possibly_lost
    // x = 0 时，still_reachable
    // x = 10 时，definitly lost
}

int main() {
    create_memory_issues();
    possibly_lost_demo();

    // ================= 4. 双重释放 =================
    int* double_free = new int;
    delete double_free;
    delete double_free; // Invalid free() / delete / delete[] / realloc()
    
    return 0;
}