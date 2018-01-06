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

  ///@note If true then overflow checks will be performed.
  enum { is_overflowable = 0 };

  ptr_type allocate(int level) { return new NodeClass(level); }

  void deallocate(const ptr_type node) {
    assert(node);
    delete node;
  }

  ///@note Only used if is_overflowable is true.
  bool overflowed() const { return false; }
};
} // namespace spatial
