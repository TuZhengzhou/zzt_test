## 线索二叉树

**线索二叉树的作用总结如下**：可以用于方便地求取中序前驱和中序后继。在没有左/右孩子的情况下，线索二叉树直接存储
了中序前驱/中序后继，从而无需遍历整个树，有效将尾查询的开销从`O(n)`降低到`O(1)`。对于一般情况，其中序前驱/后继
的查找代价仍为`O(log n)`（与普通二叉树代价相同）

 0. main-func.cpp       TBTree和BTree正确性验证
 1. main.cpp            TBTree和BTree中序遍历时间对比
 2. main-memusange.cpp  内存使用


线索二叉树，是一种特殊的二叉树。与一般的二叉树相比，增添了两个域，用于标识左孩子/右孩子是线索（此时左孩子表示
前驱线索，右孩子表示后继线索）还是普通的孩子节点。

线索二叉树可以由普通的二叉树构建而来：对普通二叉树进行中序遍历，遍历时保存上一个节点（初始时为nullptr）。
在当前节点左孩子为空时，让左孩子指向上一个节点（同时进行标记）；当上一个节点非空、且其右孩子为空时，
可以让其右孩子指向当前节点（同时进行标记）。从而完成线索二叉树的构建

构建好的线索二叉树，其最左节点的前驱是 nullptr（有标记）；其最右节点没有后继（没有标记），仅有为 nullptr 的右孩子。

给定任一节点，线索二叉树支持快速的中序前驱查找和中序后继查找，而无需从根节点开始遍历。具体来说，前驱查找为：

 1. 如果左孩子被标记，则左孩子为前驱
 2. 否则，其前驱为左子树的最右节点

后继查找为：

 1. 如果右孩子被标记，则右孩子为后继
 2. 否则，其后继为右子树的最左节点

线索二叉树支持正序的中序遍历和倒序的中序遍历，但其时间与迭代版的普通中序遍历几乎相同。且遍历时会频繁查找前驱和后继，
其局部性并不如迭代版的普通中序遍历，L1/L2 Cache命中率在树较大时并不好。尽管在构建树时让节点分布在同一块内存上
（通过 malloc 大块 + placement new的方式）可以略微提高Cache命中率，其效率仍不如迭代版的普通中序遍历（后者使用一个
vector模拟栈，后继节点都在vector里，局部性较好）。

```bash
taskset 1 ./main --size=10000000 --repeat=100
sizeof(BNode): 24, sizeof(TBNode): 24, sizeof(TBNodePure): 24
TBTree placement_new = true duration is 4090719us
TBTree placement_new = false duration is 4541153us
 BTree  placement_new = true duration is 3737946us
 BTree  placement_new = fasle duration is 3760309us     
```
                 

可以通过在代码中读 /proc/\[pid\]/status 的方式获取程序执行时的状态，包括CPU、内存等。`main-memusage.cpp`对比了线索/普通
二叉树中序遍历的最大内存使用量。从测试结果看，前者并不具内存优势。

```bash
➜  thread-binary-tree git:(main) ✗ ./main-memusage --tree-type=tbtree --placement-new=true 
sizeof(BNode): 24, sizeof(TBNode): 24, sizeof(TBNodePure): 24
TBTree placement_new = true duration is 392902us
name: VmHWM:, VmRSS: 354944 KB
memory_usage: 346.625 MB
➜  thread-binary-tree git:(main) ✗ ./main-memusage --tree-type=tbtree --placement-new=false
sizeof(BNode): 24, sizeof(TBNode): 24, sizeof(TBNodePure): 24
TBTree placement_new = true duration is 450831us
name: VmHWM:, VmRSS: 433152 KB
memory_usage: 423 MB
➜  thread-binary-tree git:(main) ✗ ./main-memusage --tree-type=btree --placement-new=false 
sizeof(BNode): 24, sizeof(TBNode): 24, sizeof(TBNodePure): 24
TBTree placement_new = true duration is 351607us
name: VmHWM:, VmRSS: 433024 KB
memory_usage: 422.875 MB
➜  thread-binary-tree git:(main) ✗ ./main-memusage --tree-type=btree --placement-new=true 
sizeof(BNode): 24, sizeof(TBNode): 24, sizeof(TBNodePure): 24
TBTree placement_new = true duration is 334443us
name: VmHWM:, VmRSS: 354816 KB
memory_usage: 346.5 MB
```
