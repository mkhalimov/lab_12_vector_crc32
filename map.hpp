#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <utility>

#include "crc32.hpp"
#include "vector.hpp"

#define STD_MAP_CAPACITY 128

template <class Key, class T> class Map {
public:
  class Iterator;

  // -- Member types --

  typedef Key key_type;
  typedef T mapped_type;
  typedef std::pair<uint32_t, T> value_type;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef Iterator iterator;
  typedef const Iterator const_iterator;

  // -- Member functions --

  Map() {}

  Map(const Map &other) : data_(other.data_) {}

  ~Map() {}

  Map &operator=(const Map &other) {
    data_ = other.data_;
    return (*this);
  }

  // -- Element accesshashed_key

  T &at(const Key &key) {
    uint32_t hashed_key = crc32(key);
    iterator it;

    for (it = begin(); it != end(); ++it) {
      if (it->first == hashed_key)
        return it->second;
    }
    throw std::out_of_range("No element with this key\n");
  }

  const T &at(const Key &key) const {
    uint32_t hashed_key = crc32(key);
    iterator it;

    for (it = begin(); it != end(); ++it) {
      if (it->first == hashed_key)
        return it->second;
    }
    throw std::out_of_range("No element with this key\n");
  }

  T &operator[](const Key &key) {
    uint32_t hashed_key = crc32(key);
    iterator it;

    for (it = this->begin(); it != this->end(); ++it)
      if (hashed_key == it->first)
        break;

    if (it == this->end()) {
      this->insert(std::pair<Key, T>{key, T()});
      sort(data_.begin(), data_.end(), key_cmp);

      for (it = this->begin(); it != this->end(); ++it)
        if (hashed_key == it->first)
          break;
    }

    if (it == NULL)
      return this->data_.back().second;

    return it->second;
  }

  // -- Iterators

  iterator begin() { return Iterator(data_.data()); }

  iterator end() { return Iterator(data_.data() + data_.size()); }

  const_iterator cbegin() const { return Iterator(data_.data()); }

  const_iterator cend() const { return Iterator(data_.data() + data_.size()); }

  // -- Capacity

  bool empty() const { return data_.empty(); }

  size_type size() const { return data_.size(); }

  size_type max_size() const { return data_.max_size(); }

  // -- Modifiers

  void clear() { data_.clear(); }

  std::pair<iterator, bool> insert(const std::pair<Key, T> &val) {
    uint32_t hashed_key = crc32(val.first);
    iterator it;

    for (it = this->begin(); it != this->end();
         ++it) { // Проверяем, есть ли ключ, который мы хотим вставить в списке
      if (it->first == hashed_key)
        return std::pair<iterator, bool>(
            it, false); // Вставить такой ключ не получилось, он уже есть
    }
    data_.push_back(std::pair<uint32_t, T>{hashed_key, val.second});
    std::sort(data_.begin(), data_.end(), key_cmp);
    return std::pair<iterator, bool>(this->find(val.first), true);
  }

  iterator insert(iterator pos, const std::pair<Key, T> &val) {
    uint32_t hashed_key = crc32(val.first);
    data_.push_back(std::pair<uint32_t, T>{hashed_key, val.second});
    std::sort(this->begin(), this->end(), key_cmp);
    return this->find(val.first);
  }

  iterator erase(iterator pos) {
    data_.erase();
    std::sort(this->begin(), this->end(), key_cmp);
    return ++pos;
  }

  size_type erase(const Key &key) {
    iterator it = find(key);
    if (it == end())
      return 0;
    this->erase(it);
    return 1;
  }

  void swap(Map &other) {
    Map<Key, T> tmp = *this;
    *this = other;
    other = tmp;
  }

  // -- Lookup

  size_type count(const Key &key) const {
    return (this->find(key) == this->end()) ? (0) : (1);
  }

  iterator find(const Key &key) {
    uint32_t hashed_key = crc32(key);
    iterator it;

    for (it = begin(); it != end(); ++it) {
      if (hashed_key == it->first)
        break;
    }
    return it;
  }

  const_iterator find(const Key &key) const {
    uint32_t hashed_key = crc32(key);
    iterator it;

    for (it = begin(); it != end(); ++it) {
      if (hashed_key == it->first)
        break;
    }
    return it;
  }

  std::pair<const_iterator, const_iterator> equal_range(const Key &key) const {
    iterator lower_it, upper_it;
    iterator it = begin();
    uint32_t hashed_key = crc32(key);

    while (it->first < hashed_key)
      ++it;
    lower_it = it;
    while (it->first <= hashed_key)
      ++it;
    upper_it = it;
    return std::pair<const_iterator, const_iterator>(lower_it, upper_it);
  }

  iterator lower_bound(const Key &key) {
    iterator it = begin();
    uint32_t hashed_key = crc32(key);

    while (it->first < hashed_key)
      ++it;
    return it;
  }

  const_iterator lower_bound(const Key &key) const {
    iterator it = begin();
    uint32_t hashed_key = crc32(key);

    while (it->first < hashed_key)
      ++it;
    return it;
  }

  iterator upper_bound(const Key &key) {
    iterator it = begin();
    uint32_t hashed_key = crc32(key);

    while (it->first <= hashed_key)
      ++it;
    return it;
  }

  const_iterator upper_bound(const Key &key) const {
    iterator it = begin();
    uint32_t hashed_key = crc32(key);

    while (it->first <= hashed_key)
      ++it;
    return it;
  }

  friend void swap(Map &lhs, Map &rhs) {
    Map tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // -- Non-member functions

  friend bool operator==(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    iterator l = lhs.begin();
    iterator r = rhs.begin();

    if (lhs.size() != rhs.size()) {
      return false;
    }
    for (; l != lhs.end(); ++l, ++r) {
      if (*l != *r)
        return false;
    }
    return true;
  }

  friend bool operator!=(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator>(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    iterator l = lhs.begin();
    iterator r = rhs.begin();

    if (lhs.size() > rhs.size()) {
      return true;
    } else if (lhs.size() < rhs.size()) {
      return false;
    }
    for (; l != lhs.end(); ++l, ++r) {
      if (*l > *r)
        return true;
      else if (*l < *r)
        return false;
    }
    return false;
  }

  friend bool operator<(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    iterator l = lhs.begin();
    iterator r = rhs.begin();

    if (lhs.size() < rhs.size()) {
      return true;
    } else if (lhs.size() > rhs.size()) {
      return false;
    }
    for (; l != lhs.end(); ++l, ++r) {
      if (*l < *r)
        return true;
      else if (*l > *r)
        return false;
    }
    return false;
  }

  friend bool operator<=(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    return !(lhs > rhs);
  }
  friend bool operator>=(const Map<Key, T> &lhs, const Map<Key, T> &rhs) {
    return !(lhs < rhs);
  }

private:
  Vector<std::pair<uint32_t, T>> data_;
  CRC32<Key> crc32;

  // -- Private functions --

  static bool key_cmp(const std::pair<uint32_t, T> &a,
                      const std::pair<uint32_t, T> &b) {
    return (a.first < b.first);
  }
};

template <class Key, class T> class Map<Key, T>::Iterator {
public:
  // -- Member types --

  typedef ptrdiff_t difference_type;
  typedef typename Map<Key, T>::value_type value_type;
  typedef typename Map<Key, T>::pointer pointer;
  typedef typename Map<Key, T>::reference reference;
  typedef std::bidirectional_iterator_tag iterator_category;

  // -- Member functions --

  Iterator() : ptr_(NULL) {}

  Iterator(const Iterator &other) : ptr_(other.ptr_) {}

  Iterator(pointer cont) : ptr_(cont) {}

  ~Iterator() {}

  Iterator &operator=(const Iterator &other) {
    ptr_ = other.ptr_;
    return *this;
  }

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  iterator &operator++() {
    ++ptr_;
    return *this;
  }

  iterator &operator++(int) {
    pointer old = ptr_;
    ++ptr_;
    return old;
  }

  iterator &operator--() {
    --ptr_;
    return *this;
  }

  iterator &operator--(int) {
    pointer old = ptr_;
    --ptr_;
    return old;
  }

  friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
    return (lhs.ptr_ == rhs.ptr_);
  }

  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
    return (lhs.ptr_ != rhs.ptr_);
  }

private:
  pointer ptr_;
};
