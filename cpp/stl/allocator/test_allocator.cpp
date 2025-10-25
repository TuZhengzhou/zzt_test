#include <iostream>
#include <vector>
#include <memory>  // 包含 allocator 相关类型

// 全局栈缓冲区（256 MB）
char buffer[1 << 28];  // 1<<28 = 268,435,456 字节 = 256 MB
char* buffer_ptr = buffer;  // 追踪当前分配位置的指针

// 自定义分配器：使用 buffer 作为内存源
template <typename T>
class StackAllocator {
public:
    // 分配器必要的类型定义（C++11 起可简化，依赖 allocator_traits）
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // 重新绑定：用于容器内部需要分配其他类型时（如 vector 的内部指针）
    template <typename U>
    struct rebind {
        using other = StackAllocator<U>;
    };

    // 构造函数（无状态，默认即可）
    StackAllocator() noexcept = default;
    template <typename U>
    StackAllocator(const StackAllocator<U>&) noexcept {}

    // 核心：从 buffer 分配 n 个 T 类型的内存
    pointer allocate(size_type n) {
        // 计算需要的总字节数（注意对齐）
        const size_type bytes_needed = n * sizeof(T);
        // 检查缓冲区是否有足够空间
        if (buffer_ptr + bytes_needed > buffer + (1 << 28)) {
            throw std::bad_alloc();  // 空间不足
        }
        // 记录当前分配的起始地址
        pointer result = reinterpret_cast<pointer>(buffer_ptr);
        // 移动指针，更新已分配位置
        buffer_ptr += bytes_needed;
        return result;
    }

    // 核心：释放内存（简单实现，不实际回收，因为 buffer 是固定的）
    void deallocate(pointer /*p*/, size_type /*n*/) noexcept {
        // 注意：此处不修改 buffer_ptr，因为栈缓冲区无法真正"释放"
        // 复杂实现可维护空闲列表，但简单场景下省略
    }

    // 相等性判断（所有 StackAllocator 实例都共享同一个 buffer，因此相等）
    bool operator==(const StackAllocator&) const noexcept { return true; }
    bool operator!=(const StackAllocator&) const noexcept { return false; }
};

// 测试用的 Node 类（4KB 大小）
class Node {
public:
    int arr[1 << 10];  // 1024 个 int，共 4*1024 = 4096 字节 = 4KB
};

int main() {
    try {
        // 定义使用自定义分配器的 vector
        std::vector<Node, StackAllocator<Node>> vec;

        // 测试分配：计算最多能放多少个 Node（256MB / 4KB = 65536 个）
        const size_t max_nodes = (1 << 28) / sizeof(Node);
        std::cout << "理论最大可分配 Node 数量：" << max_nodes << std::endl;

        // 分配 Node 并测试
        vec.reserve(max_nodes);  // 预分配最大数量
        for (size_t i = 0; i < max_nodes; ++i) {
            vec.emplace_back();  // 构造 Node（无需额外内存，已预分配）
        }
        std::cout << "实际分配 Node 数量：" << vec.size() << std::endl;

        // 验证内存地址是否在 buffer 范围内
        Node* first = &vec[0];
        Node* last = &vec.back();
        std::cout << "第一个 Node 地址：" << static_cast<void*>(first) << std::endl;
        std::cout << "最后一个 Node 地址：" << static_cast<void*>(last) << std::endl;
        std::cout << "buffer 起始地址：" << static_cast<void*>(buffer) << std::endl;
        std::cout << "buffer 结束地址：" << static_cast<void*>(buffer + (1 << 28)) << std::endl;

    } catch (const std::bad_alloc& e) {
        std::cerr << "分配失败：" << e.what() << std::endl;
        return 1;
    }

    std::vector<int> vec1;
    std::vector<int> vec(std::move(vec1));

    return 0;
}