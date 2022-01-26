#ifndef __TINYSTL_TYPE_TRAITS__
#define __TINYSTL_TYPE_TRAITS__

/*
This header file provides a framework for allowing compile time dispatch base on
type attributes.
*/

#include "stl_config.h"

STL_BEGIN_NAMESPACE

struct __true_type {};
struct __false_type {};

template <class _Tp> struct __type_traits {
  typedef __true_type this_dummy_member_must_be_first;
  /*
      This variable is used to indicate to some compilers that can support
     automatic specialization of __type_trait that this __type_traits is
     special.
  */

  typedef __false_type has_trivial_default_constructor;
  typedef __false_type has_trivial_copy_constructor;
  typedef __false_type has_assignment_operator;
  typedef __false_type has_trivial_destructor;
  typedef __false_type is_POD_type;
};

template <class _Tp> struct __type_traits_aux {};

__STL_TEMPLATE_NULL struct __type_traits_aux<__true_type> {
  typedef __true_type has_trivial_default_constructor;
  typedef __true_type has_trivial_copy_constructor;
  typedef __true_type has_trivial_assignment_operator;
  typedef __true_type has_trivial_destructor;
  typedef __true_type is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits_aux<__false_type> {
  typedef __false_type has_trivial_default_constructor;
  typedef __false_type has_trivial_copy_constructor;
  typedef __false_type has_trivial_assignment_operator;
  typedef __false_type has_trivial_destructor;
  typedef __false_type is_POD_type;
};

#define STL_DEFINE_TYPE_TRAITS_FOR(type)                                       \
  __STL_TEMPLATE_NULL struct __type_traits<type>                               \
      : __type_traits_aux<__true_type> {};                                     \
  __STL_TEMPLATE_NULL struct __type_traits<const type>                         \
      : __type_traits_aux<__true_type> {};                                     \
  __STL_TEMPLATE_NULL struct __type_traits<volatile type>                      \
      : __type_traits_aux<__true_type> {};                                     \
  __STL_TEMPLATE_NULL struct __type_traits<const volatile type>                \
      : __type_traits_aux<__true_type> {};

STL_DEFINE_TYPE_TRAITS_FOR(bool)
STL_DEFINE_TYPE_TRAITS_FOR(int)
STL_DEFINE_TYPE_TRAITS_FOR(unsigned int)
STL_DEFINE_TYPE_TRAITS_FOR(short)
STL_DEFINE_TYPE_TRAITS_FOR(unsigned short)
STL_DEFINE_TYPE_TRAITS_FOR(char)
STL_DEFINE_TYPE_TRAITS_FOR(unsigned char)
STL_DEFINE_TYPE_TRAITS_FOR(signed char)
STL_DEFINE_TYPE_TRAITS_FOR(long)
STL_DEFINE_TYPE_TRAITS_FOR(unsigned long)
STL_DEFINE_TYPE_TRAITS_FOR(float)
STL_DEFINE_TYPE_TRAITS_FOR(double)
STL_DEFINE_TYPE_TRAITS_FOR(long double)
template <class _Tp>
struct __type_traits<_Tp *> : __type_traits_aux<__true_type> {};

// function is integer

template <class _Tp> struct _Is_integer { typedef __false_type _Integral; };

__STL_TEMPLATE_NULL struct _Is_integer<bool> { typedef __true_type _Integral; };

__STL_TEMPLATE_NULL struct _Is_integer<char> { typedef __true_type _Integral; };

__STL_TEMPLATE_NULL struct _Is_integer<signed char> {
  typedef __true_type _Integral;
};

__STL_TEMPLATE_NULL struct _Is_integer<unsigned char> {
  typedef __true_type _Integral;
};

__STL_TEMPLATE_NULL struct _Is_integer<short> {
  typedef __true_type _Integral;
};

__STL_TEMPLATE_NULL struct _Is_integer<unsigned short> {
  typedef __true_type _Integral;
};

__STL_TEMPLATE_NULL struct _Is_integer<int> { typedef __true_type _Integral; };

__STL_TEMPLATE_NULL struct _Is_integer<unsigned int> {
  typedef __true_type _Integral;
};

__STL_TEMPLATE_NULL struct _Is_integer<long> { typedef __true_type _Integral; };

__STL_TEMPLATE_NULL struct _Is_integer<unsigned long> {
  typedef __true_type _Integral;
};

STL_END_NAMESPACE

#endif //__TINYSTL_TYPE_TRAITS__