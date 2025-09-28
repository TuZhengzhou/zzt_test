#include <vector>

struct BNode {
  bool ltag_;
  bool rtag_;
  int val_;
  BNode* left_;
  BNode* right_;
  BNode(): val_(0), left_(nullptr), right_(nullptr), ltag_(false), rtag_(false) {};
  BNode(int val): val_(val), left_(nullptr), right_(nullptr), ltag_(false), rtag_(false) {};
};

class BTree {
public:
  BTree(bool placement_new = false): root_(nullptr), placement_new_(placement_new) {};
  virtual ~BTree();

  virtual void Build(const std::vector<int>& vec);
  virtual BNode* InPre(const BNode* node) const;
  virtual BNode* InNext(const BNode* node) const;
  virtual void PreorderTraversal(std::vector<int>& result) const;
  virtual void InorderTraversal(std::vector<int>& result) const;
  const BNode* GetRoot() const { return root_; };
protected:
  BNode* root_;
  bool placement_new_ = false;
  void* pool = nullptr;
};
