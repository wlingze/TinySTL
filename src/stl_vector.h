#ifndef __TINYSTL_VECTOR__
#define __TINYSTL_VECTOR__

#include "stl_algobase.h"
#include "stl_alloc.h"
#include "stl_config.h"
#include "stl_construct.h"
#include "stl_iterator.h"
#include "stl_iterator_base.h"
#include "stl_uninitialized.h"
#include "type_traits.h"
#include <cstddef>
#include <type_traits>
STL_BEGIN_NAMESPACE

template <class _Tp, class _Alloc> class vector_base {

public:
  typedef _Tp *pointer;
  typedef _alloc_base_aux<_Tp, _Alloc,
                          _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
      _alloc;
  typedef typename _alloc::allocator_type allocator_type;

  allocator_type get_allocator() const { return _M_allocator.get_allocator(); }

  vector_base(const allocator_type &__a)
      : _M_allocator(__a), _M_start(0), _M_finish(0), _M_end_of_storage(0) {}

  vector_base(size_t __n, const allocator_type &__a) : _M_allocator(__a) {
    _M_start = _M_allocate(__n);
    _M_end_of_storage = _M_start + __n;
    _M_finish = _M_start;
  }

  ~vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }

protected:
  _alloc _M_allocator;
  pointer _M_start;
  pointer _M_finish;
  pointer _M_end_of_storage;

  _Tp *_M_allocate(size_t __n) { return _M_allocator._M_allocate(__n); }
  void _M_deallocate(_Tp *__p, size_t __n) {
    if (__p)
      _M_allocator._M_deallocate(__p, __n);
  }
};

template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp)>
class vector : public vector_base<_Tp, _Alloc> {

  /* typedef */
private:
  typedef vector_base<_Tp, _Alloc> _base;

public:
  /* traits */
  typedef _Tp value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;
  typedef value_type &reference;
  typedef const value_type &const_reference;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef vector<_Tp, _Alloc> self;
  typedef vector<_Tp, _Alloc> &self_reference;
  typedef const vector<_Tp, _Alloc> const_self_reference;

  /* allocator */
  typedef typename _base::allocator_type allocator_type;
  allocator_type get_allocator() const { return _base::get_allocator(); }
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;

protected:
  using _base::_M_allocate;
  using _base::_M_deallocate;
  using _base::_M_end_of_storage;
  using _base::_M_finish;
  using _base::_M_start;

public:
  /* basic function */

  iterator begin() { return _M_start; }
  iterator end() { return _M_finish; }
  const_iterator begin() const { return _M_start; }
  const_iterator end() const { return _M_finish; }

  reverse_iterator rbegin() { return reverse_iterator(begin()); }
  reverse_iterator rend() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator rend() const { return const_reverse_iterator(end()); }

  size_type size() const { return size_type(end() - begin()); }
  size_type max_size() const { return size_type(-1) / sizeof(_Tp); }
  size_type capacity() const { return size_type(_M_end_of_storage - begin()); }
  bool empty() const { return begin() == end(); }

  reference front() { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference front() const { return *begin(); }
  const_reference back() const { return *(end() - 1); }

  reference at(size_type __n) {
    _M_range_check(__n);
    return (*this)[__n];
  }
  const_reference at(size_type __n) const {
    _M_range_check(__n);
    return (*this)[__n];
  }

  /* operator */
  reference operator[](size_type __n) { return *(begin() + __n); }
  const_reference operator[](size_type __n) const { return *(begin() + __n); }

  /* construct */
  explicit vector(const allocator_type &__a = allocator_type()) : _base(__a) {}
  vector(size_type __n, const value_type &__v,
         const allocator_type &__a = allocator_type())
      : _base(__n, __a) {
    _M_finish = uninitialized_fill_n(_M_start, __n, __v);
  }

  explicit vector(size_type __n) : _base(__n, allocator_type()) {
    _M_finish = uninitialized_fill_n(_M_start, __n, value_type());
  }

  vector(const vector<_Tp, _Alloc> &__x)
      : _base(__x.size(), __x.get_allocator()) {
    _M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start);
  }

