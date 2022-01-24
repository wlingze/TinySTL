#ifndef __TEST_CPPUNIT_UNTIL__
#define __TEST_CPPUNIT_UNTIL__

#include "cppunit_mini.h"
test::__reporter_base *test::__test_base::reporter = 0;

#include "../allocator_test.h"
using test::allocator_test;
CPPUNIT_TEST_REGISTRATION(allocator_test)

#include "../memory_test.h"
using test::memory_test;
CPPUNIT_TEST_REGISTRATION(memory_test)

#endif //__TEST_CPPUNIT_UNTIL__
