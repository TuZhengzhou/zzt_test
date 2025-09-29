#include <algorithm>
#include <iostream>
#include <list>
#include <stack>
#include <vector>

using namespace std;

int main()
{

  {
    string tag = "int can be sorted. using std::sort() for vector<int>";
    printf("%s\n", tag.c_str());

    std::vector<int> vec{3, 1, 2, 4, 5};
    printf("before: ");
    for (int i = 0; i < vec.size(); i++)
    {
      std::cout << "[" << vec[i] << "], ";
    }
    std::cout << std::endl << std::endl;

    std::sort(vec.begin(), vec.end());

    printf("after : ");
    for (int i = 0; i < vec.size(); i++)
    {
      std::cout << "[" << vec[i] << "], ";
    }
    std::cout << std::endl << std::endl;
  }

  {
    string tag = "vector can be sorted. using std::sort() for vector<vector<int>>";
    printf("%s\n", tag.c_str());

    std::vector<std::vector<int>> vecs{{1, 2}, {1}, {2}, {}};
    printf("before: ");
    for (int i = 0; i < vecs.size(); i++)
    {
      std::cout << "[";
      for_each(vecs[i].begin(), vecs[i].end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;

    std::sort(vecs.begin(), vecs.end());

    printf("after : ");
    for (int i = 0; i < vecs.size(); i++)
    {
      std::cout << "[";
      for_each(vecs[i].begin(), vecs[i].end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;
  }

  {
    string tag = "list can be sorted. using list.sort() for list<list<int>>";
    printf("%s\n", tag.c_str());

    std::list<std::list<int>> lists{{1, 2}, {1}, {2}, {}};

    printf("before: ");
    for (auto &l : lists)
    {
      std::cout << "[";
      for_each(l.begin(), l.end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;

    // std::sort(lists.begin(), lists.end());  // list is not random accessable
    lists.sort(); // list support sort itself

    printf("after : ");
    for (auto &l : lists)
    {
      std::cout << "[";
      for_each(l.begin(), l.end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;
  }

  {
    string tag = "stack can be sorted. using std::sort() for vector<stack<int>>";
    printf("%s\n", tag.c_str());

    std::stack<int> s;
    std::vector<std::stack<int>> vec_stack;
    s.push(2);
    vec_stack.push_back(s); // {2}
    s.pop();
    s.push(1);
    vec_stack.push_back(s); // {1}
    s.push(2);
    vec_stack.push_back(s); // {1,2}
    s.pop();
    s.pop();
    vec_stack.push_back(s); // {}

    printf("before: ");
    for (auto &l : vec_stack)
    {
      // 栈输出特殊处理，默认后进先出，需调整为先进先出
      std::vector<int> tmp;
      while (!l.empty())
      {
        tmp.insert(tmp.begin(), l.top());
        l.pop();
      }
      std::cout << "[";
      for_each(tmp.begin(), tmp.end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;
  
    vec_stack.clear();
    s.push(2);
    vec_stack.push_back(s); // {2}
    s.pop();
    s.push(1);
    vec_stack.push_back(s); // {1}
    s.push(2);
    vec_stack.push_back(s); // {1,2}
    s.pop();
    s.pop();
    vec_stack.push_back(s); // {}
    std::sort(vec_stack.begin(), vec_stack.end());

    printf("after : ");
    for (auto &l : vec_stack)
    {
      // 栈输出特殊处理，默认后进先出，需调整为先进先出
      std::vector<int> tmp;
      while (!l.empty())
      {
        tmp.insert(tmp.begin(), l.top());
        l.pop();
      }
      std::cout << "[";
      for_each(tmp.begin(), tmp.end(), [](int v)
               { std::cout << v << ", "; });
      std::cout << "], ";
    }
    std::cout << std::endl << std::endl;
  }

  return 0;
}