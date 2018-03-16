#pragma once

#include <iostream>
#include <cassert>
#include <cstdint>
#include <vector>

#include "offset.h"

template<typename KeyT>
class BaseDynamicIndex {

public:
  BaseDynamicIndex() {}
  virtual ~BaseDynamicIndex() {}

  virtual void insert(const KeyT &key, const Uint64 &value) = 0;

  virtual void find(const KeyT &key, std::vector<Uint64> &values) = 0;

  virtual void find_range(const KeyT &lhs_key, const KeyT &rhs_key, std::vector<Uint64> &values) = 0;

  virtual void scan(const KeyT &key, std::vector<Uint64> &values) {}

  virtual void scan_reverse(const KeyT &key, std::vector<Uint64> &values) {}

  virtual void erase(const KeyT &key) = 0;

  virtual size_t size() const = 0;
  
  virtual void prepare_threads(const size_t thread_count) {}

  virtual void register_thread(const size_t thread_id) {}

  virtual void print() const {}

};