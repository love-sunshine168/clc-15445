#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  auto node_ = root_;  // 由于指向的是const对象，所以不能对其修改
  for (char ch: key) {
    if (node_->children_.count(ch) == 0) {return nullptr;}
    node_ = node_->children_.at(ch);
  }

  if (node_->is_value_node_ == 0) {return nullptr;}
  auto value_node_ = dynamic_cast<const TrieNodeWithValue<T>*>(node_.get());
  if (value_node_ == nullptr) {return nullptr;}
  else return value_node_->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  assert(key.size() != 0);
  // 千万不能取根和字典中的节点，因为全部都是const，只能自己创建然后才能改变。
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // 1.创建新根节点和对应新树
  auto new_root_ = std::make_shared<TrieNode>(root_->children_);
  if (new_root_ == nullptr) new_root_ = std::make_shared<TrieNode>();  // 防止根节点为空
  Trie new_tree(new_root_);
  // 2.遍历key，创建新的节点
  auto node_ = new_root_;  // 此时不能和新树的root_相等，因为树内的根节点指向const
  for (size_t i = 0; i < key.size()-1; ++i) {
    char ch = key[i];
    std::shared_ptr<TrieNode> new_node_ = nullptr;
    if (node_->children_.count(ch) == 0) {new_node_ = std::make_shared<TrieNode>();}
    else {new_node_ =std::shared_ptr<TrieNode>(node_->children_[ch]->Clone());}

    node_->children_[ch] = new_node_;
    node_ = new_node_;
  }
  // 3.此时到达key最后一个字符之前，下一个字符对应的节点一定是值节点
  std::shared_ptr<TrieNodeWithValue<T>> new_node_ = nullptr;
  if (node_->children_.count(key.back()) == 0) {new_node_ = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));}
  else {new_node_ = std::make_shared<TrieNodeWithValue<T>>(node_->children_[key.back()]->children_, std::make_shared<T>(std::move(value)));}

  node_->children_[key.back()] = new_node_;

  return new_tree;
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
