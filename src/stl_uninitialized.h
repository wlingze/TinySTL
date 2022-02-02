#ifndef __TINYSTL_uninitializedD__
#define __TINYSTL_uninitializedD__

#include "stl_algobase.h"
#include "stl_config.h"
#include "stl_construct.h"
#include "stl_iterator_base.h"
#include "type_traits.h"
#include <cstdio>

/*
uninitialized_copy
    * is POD
        -- why do this?
        * while Construct
    * is not
        * copy
*/
STL_BEGIN_NAMESPACE
template <class _ForwardIterator, class _InputIterator>
_ForwardIterator
_uninitialized_copy_aux(_InputIterator __first, _InputIterator __last,
                        _ForwardIterator __result, __true_type) {
  return copy(__first, __last, __result);
}

template <class _ForwardIterator, class _InputIterator>
_ForwardIterator
_uninitialized_copy_aux(_InputIterator __first, _InputIterator __last,
                        _ForwardIterator __result, __false_type) {
  _ForwardIterator __cur = __result;
  __STL_TRY {
    for (; __first != __last; __cur++, __first++) {
      construct(__cur, __first);
    }
    return __cur;
  }
  __STL_UNWIND(destory(__result, __cur);)
}

template <class _OutputIterator, class _InputIterator, class _Tp>
_OutputIterator _uninitialized_copy(_InputIterator __first,
                                    _InputIterator __last,
                                    _OutputIterator __result, _Tp *) {
  typedef typename __type_traits<_Tp>::is_POD_type _isPOD;
  return _uninitialized_copy_aux(__first, __last, __result, _isPOD());
}

template <class _OutputIterator, class _InputIterator>
_OutputIterator uninitialized_copy(_InputIterator __first,
                                   _InputIterator __last,
                                   _OutputIterator __result) {
  return _uninitialized_copy(__first, __last, __result, __VALUE_TYPE(__first));
}

/*
uninitialized_copy_n
    * input_iterator_tag
        * while + construct
    * random_access_iterator_tag
        * _last = __first + __n => copy(__first, __last)
*/
template <class _InputIterator, class _ForwardIterator, class _Size>
_ForwardIterator _uninitialized_copy_n(_InputIterator __first, _Size __n,
                                       _ForwardIterator __result,
                                       forward_iterator_tag) {
  _ForwardIterator __cur = __result;
  __STL_TRY {
    for (; __n > 0; __cur++, __first++, __n--) {
      construct(__cur, __first);
    }
    return __cur;
  }
  __STL_UNWIND(destory(__result, __cur))
}

template <class _RandomIterator, class _ForwardIterator, class _Size>
_ForwardIterator _uninitialized_copy_n(_RandomIterator __first, _Size __n,
                                       _ForwardIterator __result,
                                       random_access_iterator_tag) {
  _RandomIterator __last = __first + __n;
  return copy(__first, __last, __result);
}

template <class _InputIterator, class _OutputIterator, class _Size>
_OutputIterator uninitialized_copy_n(_InputIterator __first, _Size __n,
                                     _OutputIterator __result) {
  return _uninitialized_copy_n(__first, __n, __result,
                               __ITERATOR_CATEGORY(__first));
}

/*
uninitialized_fill
    * POD
        while construct
    * other
        fill
*/
template <class _ForwardIterator, class _Tp>
_ForwardIterator _uninitialized_fill_aux(_ForwardIterator __first,
                                         _ForwardIterator __last,
                                         const _Tp &__x, __true_type) {
  return fill(__first, __last, __x);
}

template <class _ForwardIterator, class _Tp>
_ForwardIterator _uninitialized_fill_aux(_ForwardIterator __first,
                                         _ForwardIterator __last,
                                         const _Tp &__x, __false_type) {
  _ForwardIterator __cur = __first;
  __STL_TRY {
    for (; __first != __last; __cur++, __first++) {
      construct(__cur, __first);
    }
    return __cur;
  }
  __STL_UNWIND(destory(__first, __cur);)
}

template <class _ForwardIterator, class _Tp, class _Tp1>
_ForwardIterator _uninitialized_fill(_ForwardIterator __first,
                                     _ForwardIterator __last, const _Tp &__x,
                                     _Tp1 *) {
  typedef typename __type_traits<_Tp1>::is_POD_type _isPOD;
  return _uninitialized_fill_aux(__first, __last, __x, _isPOD());
}

template <class _ForwardIterator, class _Tp>
_ForwardIterator uninitialized_fill(_ForwardIterator __first,
                                    _ForwardIterator __last, const _Tp &__x) {
  return _uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
}

/*
uninitialized_fill
*/
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                                               const _Tp &__x, __true_type) {
  return fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp>
_ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                                        const _Tp &__x, __false_type) {
  _ForwardIter __cur = __first;
  __STL_TRY {
    for (; __n > 0; --__n, ++__cur)
      _construct(&*__cur, __x);
    return __cur;
  }
  __STL_UNWIND(destory(__first, __cur));
}

template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
inline _ForwardIter __uninitialized_fill_n(_ForwardIter __first, _Size __n,
                                           const _Tp &__x, _Tp1 *) {
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}

template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter uninitialized_fill_n(_ForwardIter __first, _Size __n,
                                         const _Tp &__x) {
  return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
}

/*
uninitialize_fill_copy(__result, __mid, __x, __first, __last)
    fill: [__result, mid) => __x
    copy: [mid, mid+(last-first)) => [first, last)
*/
template <class _InputIterator, class _OutputIterator, class _Tp>
_OutputIterator uninitialize_fill_copy(_OutputIterator __result,
                                       _OutputIterator __mid, const _Tp &__x,
                                       _InputIterator __first,
                                       _InputIterator __last) {
  uninitialized_fill(__result, __mid, __x);
  __STL_TRY { return uninitialized_copy(__first, __last, __mid); }
  __STL_UNWIND(destory(__result, __mid);)
}

/*
uninitialize_copy_copy(__first1, __last1, __first2, __last2, __result)
    copy: [result, result + (last1-first1)) <= [first1, last1)
    copy: [result + (last1-first1), result + (last1-first1) + (last2-first2)) <=
[first2, last2)
*/
template <class _InputIterator1, class _InputIterator2, class _OutputIterator>
_OutputIterator
uninitialize_copy_copy(_InputIterator1 __first1, _InputIterator1 __last1,
                       _InputIterator2 __first2, _InputIterator2 __last2,
                       _OutputIterator __result) {
  _OutputIterator __mid = uninitialized_copy(__first1, __last1, __result);
  __STL_TRY { return uninitialized_copy(__first2, __last2, __mid); }
  __STL_UNWIND(destory(__result, __mid);)
}

/*
uninitialized_copy_fill(__first1, __last2, __first2, __last2, __x)
    copy [first1, last1) => [first2, mid)
    fill [mid, last2) => x
*/
template <class _InputIterator, class _OutputIterator, class _Tp>
void uninitialize_copy_fill(_InputIterator __first1, _InputIterator __last1,
                            _OutputIterator __first2, _OutputIterator __last2,
                            const _Tp &__x) {
  _OutputIterator __mid = uninitialized_copy(__first1, __last1, __first2);
  __STL_TRY { uninitialized_fill(__mid, __last2, __x); }
  __STL_UNWIND(destory(__first2, __mid);)
}

STL_END_NAMESPACE

#endif //  __TINYSTL_uninitializedD__