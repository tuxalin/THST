//
//  allocator.h
//
//

#pragma once

#include <assert.h>
#include <stdint.h>

namespace spatial {
template <class NodeClass> struct heap_allocator {
  typedef NodeClass value_type;
  typedef NodeClass *ptr_type;

  enum { is_overflowable = 0 };

  ptr_type allocate(int level) { return new NodeClass(level); }

  void deallocate(const ptr_type node) {
    assert(node);
    delete node;
  }

  bool overflowed() const { return false; }
};
} // namespace spatial
