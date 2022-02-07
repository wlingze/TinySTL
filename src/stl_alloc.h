#ifndef __TINYSTL_STL_ALLOC__
#define __TINYSTL_STL_ALLOC__

#include <malloc/_malloc.h>
#include <stdio.h>
#include <sys/cdefs.h>

#include <cassert>
#include <cstddef>
#include <new>
using std::bad_alloc;

#include "stl_config.h"

#ifndef __THROW_BAD_ALLOC
// #define __THROW_BAD_ALLOC                                                      \
//   fprintf(stderr, "out of memory!\n");                                         \
//   exit(1)
#define __THROW_BAD_ALLOC throw bad_alloc()
#endif

#define __NODE_ALLOCATOR_LOCK
#define __NODE_ALLOCATOR_UNLOCK

#define __NODE_ALLOCATOR_THREADS false

STL_BEGIN_NAMESPACE

// malloc-base allocator.
template <int __inst> class __malloc_alloc_template {
private:
  static void *_S_oom_malloc(size_t);
  static void *_S_oom_realloc(void *, size_t);

  static void (*__malloc_alloc_oom_handler)();

public:
  // simple wrapped the malloc/free/realloc.
  static void *alloc(size_t __n) {
    void *__result = malloc(__n);
    if (__result == 0)
      _S_oom_malloc(__n);
    return __result;
  }

  static void deallocate(void *__p, size_t /* __n */) { free(__p); }

  static void *reallocate(void *__p, size_t /* __old_size */,
                          size_t __new_size) {
    void *__result = realloc(__p, __new_size);
    if (__result == 0)
      _S_oom_realloc(__p, __new_size);
    return __result;
  }

  static void (*set_malloc_handler(void (*__f)()))() {
    void (*__old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = __f;
    return __old;
  }
};

// __malloc_alloc allocator out-of-memory handler.
template <int __inst>
void (*__malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = 0;

template <int __inst>
void *__malloc_alloc_template<__inst>::_S_oom_malloc(size_t __n) {
  void (*__my_oom_handler)();
  void *__result;
  for (;;) {
    __my_oom_handler = __malloc_alloc_oom_handler;
    if (__my_oom_handler == 0) {
      __THROW_BAD_ALLOC;
    }
    (*__my_oom_handler)();
    __result = malloc(__n);
    if (__result)
      return __result;
  }
}

template <int __inst>
void *__malloc_alloc_template<__inst>::_S_oom_realloc(void *__p, size_t __n) {
  void (*__my_oom_handler)();
  void *__result;
  for (;;) {
    __my_oom_handler = __malloc_alloc_oom_handler;
    if (__my_oom_handler == 0) {
      __THROW_BAD_ALLOC;
    }
    (*__my_oom_handler)();
    __result = realloc(__p, __n);
    if (__result)
      return __result;
  }
}

typedef __malloc_alloc_template<0> malloc_alloc;

// simple wrapper _Alloc
template <class _Tp, class _Alloc> class simple_alloc {
public:
  static _Tp *allocate(size_t __n) {
    return __n ? (_Tp *)_Alloc::allocate(__n * sizeof(_Tp)) : 0;
  }
  static _Tp *allocate(void) { return (_Tp *)_Alloc::allocate(sizeof(_Tp)); }

  static void deallocate(_Tp *__p, size_t __n) {
    if (__n != 0)
      _Alloc::deallocate(__p, __n * sizeof(_Tp));
  }
  static void deallocate(_Tp *__p) { _Alloc::deallocate(__p); }
};

// allocator adaptor to check size arguments for debuging.
template <class _Alloc> class debug_alloc {
private:
  enum { _S_extra = sizeof(size_t) };

public:
  static void *allocate(size_t __n) {
    char *__result = (char *)(_Alloc::allocate(__n + (int)_S_extra));
    *(size_t *)__result = __n;
    return (void *)(__result + (int)_S_extra);
  }

  static void deallocate(void *__p, size_t __n) {
    char *__real_p = (char *)__p - (int)_S_extra;
    assert(*(size_t *)__real_p == __n);
    _Alloc::deallocate(__real_p, __n);
  }

  static void *reallocate(void *__p, size_t __old_sz, size_t __new_sz) {
    char *__real_p = (char *)__p - (int)_S_extra;
    assert(*(size_t *)__real_p == __old_sz);
    char *__result = _Alloc::reallocate(__real_p, __old_sz + (int)_S_extra,
                                        __new_sz + (int)_S_extra);
    *(size_t *)__result = __new_sz;
    return __result + (int)_S_extra;
  }
};

// default node  allocator
template <bool threads, int inst> class __default_alloc_template {
private:
  enum { _ALIGN = 8 };
  enum { _MAX_BYTES = 0x80 };
  enum { _NFREELISTS = 0x10 }; // _MAX_BYTES/_ALIGN

  static inline size_t _S_round_up(size_t __bytes) {
    return (((__bytes) + (size_t)(_ALIGN)-1) & ~((size_t)_ALIGN - 1));
  }

private:
  struct _Obj {
    _Obj *_M_free_list_link;
  };

  static _Obj *_S_free_list[_NFREELISTS];

  // get _bytes index in the _S_free_list.
  static inline size_t _S_free_list_index(size_t __bytes) {
    return (((__bytes) + (size_t)_ALIGN - 1) / _ALIGN - 1);
  }

  // refill the _S_free_list.
  static void *_S_refill(size_t __n);

  // allocate memory to memory_pool.
  // called by _S_refill.
  static void *_S_chunk_alloc(size_t __size, int &__nobjs);

  // pointer to memory poll.
  static char *_S_start_free;
  static char *_S_end_free;
  // the size of memory poll's memory holding.
  static size_t _S_heap_size;

  class Lock {
  public:
    Lock() { __NODE_ALLOCATOR_LOCK; }
    ~Lock() { __NODE_ALLOCATOR_UNLOCK; }
  };

public:
  static void *allocate(size_t __n) {
    void *__ret = 0;
    if (__n > _MAX_BYTES) {
      __ret = malloc_alloc::alloc(__n);
    } else {
      _Obj **__my_free_list = _S_free_list + _S_free_list_index(__n);
      Lock __lock_instance;
      if (*__my_free_list) {
        __ret = *__my_free_list;
        *__my_free_list = (*__my_free_list)->_M_free_list_link;
      } else {
        __ret = _S_refill(_S_round_up(__n));
      }
    }
    return __ret;
  }

  static void deallocate(void *__p, size_t __n) {
    if (__n > _MAX_BYTES) {
      malloc_alloc::deallocate(__p, __n);
    } else {
      _Obj **__my_free_list = _S_free_list + _S_free_list_index(__n);
      _Obj *__q = (_Obj *)__p;
      Lock __lock_instance;
      __q->_M_free_list_link = *__my_free_list;
      *__my_free_list = __q;
    }
  }

  static void *reallocate(void *__p, size_t __old_sz, size_t __new_sz);
};

typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;

template <bool __threads, int __inst>
inline bool operator==(const __default_alloc_template<__threads, __inst> &,
                       const __default_alloc_template<__threads, __inst> &) {
  return true;
}

// alloc chunk to memory pool
template <bool __threads, int __inst>
void *
__default_alloc_template<__threads, __inst>::_S_chunk_alloc(size_t __size,
                                                            int &__nobjs) {
  void *__ret = 0;
  size_t __total_bytes = __size * __nobjs;
  size_t __bytes_left = _S_end_free - _S_start_free;

  if (__bytes_left >= __total_bytes) {
    __ret = _S_start_free;
    _S_start_free += __total_bytes;
    return __ret;
  }

  if (__bytes_left >= __size) {
    __nobjs = (int)__bytes_left / __size;
    __total_bytes = __size * __nobjs;
    __ret = _S_start_free;
    _S_start_free += __total_bytes;
    return __ret;
  }

  // left memory to free_list
  if ((int)__bytes_left > 0) {
    _Obj **__my_free_list = _S_free_list + _S_free_list_index(__bytes_left);
    _Obj *__ptr = (_Obj *)_S_start_free;
    __ptr->_M_free_list_link = *__my_free_list;
    *__my_free_list = __ptr;
  }

  // from malloc get memory to pool
  size_t __bytes_to_get = __total_bytes * 2 + _S_round_up(_S_heap_size >> 4);
  _S_start_free = (char *)malloc(__bytes_to_get);
  if (!_S_start_free) {
    for (size_t __i = __size; __i <= (size_t)_MAX_BYTES;
         __i += (size_t)_ALIGN) {
      _Obj **__my_free_list = _S_free_list + _S_free_list_index(__i);
      if (*__my_free_list) {
        _Obj *__ptr = *__my_free_list;
        *__my_free_list = __ptr->_M_free_list_link;
        _S_start_free = (char *)__ptr;
        _S_end_free = _S_start_free + __i;
        return _S_chunk_alloc(__size, __nobjs);
      }
    }

    // throw bad alloc
    malloc_alloc::alloc(__size);
  }
  _S_end_free = _S_start_free + __bytes_to_get;
  _S_heap_size += __bytes_to_get;
  return _S_chunk_alloc(__size, __nobjs);
}

// refill the free_list.
template <bool __threads, int __inst>
void *__default_alloc_template<__threads, __inst>::_S_refill(size_t __size) {
  int __nobjs = 20;
  void *__ret = _S_chunk_alloc(__size, __nobjs);

  if (__nobjs == 1)
    return __ret;

  _Obj **__my_free_list = _S_free_list + _S_free_list_index(__size);
  _Obj *__ptr = (_Obj *)((char *)__ret + __size);
  for (int __i = 1; __i < __nobjs; __i++) {
    __ptr->_M_free_list_link = *__my_free_list;
    *__my_free_list = __ptr;
    __ptr = (_Obj *)((char *)__ptr + __size);
  }
  return __ret;
}

template <bool __threads, int __inst>
typename __default_alloc_template<__threads, __inst>::_Obj
    *__default_alloc_template<__threads, __inst>::_S_free_list
        [__default_alloc_template<__threads, __inst>::_NFREELISTS] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

template <bool __threads, int __inst>
char *__default_alloc_template<__threads, __inst>::_S_start_free = 0;
template <bool __threads, int __inst>
char *__default_alloc_template<__threads, __inst>::_S_end_free = 0;
template <bool __threads, int __inst>
size_t __default_alloc_template<__threads, __inst>::_S_heap_size = 0;

// cpp standard library allocator.
template <class _Tp, class _Alloc = alloc> class allocator {
  //   typedef alloc _Alloc;
  // here in `sgistl` is specified `alloc`,
  // I choose to use the template and do a layer of wrapping.
private:
  // encapsulating non-standard allocator
  _Alloc __underlying_allocator;

public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp *pointer;
  typedef const _Tp *const_pointer;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;
  typedef _Tp value_type;

  template <class _Tp1> struct rebind { typedef allocator<_Tp1> other; };

  // constructor
  allocator() {}
  allocator(const allocator &__a)
      : __underlying_allocator(__a.__underlying_allocator) {}
  template <class _U>
  allocator(const allocator<_U> &__a)
      : __underlying_allocator(__a.__underlying_allocator) {}
  // destructor
  ~allocator() {}

  // address
  pointer address(reference __x) { return &__x; }
  const_pointer address(const_reference __x) { return &__x; }

  _Tp *allocate(size_type __n, const void * = 0) {
    return __n != 0 ? static_cast<_Tp *>(
                          __underlying_allocator.allocate(__n * sizeof(_Tp)))
                    : 0;
  }

  void deallocate(pointer __p, size_type __n) {
    if (__p)
      __underlying_allocator.deallocate(__p, __n * sizeof(_Tp));
  }

  size_t max_size() { return ((size_t)-1) / sizeof(_Tp); }

  void construct(pointer __p, const_reference __value) {
    new (__p) _Tp(__value);
  }

  void destruct(pointer __p) { __p->~_Tp(); }
};

template <> class allocator<void> {
public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef void *pointer;
  typedef const void *const_pointer;
  typedef void value_type;

  template <class _Tp1> struct rebind { typedef allocator<_Tp1> other; };
};

template <class _Tp1, class _Tp2>
bool operator==(const allocator<_Tp1> &__a1, const allocator<_Tp2> &__a2) {
  return __a1.__underlying_allocator == __a2.__underlying_allocator;
}

template <class _Tp1, class _Tp2>
bool operator!=(const allocator<_Tp1> &__a1, const allocator<_Tp2> &__a2) {
  return __a1.__underlying_allocator != __a2.__underlying_allocator;
}

template <int inst>
inline bool operator==(const __malloc_alloc_template<inst> &,
                       const __malloc_alloc_template<inst> &) {
  return true;
}

template <class _Alloc>
inline bool operator==(const debug_alloc<_Alloc> &,
                       const debug_alloc<_Alloc> &) {
  return true;
}

/*
allocator adaptor
  _Alloc_traits{

    bool _S_instanceless
      false:
        need instance
        typedef : allocator_type
          allocactor_type().allocate(size)
      true:
        dont nedd intsance
        typedef : _Alloc_type
          _Alloc_type::allocate(size)
  }
*/

// all
template <class _Tp, class _Allocator> struct _Alloc_traits {
  static const bool _S_instanceless = false;
  typedef typename _Allocator::template rebind<_Tp>::other allocator_type;
};

// allocator
template <class _Tp, class _Tp1> struct _Alloc_traits<_Tp, allocator<_Tp1>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, alloc> _Alloc_type;
  typedef allocator<_Tp1> allocator_type;
};

// __malloc_alloc_template
template <class _Tp, int __inst>
struct _Alloc_traits<_Tp, __malloc_alloc_template<__inst>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __malloc_alloc_template<__inst>> _Alloc_type;
  typedef allocator<_Tp, __malloc_alloc_template<__inst>> allocator_type;
};

