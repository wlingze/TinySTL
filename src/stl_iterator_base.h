#ifndef __TINYSTL_STL_ITERATOR_BASE__
#define __TINYSTL_STL_ITERATOR_BASE__

/*

this file contains all of the general iterator-related utilites.
 */

#include "stl_config.h"
#include <cstddef>
STL_BEGIN_NAMESPACE

// Define the following categories.
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// Corresponding example.
template <class _Tp, class _Distance> struct input_iterator {
  typedef input_iterator_tag iterator_category;
  typedef _Distance difference_type;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

struct output_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void difference_type;
  typedef void value_type;
  typedef void pointer;
  typedef void reference;
};

template <class _Tp, class _Distance> struct forward_iterator {
  typedef forward_iterator_tag iterator_category;
  typedef _Distance difference_type;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

template <class _Tp, class _Distance> struct bidirectional_iterator {
  typedef bidirectional_iterator_tag iterator_category;
  typedef _Distance difference_type;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

template <class _Tp, class _Distance> struct random_access_iterator {
  typedef random_access_iterator_tag iterator_category;
  typedef _Distance difference_type;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

// The standent interface defined, std::iterator.
template <class _Category, class _Tp, class _Distance = ptrdiff_t,
          class _Pointer = _Tp *, class _Reference = _Tp &>
struct iterator {
  typedef _Category iterator_category;
  typedef _Distance difference_type;
  typedef _Tp value_type;
  typedef _Pointer pointer;
  typedef _Reference reference;
};

// trait
template <class _Iterator> struct iterator_traits {
  typedef typename _Iterator::iterator_category iterator_category;
  typedef typename _Iterator::difference_type difference_type;
  typedef typename _Iterator::value_type value_type;
  typedef typename _Iterator::pointer pointer;
  typedef typename _Iterator::reference reference;
};

// partial specialization
template <class _Tp> struct iterator_traits<_Tp *> {
  typedef random_access_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

template <class _Tp> struct iterator_traits<const _Tp *> {
  typedef random_access_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;
  typedef _Tp value_type;
  typedef const _Tp *pointer;
  typedef const _Tp &reference;
};

/* function

        * iterator_category
        * value_type
        * distance_type
*/
template <class _Iter>
typename iterator_traits<_Iter>::iterator_category
__iterator_category(const _Iter &) {
  typedef typename iterator_traits<_Iter>::iterator_category _category;
  return _category();
}
template <class _Iter>
typename iterator_traits<_Iter>::iterator_category
iterator_category(const _Iter &__i) {
  return __iterator_category(__i);
}

template <class _Iter>
typename iterator_traits<_Iter>::difference_type *
__distance_type(const _Iter &) {
  return static_cast<typename iterator_traits<_Iter>::difference_type *>(0);
}
template <class _Iter>
typename iterator_traits<_Iter>::difference_type *
distance_type(const _Iter &__i) {
  return __distance_type(__i);
}

template <class _Iter>
typename iterator_traits<_Iter>::value_type *__value_type(const _Iter &) {
  return static_cast<typename iterator_traits<_Iter>::value_type *>(0);
}
template <class _Iter>
typename iterator_traits<_Iter>::value_type *value_type(const _Iter &__i) {
  return __value_type(__i);
}

#define __ITERATOR_CATEGORY(__i) __iterator_category(__i)
#define __DISTANCE_TYPE(__i) __distance_type(__i)
#define __VALUE_TYPE(__i) __value_type(__i)

/*
    function2

        * distance
        * advance
*/

template <class _InputIterator>
typename iterator_traits<_InputIterator>::difference_type
__distance(_InputIterator __first, _InputIterator __last, input_iterator_tag) {
  typename iterator_traits<_InputIterator>::difference_type __n = 0;
  while (__first != __last) {
    __first++;
    __n++;
  }
  return __n;
}

template <class _RandomAccessIterator>
typename iterator_traits<_RandomAccessIterator>::difference_type
__distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
           random_access_iterator_tag) {
  return __last - __first;
}

template <class _InputIterator>
typename iterator_traits<_InputIterator>::difference_type
distance(_InputIterator __first, _InputIterator __last) {
  return __distance(__first, __last, iterator_category(__first));
}

template <class _InputIterator, class _Distance = typename iterator_traits<
                                    _InputIterator>::difference_type>
void __advance(_InputIterator &__i, _Distance __n, input_iterator_tag) {
  while (__n--)
    __i++;
}

template <class _BidirectionalIterator,
          class _Distance =
              typename iterator_traits<_BidirectionalIterator>::difference_type>
void __advance(_BidirectionalIterator &__i, _Distance __n,
               bidirectional_iterator_tag) {
  if (__n > 0) {
    while (__n--)
      __i++;
  } else {
    while (__n++)
      __i--;
  }
}

template <class _RandomAccessIterator,
          class _Distance =
              typename iterator_traits<_RandomAccessIterator>::difference_type>
void __advance(_RandomAccessIterator &__i, _Distance __n,
               random_access_iterator_tag) {
  __i += __n;
}

template <class _InputIterator, class _Distance = typename iterator_traits<
                                    _InputIterator>::difference_type>
void advance(_InputIterator &__i, _Distance __n) {
  typename iterator_traits<_InputIterator>::iterator_category _Category;
  __advance(__i, __n, iterator_category(__i));
}

STL_END_NAMESPACE

#endif //__TINYSTL_STL_ITERATOR_BASE__