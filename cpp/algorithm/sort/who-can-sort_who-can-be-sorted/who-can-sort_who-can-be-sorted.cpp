#include <algorithm>
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <deque>
#include <queue>

using namespace std;

int main()
{

  {
    string tag = "int can be sorted. using sort() for vector<int>";
    printf("%s\n", tag.c_str());

    vector<int> vec{3, 1, 2, 4, 5};
    printf("before: ");
    for (int i = 0; i < vec.size(); i++)
    {
      cout << "[" << vec[i] << "], ";
    }
    cout << endl;

    sort(vec.begin(), vec.end());

    printf("after : ");
    for (int i = 0; i < vec.size(); i++)
    {
      cout << "[" << vec[i] << "], ";
    }
    cout << endl << endl;
  }

  {
    string tag = "int can be sorted. using sort() for deque<int>";
    printf("%s\n", tag.c_str());

    deque<int> deq{3, 1, 2, 4, 5};
    printf("before: ");
    for (int i = 0; i < deq.size(); i++)
    {
      cout << "[" << deq[i] << "], ";
    }
    cout << endl;

    sort(deq.begin(), deq.end());

    printf("after : ");
    for (int i = 0; i < deq.size(); i++)
    {
      cout << "[" << deq[i] << "], ";
    }
    cout << endl << endl;
  }

  {
    string tag = "char can be sorted. using sort() for basic_string<char>";
    printf("%s\n", tag.c_str());

    string s = "hello world";
    cout << "before: " << s << endl;

    sort(s.begin(), s.end());

    cout << "after : " << s << endl << endl;
  }

  {
    string tag = "vector can be sorted. using sort() for vector<vector<int>>";
    printf("%s\n", tag.c_str());

    vector<vector<int>> vecs{{1, 2}, {1}, {2}, {}};
    printf("before: ");
    for (int i = 0; i < vecs.size(); i++)
    {
      cout << "[";
      for_each(vecs[i].begin(), vecs[i].end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl;

    sort(vecs.begin(), vecs.end());

    printf("after : ");
    for (int i = 0; i < vecs.size(); i++)
    {
      cout << "[";
      for_each(vecs[i].begin(), vecs[i].end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl << endl;
  }

  {
    string tag = "list can be sorted. using list.sort() for list<list<int>>";
    printf("%s\n", tag.c_str());

    list<list<int>> lists{{1, 2}, {1}, {2}, {}};

    printf("before: ");
    for (auto &l : lists)
    {
      cout << "[";
      for_each(l.begin(), l.end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl;

    // sort(lists.begin(), lists.end());  // list is not random accessable
    lists.sort(); // list support sort itself

    printf("after : ");
    for (auto &l : lists)
    {
      cout << "[";
      for_each(l.begin(), l.end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl << endl;
  }

  {
    string tag = "stack can be sorted. using sort() for vector<stack<int>>";
    printf("%s\n", tag.c_str());

    stack<int> s;
    vector<stack<int>> vec_stack;
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
      vector<int> tmp;
      while (!l.empty())
      {
        tmp.insert(tmp.begin(), l.top());
        l.pop();
      }
      cout << "[";
      for_each(tmp.begin(), tmp.end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl;
  
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
    sort(vec_stack.begin(), vec_stack.end());

    printf("after : ");
    for (auto &l : vec_stack)
    {
      // 栈输出特殊处理，默认后进先出，需调整为先进先出
      vector<int> tmp;
      while (!l.empty())
      {
        tmp.insert(tmp.begin(), l.top());
        l.pop();
      }
      cout << "[";
      for_each(tmp.begin(), tmp.end(), [](int v)
               { cout << v << ", "; });
      cout << "], ";
    }
    cout << endl << endl;
  }

  return 0;
}