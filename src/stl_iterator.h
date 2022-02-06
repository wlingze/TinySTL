#ifndef __TINYSTL_ITERATOR__
#define __TINYSTL_ITERATOR__

#include "stl_config.h"
#include "stl_iterator_base.h"
STL_BEGIN_NAMESPACE

template <class _Container> class insert_iterator {
protected:
  _Container *container;
  typename _Container::iterator_type iter;

public:
  typedef _Container container_type;
  typedef output_iterator iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  insert_iterator(_Container &__c, typename _Container::iterator_type __i)
      : container(__c), iter(__i) {}
  insert_iterator<_Container>
  operator=(const typename _Container::value_type &__v) {
    iter = container->insert(iter, __v);
    iter++;
    return *this;
  }
  insert_iterator<_Container> operator*() { return *this; }
  insert_iterator<_Container> operator++() { return *this; }
  insert_iterator<_Container> operator++(int) { return *this; }
};

template <class _Container, class _Iterator>
insert_iterator<_Container> inserter(_Container &__c, _Iterator __i) {
  typedef typename _Container::iterator_type iter;
  return insert_iterator<_Container>(__c, iter(__i));
}

template <class _Iterator> class reverse_iterator {
protected:
  _Iterator current;

public:
  /* iterator traits */
  typedef
      typename iterator_traits<_Iterator>::iterator_category iterator_category;
  typedef typename iterator_traits<_Iterator>::difference_type difference_type;
  typedef typename iterator_traits<_Iterator>::value_type value_type;
  typedef typename iterator_traits<_Iterator>::pointer pointer;
  typedef typename iterator_traits<_Iterator>::reference reference;

  /* typedef */
  typedef _Iterator iterator_type;
  typedef reverse_iterator<_Iterator> self;

  /* construct */
  reverse_iterator(){};
  explicit reverse_iterator(iterator_type __i) : current(__i){};
  reverse_iterator(const self &__r) : current(__r.current){};

  /* function */
  iterator_type base() const { return current; }

  /* operator */
  reference operator*() {
    iterator_type tmp = current;
    return *--tmp;
  }

  self &operator++() {
    current--;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    current--;
    return this;
  }

  self &operator--() {
    current++;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    current++;
    return this;
  }

  self operator+(difference_type __n) const { return self(current - __n); }
  self operator-(difference_type __n) const { return self(current + __n); }
  self &operator+=(difference_type __n) {
    current -= __n;
    return *this;
  }
  self &operator-=(difference_type __n) {
    current += __n;
    return *this;
  }

  reference operator[](difference_type __n) const { return *(*this + __n); }
};

/* cmp */
template <class _Iterator>
inline bool operator==(const reverse_iterator<_Iterator> &__x,
                       const reverse_iterator<_Iterator> &__y) {
  return __x.base() == __y.base();
}

template <class _Iterator>
inline bool operator<(const reverse_iterator<_Iterator> &__x,
                      const reverse_iterator<_Iterator> &__y) {
  return __x.base() < __y.base();
}

template <class _Iterator>
inline bool operator>(const reverse_iterator<_Iterator> &__x,
                      const reverse_iterator<_Iterator> &__y) {
  return __y < __x;
}

template <class _Iterator>
inline bool operator!=(const reverse_iterator<_Iterator> &__x,
                       const reverse_iterator<_Iterator> &__y) {
  return !(__x == __y);
}

template <class _Iterator>
inline bool operator>=(const reverse_iterator<_Iterator> &__x,
                       const reverse_iterator<_Iterator> &__y) {
  return !(__x < __y);
}

template <class _Iterator>
inline bool operator<=(const reverse_iterator<_Iterator> &__x,
                       const reverse_iterator<_Iterator> &__y) {
  return !(__x > __y);
}

template <class _Iterator>
reverse_iterator<_Iterator>
operator+(typename reverse_iterator<_Iterator>::difference_type n,
          const reverse_iterator<_Iterator> &i) {
  return reverse_iterator<_Iterator>(i.base() - n);
}

STL_END_NAMESPACE

#endif // __TINYSTL_ITERATOR__