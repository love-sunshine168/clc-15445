//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  bool has_valid = false;

  bool has_less_k = false;
  size_t less_k_least = UINT32_MAX;
  frame_id_t less_id = 0;
  size_t more_k_most = UINT32_MAX;
  frame_id_t more_id = 0;

  for (frame_id_t id: cur_frames_) {
    LRUKNode node = node_store_[id];
    if (!(node.is_evictable_)) {continue;}
    if (has_less_k && (node.k_ >= k_)) {continue;}
    else if (node.k_ < k_) {
      has_valid = true;
      has_less_k = true;
      if (node.history_.front() < less_k_least) {
        less_k_least = node.history_.front();
        less_id = id;
      }
    }
    else {
      has_valid = true;
      if (node.history_.back() < more_k_most) {
        more_k_most = node.history_.back();
        more_id = id;
      }
    }
  }

  if (has_less_k) {
    *frame_id = less_id;
    Remove(less_id);
    return true;
  }
  else if (has_valid) {
    *frame_id = more_id;
    Remove(more_id);
    return true;
  }
  return false;
}

bool LRUKReplacer::FrameIsValid(frame_id_t frame_id) {
  if (size_t(frame_id) > replacer_size_ || size_t(frame_id) == 0) {
    return false;
  }
  return true;
}

void LRUKReplacer::UpdateFrameInfo(frame_id_t frame_id) {
  LRUKNode node = node_store_[frame_id];
  ++node.k_;
  node.history_.push_back(current_timestamp_);
  node_store_[frame_id] = node;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, AccessType access_type) {
  if (!FrameIsValid(frame_id)) {throw Exception("Wrong frame id!");}
  ++current_timestamp_;
  if (node_store_.count(frame_id)) {
    UpdateFrameInfo(frame_id);
  }
  else {
    cur_frames_.push_back(frame_id);
    LRUKNode node{{current_timestamp_}, 1, frame_id, false};
    node_store_[frame_id] = node;
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  ++current_timestamp_;
  if (node_store_.count(frame_id)) {
    //UpdateFrameInfo(frame_id);  此时不能更新时间戳
    if (node_store_[frame_id].is_evictable_ && !(set_evictable)) {
      node_store_[frame_id].is_evictable_ = false;
      --curr_size_;
    }
    else if (!(node_store_[frame_id].is_evictable_) && (set_evictable)) {
      node_store_[frame_id].is_evictable_ = true;
      ++curr_size_;
    }
  }
  else if (!FrameIsValid(frame_id)) {throw Exception("Wrong frame id!");}
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  ++current_timestamp_;
  if (!node_store_.count(frame_id)) return;
  if (!FrameIsValid(frame_id) || !(node_store_[frame_id].is_evictable_)) {throw Exception("Wrong remove!");}
  size_t i = 0;
  for (; i < cur_frames_.size(); ++i) {
    if (cur_frames_[i] == frame_id) break;
  }
  cur_frames_.erase(cur_frames_.begin()+i);
  node_store_.erase(frame_id);
  --curr_size_;
}

auto LRUKReplacer::Size() -> size_t { return curr_size_; }

}  // namespace bustub
