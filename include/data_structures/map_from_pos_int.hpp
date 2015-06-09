#ifndef _DATA_STRUCTURES_MAP_FROM_POS_INT_HPP
#define _DATA_STRUCTURES_MAP_FROM_POS_INT_HPP

#include "data_structures/dynamic_array.hpp"
#include "data_structures/internal/table_utils.hpp"

namespace parkway {
namespace data_structures {

/* New MapFromPosInt Class */
/* requires positive int keys */
template<typename T> class map_from_pos_int {
 protected:
  int size_;
  int capacity_;

  dynamic_array<T> table_;
  dynamic_array<int> keys_;

 public:
  map_from_pos_int() : map_from_pos_int(0) {
  }

  map_from_pos_int(int _size) {
    create(_size);
  }

  ~map_from_pos_int() {
  }

  void create(int size) {
    size_ = 0;
    capacity_ = internal::table_utils::table_size(size);

    #ifdef DEBUG_TABLES
    assert(size >= _size);
    #endif

    keys_.reserve(capacity_);
    table_.reserve(capacity_);

    for (std::size_t i = 0; i < capacity_; ++i) {
      keys_[i] = -1;
    }
  }

  void destroy() {
    size_ = 0;
    table_.reserve(0);
    keys_.reserve(0);
  }

  void recover() {
    table_.reserve(capacity_);
    keys_.reserve(capacity_);
    size_ = 0;

    for (std::size_t i = 0; i < capacity_; ++i) {
      keys_[i] = -1;
    }
  }

  bool insert(int key, T value) {
    int indepKey = internal::table_utils::scatter_key(key);
    int slot = internal::hashes::primary(indepKey, capacity_);

    while (keys_[slot] != -1 && keys_[slot] != indepKey)
      slot = (slot + internal::hashes::secondary(indepKey, capacity_)) %
             capacity_;

    if (keys_[slot] == -1) {
      table_[slot] = value;
      keys_[slot] = indepKey;
      ++size_;
      return false;
    } else {
      table_[slot] = value;
      return true;
    }
  }


  T &get(int key) {
    int indepKey = internal::table_utils::scatter_key(key);
    int slot = internal::hashes::primary(indepKey, capacity_);
    while (keys_[slot] != indepKey) {
      slot = (slot + internal::hashes::secondary(indepKey, capacity_)) %
             capacity_;
      #ifdef DEBUG_TABLES
      assert(keys[slot] != -1);
      #endif
    }

    return table_[slot];
  }

  inline int size() {
    return size_;
  }

  inline int capacity() {
    return capacity_;
  }
};

}  // namespace data_structures
}  // namespace parkway

#endif  // _DATA_STRUCTURES_MAP_FROM_POS_INT_HPP