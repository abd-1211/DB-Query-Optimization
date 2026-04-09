//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.cpp
//
// Identification: src/primer/count_min_sketch.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/count_min_sketch.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace bustub {

/**
 * Constructor for the count-min sketch.
 *
 * @param width The width of the sketch matrix.
 * @param depth The depth of the sketch matrix.
 * @throws std::invalid_argument if width or depth are zero.
 */
template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {  //initialized sketch_ with constructor
  if(width==0 || depth==0)
  {
    throw std::invalid_argument("Width and Depth must be greater than 0");
  }
  sketch_ = std::vector<std::vector<uint32_t>>(depth_, std::vector<uint32_t>(width_));
  /** @spring2026 PLEASE DO NOT MODIFY THE FOLLOWING */
  // Initialize seeded hash functions
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.push_back(this->HashFunction(i));
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(CountMinSketch &&other) noexcept : width_(other.width_), depth_(other.depth_), sketch_(std::move(other.sketch_)),hash_functions_(std::move(other.hash_functions_)) {
  //moved resource ownership to new obj
}

template <typename KeyType>
auto CountMinSketch<KeyType>::operator=(CountMinSketch &&other) noexcept -> CountMinSketch & {
  /** @TODO(student) Implement this function! */
  if(this!=&other)
  {
  this->width_ = other.width_;
  this->depth_ = other.depth_;
  this->hash_functions_=std::move(other.hash_functions_);
  this->sketch_ = std::move(other.sketch_);
  }
  return *this;
  
}

template <typename KeyType>
void CountMinSketch<KeyType>::Insert(const KeyType &item) {
  /** @TODO(student) Implement this function! */  
  
  std::lock_guard<std::mutex> lock(mtx_); // mutex lock to make it thread safe

  for(size_t i=0; i<depth_;i++) //loop over each hash function
  {
  auto column =hash_functions_[i](item) % width_; //get value of key for ith hashfunction for the item we've selected
  sketch_[i][column]+=1; //keep count of item inserted at index
  }
} //mutex automatically unlocked when out of scope

template <typename KeyType>
void CountMinSketch<KeyType>::Merge(const CountMinSketch<KeyType> &other) {
  if (width_ != other.width_ || depth_ != other.depth_) {
    throw std::invalid_argument("Incompatible CountMinSketch dimensions for merge.");
  }
  /** @TODO(student) Implement this function! */
  for (size_t i = 0; i < depth_; i++) {
        for (size_t j = 0; j < width_; j++) {
            sketch_[i][j] += other.sketch_[i][j];
        }
      }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::Count(const KeyType &item) const -> uint32_t {
  uint32_t min_count = UINT32_MAX;

  for (size_t i = 0; i < depth_; i++) {
      auto column = hash_functions_[i](item) % width_;
      min_count = std::min(min_count, sketch_[i][column]);
  }

  return min_count;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Clear() {
  /** @TODO(student) Implement this function! */
  for (size_t i = 0; i < depth_; i++) {
    for (size_t j = 0; j < width_; j++) {
      sketch_[i][j] = 0;
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::TopK(uint16_t k, const std::vector<KeyType> &candidates)
    -> std::vector<std::pair<KeyType, uint32_t>> {
  /** @TODO(student) Implement this function! */
    std::vector<std::pair<KeyType, uint32_t>> results;

  // Step 1: compute estimated counts
  for (const auto &item : candidates) {
    uint32_t count = Count(item);
    results.emplace_back(item, count);
  }

  // Step 2: sort by count descending
  std::sort(results.begin(), results.end(),
            [](const auto &a, const auto &b) {
              return a.second > b.second;
            });

  // Step 3: limit to k results
  if (results.size() > k) {
    results.resize(k);
  }

  return results;
}

// Explicit instantiations for all types used in tests
template class CountMinSketch<std::string>;
template class CountMinSketch<int64_t>;  // For int64_t tests
template class CountMinSketch<int>;      // This covers both int and int32_t
}  // namespace bustub
