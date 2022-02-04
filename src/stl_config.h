#ifndef __TINYSTL_STL_CONFIG__
#define __TINYSTL_STL_CONFIG__

#define STL_NAMESPACE tinystl
#define STL_BEGIN_NAMESPACE namespace tinystl {
#define STL_END_NAMESPACE }

#define __STL_TEMPLATE_NULL template <>

#define __STL_TRY try
#define __STL_CATCH_ALL catch (...)
#define __STL_UNWIND(action)                                                   \
  catch (...) {                                                                \
    action;                                                                    \
    throw;                                                                     \
  }

#define __STL_DEFAULT_ALLOCATOR(T) allocator<T>

#endif // __TINYSTL_STL_CONFIG__