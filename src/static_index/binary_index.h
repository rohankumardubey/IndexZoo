#pragma once

#include <vector>
#include <cmath>

#include "base_static_index.h"

namespace static_index {

template<typename KeyT, typename ValueT>
class BinaryIndex : public BaseStaticIndex<KeyT, ValueT> {

public:
  BinaryIndex(DataTable<KeyT, ValueT> *table_ptr, const size_t num_layers = 4) : BaseStaticIndex<KeyT, ValueT>(table_ptr), num_layers_(num_layers) {}

  virtual ~BinaryIndex() {}

  virtual void find(const KeyT &key, std::vector<Uint64> &values) final {

    if (this->size_ == 0) {
      return;
    }

    if (key > key_max_ || key < key_min_) {
      return;
    }
    if (key_max_ == key_min_) {
      if (key_max_ == key) {
        for (size_t i = 0; i < this->size_; ++i) {
          values.push_back(this->container_[i].value_);
        }
      }
      return;
    }

    size_t offset_find = find_internal(key, 0, this->size_);

    if (offset_find == this->size_) {
      // find nothing
      return;
    }

    values.push_back(this->container_[offset_find].value_);

    // move left
    int offset_find_lhs = offset_find - 1;
    while (offset_find_lhs >= 0) {

      if (this->container_[offset_find_lhs].key_ == key) {
        values.push_back(this->container_[offset_find_lhs].value_);
        offset_find_lhs -= 1;
      } else {
        break;
      }
    }
    // move right
    int offset_find_rhs = offset_find + 1;
    while (offset_find_rhs < this->size_ - 1) {

      if (this->container_[offset_find_rhs].key_ == key) {
        values.push_back(this->container_[offset_find_rhs].value_);
        offset_find_rhs += 1;
      } else {
        break;
      }
    }
  }

  virtual void find_range(const KeyT &lhs_key, const KeyT &rhs_key, std::vector<Uint64> &values) final {
    assert(lhs_key < rhs_key);

    if (this->size_ == 0) {
      return;
    }
    if (lhs_key > key_max_ || rhs_key < key_min_) {
      return;
    }

  }

  virtual void reorganize() final {

    this->base_reorganize();

    ASSERT(std::log(this->size_) / std::log(2) > num_layers_, "exceed maximum layers");

    key_min_ = this->container_[0].key_;
    key_max_ = this->container_[this->size_ - 1].key_;
    inner_nodes_ = new KeyT[this->size_];
  }

  virtual void print() const final {

  }

  virtual void print_stats() const final {

  }

private: 

  size_t find_internal(const KeyT &key, const size_t offset_begin, const size_t offset_end) {
    if (offset_begin > offset_end) {
      return this->size_;
    }
    size_t offset_lookup = (offset_begin + offset_end) / 2;
    KeyT key_lookup = this->container_[offset_lookup].key_;
    if (key == key_lookup) {
      return offset_lookup;
    }
    if (key > key_lookup) {
      return find_internal(key, offset_lookup + 1, offset_end);
    } else {
      return find_internal(key, offset_begin, offset_lookup - 1);
    }
  }
  
  void construct_inner_layers() {
    if (max_layer == 0) { return; }

    size_t mid_offset = (begin_offset + end_offset) / 2;
    dst_arr[0] = src_arr[mid_offset];
    if (max_layer == 1) { return; }

    construct_inner_layers_internal(src_arr, dst_arr, begin_offset, mid_offset - 1, 1, 0, max_layer, 1);
    construct_inner_layers_internal(src_arr, dst_arr, mid_offset + 1, end_offset, 1, 1, max_layer, 1);
  }

  void construct_inner_layers_internal(const int begin_offset, const int end_offset, const int base_pos, const int dst_pos, const int max_layer, const int curr_layer) {
    if (begin_offset > end_offset) { return; }

    size_t mid_offset = (begin_offset + end_offset) / 2;
    dst_arr[base_pos + dst_pos] = src_arr[mid_offset];

    int new_base_pos = (base_pos + 1) * 2 - 1;

    if (max_layer == curr_layer + 1) {
      return;
    }

    construct_inner_layers_internal(src_arr, dst_arr, begin_offset, mid_offset - 1, new_base_pos, dst_pos * 2, max_layer, curr_layer + 1);
    construct_inner_layers_internal(src_arr, dst_arr, mid_offset + 1, end_offset, new_base_pos, dst_pos * 2 + 1, max_layer, curr_layer + 1);
  }

private:

  size_t num_layers_;

  KeyT key_min_;
  KeyT key_max_;
  KeyT *inner_nodes_;

};

}