// __default_alloc_template
template <class _Tp, bool __threads, int __inst>
struct _Alloc_traits<_Tp, __default_alloc_template<__threads, __inst>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __default_alloc_template<__threads, __inst>>
      _Alloc_type;
  typedef allocator<_Tp, __default_alloc_template<__threads, __inst>>
      allocator_type;
};

// debug_alloc
template <class _Tp, class _Alloc>
struct _Alloc_traits<_Tp, debug_alloc<_Alloc>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, debug_alloc<_Alloc>> _Alloc_type;
  typedef allocator<_Tp, debug_alloc<_Alloc>> allocator_type;
};

// allocator(__malloc_alloc_template)
template <class _Tp, class _Tp1, int __inst>
struct _Alloc_traits<_Tp, allocator<_Tp1, __malloc_alloc_template<__inst>>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __malloc_alloc_template<__inst>> _Alloc_type;
  typedef allocator<_Tp, __malloc_alloc_template<__inst>> allocator_type;
};

// allocator(__default_alloc_template)
template <class _Tp, class _Tp1, bool __thr, int __inst>
struct _Alloc_traits<_Tp,
                     allocator<_Tp1, __default_alloc_template<__thr, __inst>>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __default_alloc_template<__thr, __inst>>
      _Alloc_type;
  typedef allocator<_Tp, __default_alloc_template<__thr, __inst>>
      allocator_type;
};

