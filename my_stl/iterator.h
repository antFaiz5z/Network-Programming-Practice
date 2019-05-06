//
// Created by faiz on 19-4-28.
//

#ifndef MY_STL_ITERATOR_H
#define MY_STL_ITERATOR_H

#include <cerrno>
//#include <bits/stl_iterator_base_types.h>

struct input_iterator_tag {
};
struct output_iterator_tag {
};
struct forward_iterator_tag : public input_iterator_tag, public output_iterator_tag {//???
};
struct bidirectional_iterator_tag : forward_iterator_tag {
};
struct random_access_iterator_tag : bidirectional_iterator_tag {
};


template<class Category,
        class T,
        class Distance = std::ptrdiff_t,
        class Pointer = T *,
        class Reference = T &>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

template<class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

template<class T>
struct iterator_traits<T *> {

};

template<class T>
struct iterator_traits<const T *> {

};

template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator &) {

}

template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type *difference_type(const Iterator &) {

}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type *value_type(const Iterator &) {

}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {

}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {

}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category category();
    return __distance(first, last, category());
}

template<class InputIterator, class Distance>
inline void __advance(InputIterator &i, Distance n, input_iterator_tag) {

}

template<class BidrectionalIterator, class Distance>
inline void __advance(BidrectionalIterator &i, Distance n, bidirectional_iterator_tag) {

}

template<class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag) {

}

template<class InputIterator, class Distance>
inline void advance(InputIterator &i, Distance n) {
    __advance(i, n, iterator_category(i));
}



#endif //MY_STL_ITERATOR_H
