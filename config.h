//
//  config.h
//
//

#pragma once

#if defined(_MSC_VER)
#if _MSC_VER >= 1800
#define SPATIAL_TREE_USE_CPP11
#endif
#elif __cplusplus >= 201103L
#define SPATIAL_TREE_USE_CPP11
#endif

/// Preprocessor helper macros
#define SPATIAL_TREE_STRINGIFY(x) #x
#define SPATIAL_TREE_TOSTRING(x) SPATIAL_TREE_STRINGIFY(x)
#define SPATIAL_TREE_TOKENPASTE(x, y) x##y
#define SPATIAL_TREE_TOKENPASTE2(x, y) SPATIAL_TREE_TOKENPASTE(x, y)

#ifdef SPATIAL_TREE_CPP11
// we have C++11 so use built in static assert
#define SPATIAL_TREE_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else

#define SPATIAL_TREE_STATIC_ASSERT(cond, msg)                                  \
  spatial::detail::static_assertion<(cond)> SPATIAL_TREE_TOKENPASTE2(          \
      static_assert_t, __LINE__);                                              \
  (void)(SPATIAL_TREE_TOKENPASTE2(static_assert_t, __LINE__));

#if defined(__clang__)
#if __has_attribute(enable_if) && !defined(NDEBUG)
// use enable_if extension on clang to print the assert message
#undef SPATIAL_TREE_STATIC_ASSERT
#define SPATIAL_TREE_STATIC_ASSERT(cond, msg)                                  \
  _Pragma("clang diagnostic push");                                            \
  _Pragma("clang diagnostic ignored \"-Wgcc-compat\"");                        \
  struct SPATIAL_TREE_TOKENPASTE2(s, __LINE__) {                               \
    static void static_check(bool) {}                                          \
    static void static_check(bool c)                                           \
        __attribute__((enable_if(c == false, "invalid")))                      \
            __attribute__((unavailable(msg))) {                                \
      (void)(c);                                                               \
    }                                                                          \
  };                                                                           \
  SPATIAL_TREE_TOKENPASTE2(s, __LINE__)::static_check(bool(cond));             \
  _Pragma("clang diagnostic pop");                                             \
  do {                                                                         \
  } while (false)
#endif //#if __has_attribute(enable_if) && !defined(NDEBUG)
#endif //#if defined(__clang__)

#endif // #ifdef SPATIAL_TREE_CPP11

namespace spatial {
namespace detail {
struct dummy_iterator {
  dummy_iterator &operator++() { return *this; }
  dummy_iterator &operator*() { return *this; }
  template <typename T> dummy_iterator &operator=(const T &) { return *this; }
};

template <bool> struct static_assertion;
template <> struct static_assertion<true> {};
} // namespace detail
} // namespace spatial
