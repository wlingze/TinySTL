#ifndef __TINYSTL_STL_ALGOBASE__
#define __TINYSTL_STL_ALGOBASE__

#include "stl_config.h"
#include "stl_iterator_base.h"
#include "type_traits.h"
#include <string.h>

STL_BEGIN_NAMESPACE

/* copy

    * has trivial assignment operator
        => memmove
    * don't has trivial assignment operator
        * forward iterator
            => not __n
        * random access iterator
            => __n
 */
template <class _InputIterator, class _OutputIterator, class _Distance>
_OutputIterator _copy_aux2(_InputIterator __first, _InputIterator __last,
                           _OutputIterator __result, forward_iterator_tag,
                           _Distance *) {
  for (; __first != __last; __first++, __result++) {
    *__result = *__first;
  }
  return __result;
}

template <class _InputIterator, class _OutputIterator, class _Distance>
_OutputIterator _copy_aux2(_InputIterator __first, _InputIterator __last,
                           _OutputIterator __result, random_access_iterator_tag,
                           _Distance *) {
  _Distance __n = __last - __first;
  for (; __n > 0; __n--) {
    *__result = *__first;
    __result++;
    __first;
  }
  return __result;
}

template <class _InputIterator, class _OutputIterator>
_OutputIterator _copy_aux(_InputIterator __first, _InputIterator __last,
                          _OutputIterator __result, __false_type) {
  return _copy_aux2(__first, __last, __result, __ITERATOR_CATEGORY(__first),
                    __DISTANCE_TYPE(__first));
}

template <class _Tp>
_Tp *_copy_aux(_Tp *__first, _Tp *__last, _Tp *__result, __true_type) {
  memmove(__first, __result, sizeof(_Tp) * (__last - __first));
  return __result + (__last - __first);
}

template <class _InputIterator, class _OutputIterator, class _Tp>
_OutputIterator _copy(_InputIterator __first, _InputIterator __last,
                      _OutputIterator __result, _Tp *) {
  typedef typename __type_traits<_Tp>::has_trivial_assignment_operator _Trivial;
  return _copy_aux(__first, __last, __result, _Trivial());
}

template <class _InputIterator, class _OutputIterator>
_OutputIterator copy(_InputIterator __first, _InputIterator __last,
                     _OutputIterator __result) {
  return _copy(__first, __last, __result, __VALUE_TYPE(__first));
}

/*
  fill

    fill from __first to __last, set to __value;
    fill_n from __first to __first+__n, set to __value;
 */

template <class _ForwardIterator, class _Tp>
void fill(_ForwardIterator __first, _ForwardIterator __last,
          const _Tp &__value) {
  for (; __first != __last; __first++, __last++) {
    *__first = __value;
  }
}

template <class _OutputIterator, class _Size, class _Tp>
_OutputIterator fill_n(_OutputIterator __first, _Size __n, const _Tp &__value) {
  for (; __n > 0; __n--, __first++) {
    *__first = __value;
  }
  return __first;
}

template <> void fill(char *__first, char *__last, const char &__value) {
  char __tmp = __value;
  memset(__first, __tmp, (__last - __first));
}

template <>
void fill(unsigned char *__first, unsigned char *__last,
          const unsigned char &__value) {
  char __tmp = __value;
  memset(__first, __tmp, (__last - __first));
}

template <>
void fill(signed char *__first, signed char *__last,
          const signed char &__value) {
  char __tmp = __value;
  memset(__first, __tmp, (__last - __first));
}

template <class _Size>
char *fill_n(char *__first, _Size __n, const char &__value) {
  fill(__first, __first + __n, __value);
  return __first + __n;
}

template <class _Size>
unsigned char *fill_n(unsigned char *__first, _Size __n,
                      const unsigned char &__value) {
  fill(__first, __first + __n, __value);
  return __first + __n;
}

template <class _Size>
signed char *fill_n(signed char *__first, _Size __n,
                    const signed char &__value) {
  fill(__first, __first + __n, __value);
  return __first + __n;
}

STL_END_NAMESPACE

#endif // __TINYSTL_STL_ALGOBASE__