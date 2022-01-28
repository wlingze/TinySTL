#ifndef __TINYSTL_STL_CONFIG__
#define __TINYSTL_STL_CONFIG__

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

#endif // __TINYSTL_STL_CONFIG__