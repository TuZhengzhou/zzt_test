## C++：who can sort? who can be sorted?

`std::vector<int>` 可以调用 `std::sort` 排序, 其他的呢？

### who can sort

`std::sort`函数模板定义如下：

```cpp
template< class RandomIt, class Compare >
void sort( RandomIt first, RandomIt last, Compare comp );
```

其中 `RandomIt` 是随机访问迭代器类型（Random Access Iterator）。C++标准库中, 支持随机访问迭代器的容器有：

1. `std::vector`
2. `std::array`
3. `std::deque`
5. `std::basic_string`

如果不支持随机迭代访问器, 例如`std::list`, 则无法调用`std::sort`, 但可以调用`std::list::sort`成员函数。有 `sort` 成员函数的容器有：

1. `std::list`
2. `std::forward_list`

此外，有序关联式容器（`map`, `set`, `multimap`, `multiset`）虽然没有 `sort` 成员函数, 但其内部元素本身是有序的。

### who can be sorted

不仅是简单的 `int` 类型可以比较、排序, 自定义类型也可以。只要重载了比较运算符 `<`, 或者提供了自定义的比较函数对象（仿函数）。

C++ STL 中支持按字典序的比较和排序容器有：

1. **序列式容器**：内部结构稳定, **需要容器元素可比较**, `vector`, `deque`, `array`, `list`, `forward_list`, `basic_string`, 其中`list`, `forward_list`不能使用std::sort, 需要调用其成员函数`list::sort`, `forward_list::sort`
2. **有序关联式容器**：内部本身有序, **键元素由比较器有序**, `map`, `set`, `multimap`, `multiset`。`map`,`multimap` 还需要**值元素可比较**
3. **容器适配器**：`stack`, `queue`, 其底层实现是序列式容器, **需要底层容器元素可比较**, `priority_queue`除外（见下文）
4. **通用工具**：`pair`, `tuple`，`optionan`, `variant`, **需要所有成员元素可比较**

C++ STL 中不支持按字典序的比较和排序容器有：

1. **无序关联式容器**：C++ STL 提供的**无序关联式容器/哈希容器**内部元素排列不稳定, 不同实现、不同负载因子甚至相同程序的不同运行中, 迭代顺序可能不同, 因此不支持按字典序的比较和排序, 只支持等于/不等于。如`unordered_map`, `unordered_set`, `unordered_multimap`, `unordered_multiset`
2. **容器适配器**：特殊的, `priority_queue`也不支持比较。虽然其底层实现是序列式容器, 但其内部元素排列不稳定
3. **通用工具**：`expected`, `bitset`, `any`