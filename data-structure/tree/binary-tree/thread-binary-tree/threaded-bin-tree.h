#include <vector>

#include "/root/codes/zzt_test/data-structure/tree/binary-tree/bin-tree.h"

struct TBNode : BNode
{
  // bool ltag_; // true if left pointer is a thread
  // bool rtag_; // true if right pointer is a thread
  TBNode() : BNode() {};
  TBNode(int val) : BNode(val) {};
};

class TBTree : BTree
{
public:
  TBTree(bool placement_new = false) : BTree(placement_new) {};
  virtual void Build(const std::vector<int> &vec) override;
  virtual ~TBTree();
  virtual BNode* InPre(const BNode* node) const override;  // 获取中序遍历的前驱节点
  virtual BNode *InNext(const BNode *node) const override; // 获取中序遍历的后继节点
  virtual void InorderTraversal(std::vector<int> &result) const override;
  const TBNode *GetRoot() const { return reinterpret_cast<TBNode *>(root_); };

 private:
};

struct TBNodePure
{
  int val_;
  bool ltag_; // true if left pointer is a thread
  bool rtag_; // true if right pointer is a thread
  TBNodePure* left_;
  TBNodePure* right_;
  TBNodePure() : val_(0), left_(nullptr), right_(nullptr), ltag_(false), rtag_(false) {};
  TBNodePure(int val) : val_(val), left_(nullptr), right_(nullptr), ltag_(false), rtag_(false) {};
};

class TBTreePure {
 public:
  TBTreePure() { root_ = nullptr; };
  void Build(const std::vector<int> &vec);
  ~TBTreePure();
  TBNodePure* InPre(const TBNodePure* node) const;  // 获取中序遍历的前驱节点
  TBNodePure *InNext(const TBNodePure *node) const; // 获取中序遍历的后继节点
  void InorderTraversal(std::vector<int> &result) const;
  const TBNodePure *GetRoot() const { return root_; };
 private:
  TBNodePure* root_;
};