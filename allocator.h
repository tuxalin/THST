//
//  allocator.h
//
//

#pragma once

#include <assert.h>
#include <stdint.h>
#include <cstddef>

namespace spatial {
template <class NodeClass> struct heap_allocator {
  typedef NodeClass value_type;
  typedef NodeClass *ptr_type;

  enum { is_overflowable = 0 };

  heap_allocator() : count(0) {}

  ptr_type allocate(int level) {
    ++count;
    return new NodeClass(level);
  }

  void deallocate(const ptr_type node) {
    --count;
    assert(node);
    delete node;
  }

  bool overflowed() const { return false; }

  size_t count;
};
} // namespace spatial