  // if _InputIterator is integral, this is not a iterator
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last,
         const allocator_type &__a = allocator_type())
      : _base(__a) {
    typedef typename _Is_integer<_InputIterator>::_Integral _is_integral;
    _M_initialize_aux(__first, __last, _is_integral());
  }

  ~vector() { destory(_M_start, _M_finish); }

  /* operator =, assignment construct */
  self_reference operator=(const_self_reference __v) {
    if (&__v != this) {
      size_type _new_size = __v.size();
      if (_new_size > capacity()) {
        iterator _new_start =
            _M_allocate_copy(_new_size, __v.begin(), __v.end());
        destory(begin(), end());
        _M_deallocate(_M_start, _M_end_of_storage - _M_start);
        _M_start = _new_start;
        _M_end_of_storage = _M_start + _new_size;
      } else if (_new_size <= size()) {
        iterator _i = copy(__v.begin(), __v.end(), _M_start);
        destory(_i, _M_finish);
      } else {
        copy(__v.begin(), __v.begin() + size(), _M_start);
        uninitialized_copy(__v.begin() + size(), __v.end(), _M_finish);
      }
      _M_finish = _M_start + _new_size;
    }
    return *this;
  }

  /* reserve */
  void reserve(size_type __n) {
    if (capacity() < __n) {
      size_type old_size = size();
      iterator _new_start = _M_allocate_copy(__n, _M_start, _M_finish);
      destory(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = _new_start;
      _M_finish = _M_start + old_size;
      _M_end_of_storage = _M_start + __n;
    }
  }

  /* assign */
  void assign(size_type __n, const_reference __v) { _M_assign_aux(__n, __v); }
  template <class _InputIter>
  void assign(_InputIter __first, _InputIter __last) {
    typedef typename _Is_integer<_InputIter>::_Integral _is_interger;
    _M_assign_dispatch(__first, __last, _is_interger());
  }

  /* swap */
  void swap(self_reference __v) {
    STL_NAMESPACE::swap(_M_start, __v._M_start);
    STL_NAMESPACE::swap(_M_finish, __v._M_finish);
    STL_NAMESPACE::swap(_M_end_of_storage, __v._M_end_of_storage);
  }

  /* push */
  void push_back(const_reference __v = value_type()) {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish, __v);
      _M_finish++;
    } else {
      _M_push_back_aux(__v);
    }
  }

  /* insert */
  iterator insert(iterator __position, const_reference __v = value_type()) {
    size_type distance = __position - _M_start;
    if (_M_finish == _M_end_of_storage) {
      _M_insert_aux(__position, __v);
    } else if (__position == _M_finish) {
      push_back(__v);
    } else {
      construct(_M_finish, *(_M_finish - 1));
      _M_finish++;
      copy(__position, _M_finish, __position + 1);
      *__position = __v;
    }
    return _M_start + distance;
  }

  template <class _InputIter>
  void insert(iterator __position, _InputIter __first, _InputIter __last) {
    typedef typename _Is_integer<_InputIter>::_Integral _is_integral;
    _M_insert_dispatch(__position, __first, __last, _is_integral());
  }

  /* pop */
  void pop_back() {
    _M_finish--;
    destory(_M_finish);
  }

  /* erase */
  iterator erase(iterator __first, iterator __last) {
    iterator __i = copy(__last, _M_finish, __first);
    destory(__i, _M_finish);
    _M_finish = _M_finish - (__last - __first);
    return __first;
  }

  iterator erase(iterator __p) {
    if (__p + 1 != end())
      copy(__p + 1, _M_finish, __p);
    _M_finish--;
    destory(_M_finish);
    return __p;
  }

  void clear() { erase(begin(), end()); }

