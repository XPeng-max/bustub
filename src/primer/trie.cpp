#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if (nullptr == root_) {
    return nullptr;
  }
  auto node = root_;
  for (auto ch : key) {
    auto iter = node->children_.find(ch);
    if (iter == node->children_.end()) {
      return nullptr;
    }
    node = iter->second;
  }
  auto ptr = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(node);
  if (nullptr == ptr) {
    return nullptr;
  }
  return ptr->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when crating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  size_t n = key.size();
  if (0 == n) {
    if (nullptr == root_) {
      return Trie{std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)))};
    }
    return Trie{std::make_shared<TrieNodeWithValue<T>>(root_->children_,std::make_shared<T>(std::move(value)))};
  }

  auto root = (root_ == nullptr) ? std::make_shared<TrieNode>() : std::shared_ptr<TrieNode>(root_->Clone());
  auto node = root;

  for (size_t i = 0; i < n - 1; i++) {
    char ch = key[i];
    auto& children = node->children_;
    auto iter = children.find(ch);
    auto nnode = (iter == children.end()) ? std::make_shared<TrieNode>() : std::shared_ptr<TrieNode>(iter->second->Clone());

    children[ch] = nnode;
    node = nnode;
  }
  auto& children = node->children_;
  if (children.find(key[n - 1]) != node->children_.end()) {
    children[key[n - 1]] = std::make_shared<TrieNodeWithValue<T>>(children[key[n - 1]]->children_,std::make_shared<T>(std::move(value)));
  }
  else {
    children[key[n - 1]] = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
  }
  return Trie{root};
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  if (nullptr == root_) {
    return Trie{nullptr};
  }
  size_t n = key.size();
  if (0 == n) {
    if (root_->is_value_node_) {
      if (root_->children_.empty()) {
        return Trie{nullptr};
      }
      return Trie{std::make_shared<TrieNode>(root_->children_)};
    }
    return Trie{root_->Clone()};
  }
  std::function<std::shared_ptr<TrieNode>(std::shared_ptr<TrieNode>, size_t)> remove_node = [&](std::shared_ptr<TrieNode> node, size_t index) {
    char ch = key[index];
    if (node->children_.find(ch) == node->children_.end() || (index == n - 1 && !node->children_[ch]->is_value_node_)) {
      return node;
    }
    auto& child = node->children_[ch];
    std::shared_ptr<TrieNode> new_child;

    if (index == n - 1) {
      if (!child->children_.empty()) {
        new_child = std::make_shared<TrieNode>(child->children_);
      }
    }
    else {
      new_child = remove_node(child->Clone(), index + 1);
    }

    if (new_child) {
      node->children_[ch] = new_child;
    }
    else {
      node->children_.erase(ch);
    }

    if (node->children_.empty() && !node->is_value_node_) {
      node = nullptr;
    }
    return node;
  };
  auto root = remove_node(root_->Clone(), 0);
  return Trie {root};
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
