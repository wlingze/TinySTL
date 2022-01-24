#ifndef __TEST_ALLOCATOR_TEST__
#define __TEST_ALLOCATOR_TEST__

#include "cppunit/cppunit_mini.h"
#include "stl_alloc.h"
namespace test {
class allocator_test : public __test_base {
  CPPUNIT_TEST_START(allocator_test)
  CPPUNIT_TEST_BODY(zero_allocation)
  CPPUNIT_TEST_BODY(bad_alloc_test)
  CPPUNIT_TEST_END()

private:
  void zero_allocation();
  void bad_alloc_test();
};

inline void allocator_test::zero_allocation() {
  typedef tinystl::allocator<char> char_allocator_t;
  char_allocator_t char_allocator;
  char *buf = char_allocator.allocate(0);
  char_allocator.deallocate(buf, 0);
  char_allocator.deallocate(0, 0);
}

struct big_struct {
  char _data[0x1000];
};

inline void allocator_test::bad_alloc_test() {
  typedef tinystl::allocator<big_struct> big_struct_allocator_t;
  big_struct_allocator_t big_struct_allocator;

  try {
    big_struct_allocator_t::pointer p_big_struct =
        big_struct_allocator.allocate(0x100 * 0x100 * 0x100);

    CPPUNIT_ASSERT(p_big_struct != 0);
  } catch (bad_alloc const &) {
  } catch (...) {
    CPPUNIT_FAIL
  }
}

CPPUNIT_TEST_REGISTRATION(allocator_test)
} // namespace test

#endif //__TEST_ALLOCATOR_TEST__