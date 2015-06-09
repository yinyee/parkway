#ifndef _BITFIELD_HPP
#define _BITFIELD_HPP

// ### bit_field.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
// Modified from code by William Knottenbelt
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// NOTES:
//
//  - Assume that sizeof(unsigned int) == 4 bytes
//
// ###

// TODO(gb610): remove reliance on assumption

#include <iostream>
#include "data_structures/dynamic_array.hpp"

namespace parkway {
namespace data_structures {

class bit_field {
 public:
  bit_field() {
  }

  bit_field(int bits) {
    reserve(bits);
  }

  ~bit_field() {
  }

  inline void check(int bit) {
    int old = data_.capacity();
    int bits = bit + 1;
    int target = (bits >> 5) + ((bits & 31) ? 1 : 0);

    data_.check(target);
    capacity_ = data_.capacity();

    if (capacity_ > old) {
      for (int n = old; n < capacity_; ++n)
        data_[n] = 0;
    }
    bit_length_ = (capacity_ << 5);
  }

  inline int number_of_bits() const {
    return bit_length_;
  }

  inline int capacity() const {
    return capacity_;
  }

  inline unsigned int *data() const {
    return data_.data();
  }

  inline unsigned int chunk(int i) const {
    return data_[i];
  }

  inline void reserve(int bits) {
    bit_length_ = bits;
    capacity_ = (bits >> 5) + ((bits & 31) ? 1 : 0);
    data_.reserve(capacity_);
  }

  inline void unset() {
    for (int l = 0; l < capacity_; ++l) {
      data_[l] = 0;
    }
  }

  inline void set() {
    for (int l = 0; l < capacity_; ++l) {
      data_[l] = 0xFFFFFFFF;
    }
  }

  inline bool test_all() {
    bool all_set = true;
    for (int i = 0; i < capacity_; ++i)
      if (data_[i] != 0xFFFFFFFF)
        return false;
    return all_set;
  }

  inline int operator()(int index) const {
    return (data_(index >> 5) & (1 << (index & 31))) ? 1 : 0;
  }

  inline void set(int index) {
    data_[index >> 5] |= (1 << (index & 31));
  }

  inline void unset(int index) {
    data_[index >> 5] &= (~(1 << (index & 31)));
  }

 private:
  int bit_length_;
  int capacity_;
  dynamic_array<unsigned int> data_;
};

}  // namespace data_structures
}  // namespace parkway

#endif
