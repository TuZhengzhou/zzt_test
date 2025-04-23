#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>

// How to run this file
//      g++ hash_specification.cpp -o hash_specification -std=c++17
//      ./hash_specification

struct Person;
struct Person1;;
struct Person1Hash;

struct Person {
    std::string name;
    int age;
    Person(std::string name, int age): name(name), age(age) {}
    bool operator==(const Person& p) const {
        return name == p.name && age == p.age;
    }
};

// specialization std::hash
namespace std {
    template<>
    struct hash<Person> {
        size_t operator()(const Person& p) const {
            return std::hash<std::string>()(p.name) ^ std::hash<int>()(p.age);
        }
    };
}

// Hash functor class
struct PersonHash {
    size_t operator()(const Person& p) const {
        return std::hash<std::string>()(p.name) ^ std::hash<int>()(p.age);
    }
};

int main() {
    std::hash<Person> hash_fn;
    Person p1("Tom", 20);
    Person p2("Jack", 20);
    std::cout << hash_fn(p1) << std::endl;
    std::cout << hash_fn(p2) << std::endl;

    std::cout << "Hash by specialization std::hash" << std::endl;
    std::unordered_map<Person, std::string> mp;
    mp[p1] = "Devin";
    mp[p2] = "Fin";
    std::cout << mp.size() << std::endl;
    for(auto& [p, s]: mp) {
        std::cout << p.name << " " << p.age << " " << s << std::endl;
    }

    std::cout << "Hash by Person1Hash Functor" << std::endl;
    std::unordered_map<Person, std::string, PersonHash> mp1;
    Person p3{"Tom", 20};
    Person p4{"Jack", 20};
    mp1[p3] = "Devin";
    mp1[p4] = "Fin";
    std::cout << mp1.size() << std::endl;
    for(auto& [p, s]: mp1) {
        std::cout << p.name << " " << p.age << " " << s << std::endl;
    }

    std::cout << "Hash by lambda" << std::endl;
    auto arrayHash = [] (const Person& p) -> size_t {
        return std::hash<std::string>()(p.name) ^ std::hash<int>()(p.age);
    };
    /*
        decltype(arrayHash) 等价于 arrayHash 的实际类型（一个编译器生成的 lambda 类型）。
        
        为什么需要 decltype(arrayHash)？
        unordered_map 的模板参数需要 哈希函数的类型，而 arrayHash 是一个 对象（lambda 表达式），不是类型。
        decltype(arrayHash) 提取它的类型，使 unordered_map 知道该用哪种哈希函数。
        mp(0, arrayHash) 的 0 是初始桶数，arrayHash 是具体的哈希函数对象。
    */
    std::unordered_map<Person, std::string, decltype(arrayHash)> mp2(0, arrayHash);
    mp2[p3] = "Devin";
    mp2[p4] = "Fin";
    std::cout << mp2.size() << std::endl;
    for(auto& [p, s]: mp2) {
        std::cout << p.name << " " << p.age << " " << s << std::endl;
    }

    return 0;
}
