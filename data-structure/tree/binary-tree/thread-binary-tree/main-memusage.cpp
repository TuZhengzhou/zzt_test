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
  std::string tree_type = "tbtree";
  bool placement_new = true;
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg.find("--tree-type=") == 0)
    {
      tree_type = arg.substr(strlen("--tree-type="));
    }
    else if (arg.find("--placement-new=") == 0)
    {
      std::string val = arg.substr(strlen("--placement-new="));
      placement_new = val == "true" ? true: false;
    }
    else {
      printf("Usage: %s\n\n", "./main-memusage [--tree-type=tbtree] [--placement-new=true]");
      printf("Compile: \n");
      printf("g++ -g -O2 -std=c++11 -o main-memusage main-memusage.cpp threaded-bin-tree.cpp ../bin-tree.cpp\n\n");
      printf("Common commands:\n");
      printf("1. ./main-memusage --tree-type=tbtree --placement-new=true\n");
      printf("2. ./main-memusage --tree-type=btree --placement-new=true\n");
      return 0;
    }
  }

  printf("sizeof(BNode): %lu, sizeof(TBNode): %lu, sizeof(TBNodePure): %lu\n", sizeof(BNode), sizeof(TBNode), sizeof(TBNodePure));

  BTree* tree;
  if (tree_type == "tbtree") {
    tree = reinterpret_cast<BTree*>(new TBTree(placement_new));
  } else {
    tree = new BTree(placement_new);
  }

  int size = 10000000;
  int repeat = 10;

  {
    std::string tag = "TBTree placement_new = true";
    tree->Build(std::vector<int>(size, 0));

    Timer t(tag.c_str());
    for (int i = 0; i < repeat; i++)
    {
      std::vector<int> result;
      result.reserve(size);
      tree->InorderTraversal(result);
    }
  }
  int current_pid = GetCurrentPid();
  float memory_usage = GetMemoryUsage(current_pid);
  std::cout << "memory_usage: " << memory_usage << " MB" << std::endl;

  return 0;
}