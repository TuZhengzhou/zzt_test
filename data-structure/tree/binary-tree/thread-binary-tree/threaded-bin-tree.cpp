#include <cstdio>
#include <string>
#include <vector>
#include <functional>

#include "threaded-bin-tree.h"

void TBTree::Build(const std::vector<int>& vec)
{
  // 层次遍历建立二叉树
  if (vec.empty())
  {
    root_ = nullptr;
    return;
  }

  std::vector<TBNode *> nodes(vec.size(), nullptr);
  if (placement_new_)
  {
    void *pool = malloc(sizeof(TBNode) * vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
    {
      nodes[i] = new (reinterpret_cast<char *>(pool) + i * sizeof(TBNode)) TBNode(vec[i]);
    }
  }
  else
  {
    for (size_t i = 0; i < vec.size(); ++i)
    {
      nodes[i] = new TBNode(vec[i]);
    }
  }

  for (size_t i = 0; i < vec.size(); ++i)
  {
    if (nodes[i] != nullptr)
    {
      size_t left_idx = 2 * i + 1;
      size_t right_idx = 2 * i + 2;
      if (left_idx < vec.size())
      {
        nodes[i]->left_ = nodes[left_idx];
      }
      if (right_idx < vec.size())
      {
        nodes[i]->right_ = nodes[right_idx];
      }
    }
  }
  root_ = nodes[0];

  // 中序遍历线索化
  TBNode *pre = nullptr; // 记录前驱节点
  std::function<void(TBNode *)> inorder = [&](TBNode *node) {
    if (node == nullptr)
    {
      return;
    }
    inorder(reinterpret_cast<TBNode *>(node->left_));
    // 处理当前节点
    if (node->left_ == nullptr)
    {
      node->ltag_ = true;        // 左指针是线索
      node->left_ = pre;        // 指向前驱节点
    }
    if (pre != nullptr && pre->right_ == nullptr)
    {
      pre->rtag_ = true;        // 右指针是线索
      pre->right_ = node;       // 前驱节点的右指针指向当前节点
    }
    pre = node;                 // 更新前驱节点为当前节点
    inorder(reinterpret_cast<TBNode *>(node->right_));
  };
  inorder(reinterpret_cast<TBNode *>(root_));
}

TBTree::~TBTree()
{
  if (root_ == nullptr)
  {
    return;
  }

  // 首先移动到最左节点
  TBNode *node = reinterpret_cast<TBNode *>(root_);
  while (node->ltag_ == false && node->left_ != nullptr)
  {
    node = reinterpret_cast<TBNode *>(node->left_);
  }
  while (node != nullptr)
  {
    TBNode *to_delete = const_cast<TBNode *>(node);
    node = reinterpret_cast<TBNode*>(InNext(reinterpret_cast<BNode*>(node)));
    if (placement_new_) {
      to_delete->~TBNode(); // 使用 placement new 时，需手动调用析构函数
    } else {
      delete to_delete;
    }
    to_delete = nullptr;
  }
  if (placement_new_) {
    free(pool);
  }
  root_ = nullptr;
}

BNode* TBTree::InPre(const BNode* node) const
{
  TBNode* node_ = const_cast<TBNode *>(reinterpret_cast<const TBNode *>(node));
  if (node == nullptr)
  {
    return nullptr;
  }
  // 如果左指针是线索，直接返回前驱节点
  if (node_->ltag_ == true)
  {
    return reinterpret_cast<BNode *>(node_->left_);
  }
  // 否则找左子树的最右下节点
  TBNode *pre = reinterpret_cast<TBNode *>(node_->left_);
  while (pre != nullptr && pre->rtag_ == false && pre->right_ != nullptr)
  {
    pre = reinterpret_cast<TBNode *>(pre->right_);
  }
  return pre;
}

BNode* TBTree::InNext(const BNode* node) const
{
  TBNode *next = const_cast<TBNode *>(reinterpret_cast<const TBNode *>(node));
  if (next == nullptr)
  {
    return nullptr;
  }
  // 如果右指针是线索，直接返回后继节点
  if (next->rtag_ == true)
  {
    return reinterpret_cast<TBNode *>(next->right_);
  }
  // 否则找右子树的最左下节点
  next = reinterpret_cast<TBNode *>(next->right_);
  if (next != nullptr) {
    while (next->ltag_ == false && next->left_ != nullptr)
    {
      next = reinterpret_cast<TBNode *>(next->left_);
    }
  }
  return next;
}

void TBTree::InorderTraversal(std::vector<int> &result) const
{
  result.clear();
  // printf("result.capacity: %lu\n", result.capacity());
  if (root_ == nullptr)
  {
    return;
  }
  TBNode *node = reinterpret_cast<TBNode *>(root_);
  // 找到最左下节点
  while (node->ltag_ == false && node->left_ != nullptr)
  {
    node = reinterpret_cast<TBNode *>(node->left_);
  }
  while (node != nullptr)
  {
    result.push_back(node->val_);
    // node = reinterpret_cast<TBNode*>(InNext(reinterpret_cast<BNode*>(node)));

    if (node->rtag_ == true) {
      node = reinterpret_cast<TBNode *>(node->right_);
    } else {
      TBNode* next = reinterpret_cast<TBNode *>(node->right_);
      if (next != nullptr) {
        while (!next->ltag_)
        {
          next = reinterpret_cast<TBNode *>(next->left_);
        }
      }
      node = next;
    }
  }
}

void TBTreePure::Build(const std::vector<int> &vec) {
  // 层次遍历建立二叉树
  if (vec.empty())
  {
    root_ = nullptr;
    return;
  }
  std::vector<TBNodePure *> nodes(vec.size(), nullptr);
  for (size_t i = 0; i < vec.size(); ++i)
  {
    nodes[i] = new TBNodePure(vec[i]);
  }
  for (size_t i = 0; i < vec.size(); ++i)
  {
    if (nodes[i] != nullptr)
    {
      size_t left_idx = 2 * i + 1;
      size_t right_idx = 2 * i + 2;
      if (left_idx < vec.size())
      {
        nodes[i]->left_ = nodes[left_idx];
      }
      if (right_idx < vec.size())
      {
        nodes[i]->right_ = nodes[right_idx];
      }
    }
  }
  root_ = nodes[0];

  // 中序遍历线索化
  TBNodePure *pre = nullptr; // 记录前驱节点
  std::function<void(TBNodePure *)> inorder = [&](TBNodePure *node) {
    if (node == nullptr)
    {
      return;
    }
    inorder(node->left_);
    // 处理当前节点
    if (node->left_ == nullptr)
    {
      node->ltag_ = true;        // 左指针是线索
      node->left_ = pre;        // 指向前驱节点
    }
    if (pre != nullptr && pre->right_ == nullptr)
    {
      pre->rtag_ = true;        // 右指针是线索
      pre->right_ = node;       // 前驱节点的右指针指向当前节点
    }
    pre = node;                 // 更新前驱节点为当前节点
    inorder(node->right_);
  };
  inorder(root_);
}

TBTreePure::~TBTreePure() {
  if (root_ == nullptr)
  {
    return;
  }
  TBNodePure *node = root_;
  // 找到最左下节点
  while (node->ltag_ == false && node->left_ != nullptr)
  {
    node = node->left_;
  }
  while (node != nullptr)
  {
    TBNodePure *to_delete = const_cast<TBNodePure *>(node);
    node = InNext(node);
    delete to_delete;
  }
  root_ = nullptr;
}

TBNodePure* TBTreePure::InPre(const TBNodePure* node) const {
  TBNodePure* node_ = const_cast<TBNodePure *>(node);
  if (node == nullptr)
  {
    return nullptr;
  }
  // 如果左指针是线索，直接返回前驱节点
  if (node_->ltag_ == true)
  {
    return node_->left_;
  }
  // 否则找左子树的最右下节点
  TBNodePure *pre = node_->left_;
  while (pre != nullptr && pre->rtag_ == false && pre->right_ != nullptr)
  {
    pre = pre->right_;
  }
  return pre;
}

TBNodePure* TBTreePure::InNext(const TBNodePure* node) const
{
  TBNodePure *next = const_cast<TBNodePure *>(node);
  if (next == nullptr)
  {
    return nullptr;
  }
  // 如果右指针是线索，直接返回后继节点
  if (next->rtag_ == true)
  {
    return next->right_;
  }
  // 否则找右子树的最左下节点
  next = next->right_;
  while (next != nullptr && next->ltag_ == false && next->left_ != nullptr)
  {
    next = next->left_;
  }
  return next;
}

void TBTreePure::InorderTraversal(std::vector<int> &result) const
{
  result.clear();
  if (root_ == nullptr)
  {
    return;
  }
  TBNodePure *node = root_;
  // 找到最左下节点
  while (node->ltag_ == false && node->left_ != nullptr)
  {
    node = node->left_;
  }
  while (node != nullptr)
  {
    result.push_back(node->val_);
    // node = InNext(node);
    if (node->rtag_ == true) {
      node = node->right_;
    } else {
      TBNodePure* next = node->right_;
      if (next != nullptr) {
        while (!next->ltag_)
        {
          next = next->left_;
        }
      }
      node = next;
    }
  }
}