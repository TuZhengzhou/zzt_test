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
  // ./main --size=[size] --repeat=[repeat], default true, 1000000, 1

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
      printf("Usage: %s\n\n", "./main [--size=size] [--repeat=repeat], default 1000000, 10");
      printf("Compile: \n");
      printf("g++ -g -O2 -std=c++11 -o main main.cpp threaded-bin-tree.cpp ../bin-tree.cpp\n\n");
      printf("Common commands:\n");
      printf("1. ./main --size=1000000 --repeat=1000\n");
      printf("2. ./main --size=1000000 --repeat=1000\n");
      return 0;
    }
  }
  // 另起一个进程使用 perf
  printf("sizeof(BNode): %lu, sizeof(TBNode): %lu, sizeof(TBNodePure): %lu\n", sizeof(BNode), sizeof(TBNode), sizeof(TBNodePure));


  {
    std::string tag = "TBTree placement_new = true";
    bool placement_new = true;

    BTree *tree = reinterpret_cast<BTree *>(new TBTree(placement_new));
    tree->Build(std::vector<int>(size, 0));

    Timer t(tag.c_str());
    for (int i = 0; i < repeat; i++)
    {
      std::vector<int> result;
      result.reserve(size);
      tree->InorderTraversal(result);
    }
    // t.~Timer();
  }
  // int current_pid = GetCurrentPid();
  // float memory_usage = GetMemoryUsage(current_pid);
  // std::cout << "memory_usage: " << memory_usage << " MB" << std::endl;

  {
    std::string tag = "TBTree placement_new = false";
    bool placement_new = false;

    BTree *tree = reinterpret_cast<BTree *>(new TBTree(placement_new));
    tree->Build(std::vector<int>(size, 0));

    Timer t(tag.c_str());
    for (int i = 0; i < repeat; i++)
    {
      std::vector<int> result;
      result.reserve(size);
      tree->InorderTraversal(result);
    }
    // t.~Timer();
  }

  {
    std::string tag = " BTree placement_new = true";

    BTree *tree = new BTree();
    tree->Build(std::vector<int>(size, 0));

    Timer t(tag.c_str());
    for (int i = 0; i < repeat; i++)
    {
      std::vector<int> result;
      result.reserve(size);
      tree->InorderTraversal(result);
    }
    // t.~Timer();
  }

  {
    std::string tag = " BTree placement_new = fasle";

    BTree *tree = new BTree();
    tree->Build(std::vector<int>(size, 0));

    Timer t(tag.c_str());
    for (int i = 0; i < repeat; i++)
    {
      std::vector<int> result;
      result.reserve(size);
      tree->InorderTraversal(result);
    }
    // t.~Timer();
  }

  return 0;
}