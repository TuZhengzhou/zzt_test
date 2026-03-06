// template<
//     class Key,
//     class T,
//     class Hash = std::hash<Key>,
//     class KeyEqual = std::equal_to<Key>,
//     class Allocator = std::allocator<std::pair<const Key, T>>
// > class unordered_map;

#include <system_error>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <cxxabi.h>

struct Node {
    std::string name;
    int val;
    void* ptr;
    std::shared_ptr<int> sptr;
    std::thread::id t_id;
    std::error_code ecode;

    bool operator==(const Node& other) const {
        return this->name == other.name &&\
               this->val == other.val && \
               this->ptr == other.ptr && \
               this->sptr == other.sptr && \
               this->t_id == other.t_id && \
               this->ecode == other.ecode;
    }
};

template <>
struct std::hash<Node> {
    size_t operator()(const Node& node) const {
        return std::hash<std::string>{}(node.name) \
               ^ (std::hash<int>{}(node.val) << 1) \
               ^ (std::hash<void*>{}(node.ptr) << 2) \
               ^ (std::hash<std::shared_ptr<int>>{}(node.sptr) >> 1) \
               ^ (std::hash<std::thread::id>{} (node.t_id) >> 2) \
               ^ std::hash<std::error_code>{}(node.ecode);
    }
};

// template <>
// struct std::equal_to<Node> {
//     size_t operator()(const Node& n1, const Node& n2) const {
//         return n1 == n2;
//     }
// };

// 辅助函数：解码类型名
std::string demangle(const char* mangled_name) {
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> res{
        abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : mangled_name;
}

int main() {
    std::unordered_map<Node, int> umap;
    std::cout << demangle(typeid(umap).name()) << std::endl;

    Node n1{"test", 1, nullptr, nullptr, std::this_thread::get_id(), {}};
    // 这行会导致编译错误：没有 operator==
    umap[n1] = 42;
    
    // 但可以编译（只是声明类型）
    std::equal_to<Node> eq;  // 这也只是声明
    
    // 实际调用才会报错
    bool b = eq(n1, n1);  // 编译错误
}