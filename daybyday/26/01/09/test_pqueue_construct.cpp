#include <vector>
#include <queue>
#include <iostream>
#include <functional>

using namespace std;

bool compare_func(int a, int b) {
    return a < b;
}

template<>
struct std::less<int> {
    size_t operator()(int a, int b) {
        return a < b;
    }
};

int main() {
    using T = int;
    
    struct MyComp {
        bool operator()(T a, T b) {
            return a < b;
        }
    } MyCompInst;



    vector<T> data = {1,2,3};

    auto compare_lambda = [] (T a, T b) {
        return a < b;
    };

    priority_queue pqueue_default_compare(data.begin(), data.end());
    priority_queue<int, vector<int>> pqueue_default_compare2(data.begin(), data.end());

    /*
        若想不提供模板参数，需要指定数据源（自带数据类型）
        若不含/不使用默认比较函数，还需指定比较类对象/函数
    */
    priority_queue pqueue2(data.begin(), data.end(), MyCompInst);
    priority_queue pqueue3(data.begin(), data.end(), compare_lambda);
    priority_queue pqueue4(data.begin(), data.end(), compare_func);

    // 不指定数据源时，需在模板参数中指定数据类型 T 和容器 vector<T>
    // 使用比较类时，无需在构造参数时提供比较类对象(可实例化且类中已撰写具体 bool operator())
    priority_queue<T, vector<T>, MyComp> pqueue1;
    priority_queue<T, vector<T>, std::less<int>> pqueue1_1;
    // 使用Lambda函数或普通函数时，需在构造参数中提供具体函数(因为模板参数中相当于只声明了一个函数头)
    priority_queue<T, vector<T>, decltype(compare_lambda)> pqueue5(compare_lambda);
    priority_queue<T, vector<T>, decltype(compare_func)*> pqueue6_0(compare_func);
    priority_queue<T, vector<T>, decltype(&compare_func)> pqueue6_1(compare_func);
    priority_queue<T, vector<T>, function<bool(T, T)>> pqueue6_2(compare_func);
    priority_queue<T, vector<T>, bool(*)(T, T)> pqueue6_3(compare_func);

    // priority_queue pqueue7_0(MyCompInst);	// 不指定数据类型又不在模板参数中提供数据类型
    // priority_queue pqueue7_1(compare_func);	// 将无法编译通过
    // priority_queue pqueue7_2(compare_lambda);
}

