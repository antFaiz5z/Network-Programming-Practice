//
// Created by Faiz on 2019/5/1 0001.
//

#ifndef MY_STL_LIST_H
#define MY_STL_LIST_H

template <class T>

struct __list_node{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

class list {

};


#endif //MY_STL_LIST_H
