#include <cstdio>
#include <stdlib.h>
#include <functional>
#include <unistd.h>

#include "bin-tree.h"

/*
    将输入 vec 看做树的层序遍历建树
*/
void BTree::Build(const std::vector<int>& vec)
{
  if (vec.empty())
  {
    root_ = nullptr;
    return;
  }
  std::vector<BNode *> nodes(vec.size(), nullptr);
  if (placement_new_)
  {
    void *pool = malloc(sizeof(BNode) * vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
    {
      nodes[i] = new (reinterpret_cast<char *>(pool) + i * sizeof(BNode)) BNode(vec[i]);
    }
  }
  else
  {
    for (size_t i = 0; i < vec.size(); ++i)
    {
      nodes[i] = new BNode(vec[i]);
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
}

void BTree::PreorderTraversal(std::vector<int> &result) const
{
  std::function<void(BNode *)> preorder = [&](BNode *node)
  {
    if (node == nullptr)
    {
      return;
    }
    result.push_back(node->val_);
    preorder(node->left_);
    preorder(node->right_);
  };
  preorder(root_);
}

void BTree::InorderTraversal(std::vector<int> &result) const
{
  // std::function<void(BNode*)> inorder = [&](BNode* node) {
  //   if (node == nullptr) {
  //     return;
  //   }
  //   inorder(node->left_);
  //   result.push_back(node->val_);
  //   inorder(node->right_);
  // };
  // inorder(root_);
  // 非递归中序遍历
  result.clear();
  if (root_ == nullptr)
  {
    return;
  }
  std::vector<BNode *> stack;
  BNode *current = root_;
  while (current != nullptr || !stack.empty())
  {
    while (current != nullptr)
    {
      stack.push_back(reinterpret_cast<BNode *>(current));
      current = reinterpret_cast<BNode *>(current->left_);
    }
    if (!stack.empty())
    {
      current = reinterpret_cast<BNode *>(stack.back());
      stack.pop_back();
      result.push_back(current->val_);
      current = reinterpret_cast<BNode *>(current->right_);
    }
  }
}

BNode* BTree::InPre(const BNode* node) const {
  if (node == nullptr) {
    return nullptr;
  }
  // 左子树不为空，前驱节点为左子树的最右下节点
  if (node->left_ != nullptr) {
    BNode* pre = node->left_;
    while (pre->right_ != nullptr) {
      pre = pre->right_;
    }
    return pre;
  }
  // 左子树为空，前驱节点为第一个左链接的祖先节点
  BNode* current = root_;
  BNode* pre = nullptr;
  while (current != nullptr) {
    if (node->val_ < current->val_) {
      current = current->left_;
    } else if (node->val_ > current->val_) {
      pre = current; // 记录可能的前驱节点
      current = current->right_;
    } else {
      break; // 找到节点
    }
  }
  return pre;
}

BNode* BTree::InNext(const BNode* node) const {
  if (node == nullptr) {
    return nullptr;
  }
  // 右子树不为空，后继节点为右子树的最左下节点
  if (node->right_ != nullptr) {
    BNode* next = node->right_;
    while (next->left_ != nullptr) {
      next = next->left_;
    }
    return next;
  }
  // 右子树为空，后继节点为第一个右链接的祖先节点
  BNode* current = root_;
  BNode* next = nullptr;
  while (current != nullptr) {
    if (node->val_ < current->val_) {
      next = current; // 记录可能的后继节点
      current = current->left_;
    } else if (node->val_ > current->val_) {
      current = current->right_;
    } else {
      break; // 找到节点
    }
  }
  return next;
}

BTree::~BTree()
{
  if (root_ == nullptr)
  {
    return;
  }
  std::function<void(BNode *)> postorder = [&](BNode *node)
  {
    if (node == nullptr)
    {
      return;
    }
    postorder(node->left_);
    postorder(node->right_);
    if (placement_new_) {
      node->~BNode();
    } else {
      delete node;
    }
    node = nullptr;
  };
  postorder(root_);
  root_ = nullptr;


}