// allocator(debug_alloc)
template <class _Tp, class _Tp1, class _Alloc>
struct _Alloc_traits<_Tp, allocator<_Tp1, debug_alloc<_Alloc>>> {
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, debug_alloc<_Alloc>> _Alloc_type;
  typedef allocator<_Tp, debug_alloc<_Alloc>> allocator_type;
};

// to container

template <class _Tp, class _Alloc, bool _isStatic> class _alloc_base_aux {
public:
  typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
  allocator_type get_allocator() const { return _M_data_allocator; }

  _alloc_base_aux(const allocator_type &__a) : _M_data_allocator(__a) {}

  allocator_type _M_data_allocator;

  _Tp *_M_allocate(size_t __n) { return _M_data_allocator.allocate(__n); }
  void _M_deallocate(_Tp *__p, size_t __n) {
    if (__p)
      _M_data_allocator.deallocate(__p, __n);
  }
};

template <class _Tp, class _Alloc> class _alloc_base_aux<_Tp, _Alloc, true> {

public:
  typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
  typedef typename _Alloc_traits<_Tp, _Alloc>::_Alloc_type _Alloc_type;
  allocator_type get_allocator() const { return allocator_type(); }
  _alloc_base_aux(const allocator_type &__a) {}

  _Tp *_M_allocate(size_t __n) { return _Alloc_type::allocate(__n); }
  void _M_deallocate(_Tp *__p, size_t __n) {
    if (__p)
      _Alloc_type::deallocate(__p, __n);
  }
};

STL_END_NAMESPACE

#endif // __TINYSTL_STL_ALLOC__