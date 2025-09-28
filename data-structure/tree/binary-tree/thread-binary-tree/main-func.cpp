#include <iostream>
#include <chrono>
#include <cassert>

#include "tools.h"
#include "threaded-bin-tree.h"

struct Timer
{
  Timer(const char *msg)
  {
    msg_ = std::string(msg);
    start_ = std::chrono::steady_clock::now().time_since_epoch().count();
  }
  ~Timer()
  {
    long long end_ = std::chrono::steady_clock::now().time_since_epoch().count();
    std::cout << msg_ << " duration is " << (end_ - start_) / 1000 << "us\n";
  }
  std::string msg_;
  long long start_;
};

int main(int argc, char **argv)
{
  // ./main-func --size=[size] --repeat=[repeat], default true, 1000000, 1

  int size = 1000000;
  int repeat = 10;
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg.find("--size=") == 0)
    {
      std::string value = arg.substr(strlen("--size="));
      size = std::stoi(value);
    }
    else if (arg.find("--repeat=") == 0)
    {
      std::string value = arg.substr(strlen("--repeat="));
      repeat = std::stoi(value);
    }
    else {
      printf("Usage: %s\n\n", "./main-func");
      printf("Compile: \n");
      printf("g++ -g -std=c++11 -o main-func main-func.cpp threaded-bin-tree.cpp ../bin-tree.cpp\n\n");
      printf("Common commands:\n");
      printf("1. ./main-func\n");
      return 0;
    }
  }
  // 另起一个进程使用 perf
  printf("sizeof(BNode): %lu, sizeof(TBNode): %lu, sizeof(TBNodePure): %lu\n", sizeof(BNode), sizeof(TBNode), sizeof(TBNodePure));

  {
    
    std::vector<int> input_vec {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> output_vec {8, 4, 9, 2, 5, 1, 6, 3, 7};
    std::vector<int> tmp;

    bool placement_new = false;
    TBTree tree(placement_new);
    tree.Build(input_vec);
    tree.InorderTraversal(tmp);
    for (int i = 0; i < tmp.size(); i++) {
      assert(tmp[i] == output_vec[i]);
    }
    tree.~TBTree();

    tmp.clear();
    placement_new = true;
    TBTree tree_2(placement_new);
    tree_2.Build(input_vec);
    tree_2.InorderTraversal(tmp);
    for (int i = 0; i < tmp.size(); i++) {
      assert(tmp[i] == output_vec[i]);
    }
    tree_2.~TBTree();

    tmp.clear();
    placement_new = true;
    BTree tree_3(placement_new);
    tree_3.Build(input_vec);
    tree_3.InorderTraversal(tmp);
    for (int i = 0; i < tmp.size(); i++) {
      assert(tmp[i] == output_vec[i]);
    }
    tree_3.~BTree();
  }
  printf("\033[32mfunc test pass\033[0m\n");
}