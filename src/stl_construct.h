#ifndef __TINYSTL_STL_CONSTRUCT__
#define __TINYSTL_STL_CONSTRUCT__

#include "stl_config.h"
#include "stl_iterator_base.h"
#include "type_traits.h"

STL_BEGIN_NAMESPACE

// construct (__p, __v)
template <class _Tp1, class _Tp2> void _construct(_Tp1 *__p, const _Tp2 &__v) {
  new (__p) _Tp2(__v);
}
template <class _Tp1, class _Tp2> void construct(_Tp1 *__p, const _Tp2 &__v) {
  _construct(__p, __v);
}

// construct (__p)
template <class _Tp> void _construct(_Tp *__p) { new (__p) _Tp(); }
template <class _Tp> void construct(_Tp *__p) { _construct(__p); }

// destory (__p)
template <class _Tp> void _destory(_Tp *__p) { __p->~_Tp(); }
template <class _Tp> void destory(_Tp *__p) { _destory(__p); }

// destory (__first, __last)
template <class _ForwardIterator>
void __destory_aux(_ForwardIterator __first, _ForwardIterator __last,
                   __false_type) {
  for (; __first != __last; __first++) {
    destory(&*__first);
  }
}
template <class _ForwardIterator>
void __destory_aux(_ForwardIterator __first, _ForwardIterator __last,
                   __true_type) {}
template <class _ForwardIterator, class _Tp>
void _destory(_ForwardIterator __first, _ForwardIterator __last, _Tp) {
  typedef
      typename __type_traits<_Tp>::has_trivial_destructor _trivial_destructor;
  __destory_aux(__first, __last, _trivial_destructor());
}
template <class _ForwardIterator>
void destory(_ForwardIterator __first, _ForwardIterator __last) {
  _destory(__first, __last, __VALUE_TYPE(__first));
}

STL_END_NAMESPACE

#endif // __TINYSTL_STL_CONSTRUCT__