private:
  void _M_range_check(size_type __n) const {
    if (__n >= size()) {
      throw;
      // TODO: implemention the exception section.
      // here a throw outside catch structure bring the program to an end.
    }
  }

  template <class _Integral>
  void _M_initialize_aux(_Integral __n, _Integral __v, __true_type) {
    _M_start = _M_allocate(__n);
    _M_end_of_storage = _M_start + __n;
    _M_finish = uninitialized_fill_n(_M_start, __n, __v);
  }

  template <class _InputIterator>
  void _M_initialize_aux(_InputIterator __first, _InputIterator __last,
                         __false_type) {
    for (; __first != __last; __first++)
      push_back(*__first);
  }

  void _M_push_back_aux(const_reference _v) {
    size_type _old_size = size();
    size_type _new_size = _old_size ? _old_size * 2 : 1;
    iterator _new_start = _M_allocate(_new_size);
    iterator _new_finish = _new_start;
    __STL_TRY {
      _new_finish = uninitialized_copy(_M_start, _M_finish, _new_start);
      construct(_new_finish, _v);
      _new_finish++;
    }
    __STL_UNWIND(destory(_new_start, _new_finish);
                 _M_deallocate(_new_start, _new_size);)
    destory(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = _new_start;
    _M_finish = _new_finish;
    _M_end_of_storage = _new_start + _new_size;
  }

  iterator _M_allocate_copy(size_type __n, const_iterator __first,
                            const_iterator __last) {
    iterator _new = _M_allocate(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _new);
      return _new;
    }
    __STL_UNWIND(_M_deallocate(_new, __n);)
  }

  void _M_insert_aux(iterator __p, const_reference __v) {
    size_type _old_size = size();
    size_type _new_size = _old_size ? _old_size * 2 : 1;
    iterator _new_start = _M_allocate(_new_size);
    iterator _new_finish = _new_start;
    __STL_TRY {
      _new_finish = uninitialized_copy(_M_start, __p, _new_start);
      construct(_new_finish, __v);
      _new_finish++;
      _new_finish = uninitialized_copy(__p, _M_finish, _new_finish);
    }
    __STL_UNWIND(destory(_new_start, _new_finish);
                 _M_deallocate(_new_start, _new_size);)
    destory(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = _new_start;
    _M_finish = _new_finish;
    _M_end_of_storage = _new_start + _new_size;
  }

  template <class _Integer>
  void _M_insert_dispatch(iterator __p, _Integer __n, _Integer __v,
                          __true_type) {
    _M_fill_insert(__p, (size_type)__n, (value_type)__v);
  }

  template <class _InputIter>
  void _M_insert_dispatch(iterator __p, _InputIter __first, _InputIter __last,
                          __false_type) {
    _M_range_insert(__p, __first, __last);
  }

  /*
    [start : __p : finish)--end], -> [start:__v*__n:finish)--end]
  */
  void _M_fill_insert(iterator __p, size_type __n, const_reference __v) {
    if (!__n)
      return;

    if ((_M_end_of_storage - _M_finish) > __n) {
      // sufficient memory
      const size_type _distance = _M_finish - __p;

      if (_distance > __n) {
        /*
          [start, fill copy |finish| uninitialize_copy]
          p->p+n->f->f+n
            f, f+n  <=  f-n, f  u_copy
            p+n, f  <=  p, f-n  copy
            p, p+n              fill

        */
        uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
        copy(__p, _M_finish - __n, __p + __n);
        _M_finish += __n;
        fill_n(__p, __n, __v);
      } else {
        /*
          [start, fill,|finish| uninitialize_fill,  uninitialize_copy]

           p->f->p+n->f+n
            p+n, f+n <= p, f u_copy
            f, p+n           u_fill_n
            p, f             fill_n
        */
        uninitialized_copy(__p, _M_finish, __p + __n);
        uninitialized_fill_n(_M_finish, __n, __v);
        _M_finish += __n;
        fill_n(__p, __n, __v);
      }
    } else {
      // insufficient memory, allocate memory again.

      /*
        s->p->f
        s->i . i->f
      */
      size_type _old_size = size();
      size_type _new_size = _old_size + max(_old_size, __n);
      iterator _new_start = _M_allocate(_new_size);
      iterator _new_finish = _new_start;
      __STL_TRY {
        _new_finish = uninitialized_copy(_M_start, __p, _new_start);
        _new_finish = uninitialized_fill_n(_new_finish, __n, __v);
        _new_finish = uninitialized_copy(__p, _M_finish, _new_finish);
      }
      __STL_UNWIND(destory(_new_start, _new_finish);
                   _M_deallocate(_new_start, _new_size);)
      destory(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = _new_start;
      _M_finish = _new_finish;
      _M_end_of_storage = _new_start + _new_size;
    }
  }

  void _M_range_insert(iterator __p, iterator __first, iterator __last) {
    if (__first == __last)
      return;
    if ((_M_end_of_storage - _M_start) > (__last - __first)) {
      const size_type _n = __last - __first;
      const size_type _distance = _M_finish - __p;
      if (_distance > _n) {
        /*
          s p p+n f f+n
        */
        uninitialized_copy(_M_finish - _n, _M_finish, _M_finish);
        copy(__p, _M_finish - _n, __p + _n);
        copy(__first, __last, __p);
        _M_finish += _n;
      } else {
        /*
          s p f p+n f+n
        */
        uninitialized_copy(__p, _M_finish, __p + _n);
        copy(__first, __first + _distance, __p);
        uninitialized_copy(__first + _distance, __last, __p);
        _M_finish += _n;
      }
    } else {
      size_type _n = __last - __first;
      size_type _old_size = size();
      size_type _new_size = _old_size + max(_old_size, _n);
      iterator _new_start = _M_allocate(_new_size);
      iterator _new_finish = _new_start;
      __STL_TRY {
        _new_finish = uninitialized_copy(_M_start, __p, _new_start);
        _new_finish = uninitialized_copy(__first, __last, _new_finish);
        _new_finish = uninitialized_copy(__p, _M_finish, _new_finish);
      }
      __STL_UNWIND(destory(_new_start, _new_finish);
                   _M_deallocate(_new_start, _new_size);)
      _M_start = _new_start;
      _M_finish = _new_finish;
      _M_end_of_storage = _M_start + _new_size;
    }
  }

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __v, __true_type) {
    _M_assign_aux((size_type)__n, (value_type)__v);
  }

  template <class _InputIter>
  void _M_assign_dispatch(_InputIter __first, _InputIter __last, __false_type) {
    size_type __len = 0;
    __len = distance(__first, __last);

    if (__len > capacity()) {
      iterator tmp = _M_allocate_copy(__len, __first, __last);
      destory(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = tmp;
      _M_end_of_storage = _M_finish = _M_start + __len;
    } else if (__len < size()) {
      iterator _new_finish = copy(__first, __last, _M_start);
      destory(_new_finish, _M_finish);
      _M_finish = _new_finish;
    } else {
      iterator mid = __first;
      advance(mid, size());
      copy(__first, mid, _M_start);
      _M_finish = uninitialized_copy(mid, __last, _M_finish);
    }
  }

  void _M_assign_aux(size_type __n, const_reference __v) {
    if (__n > capacity()) {
      self tmp(__n, __v);
      tmp.swap(*this);
    } else if (__n < size()) {
      erase(fill_n(begin(), __n, __v), __n);
    } else {
      fill(begin(), end(), __v);
      _M_finish = uninitialized_fill_n(_M_finish, __n - size(), __v);
    }
  }
};

template <class _Tp, class _Alloc>
bool operator==(const vector<_Tp, _Alloc> &__x,
                const vector<_Tp, _Alloc> &__y) {
  return (__x.size() == __y.size()) &&
         (equal(__x.begin(), __x.end(), __y.begin()));
}

template <class _Tp, class _Alloc>
bool operator<(const vector<_Tp, _Alloc> &__x, const vector<_Tp, _Alloc> &__y) {
  return lexicographical_compare(__x.begin(), __x.end(), __y.begin(),
                                 __y.end());
}

STL_END_NAMESPACE
#endif // __TINYSTL_VECTOR__