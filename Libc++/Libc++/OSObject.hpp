//
//  OSObject.hpp
//  Libc++
//
//  Created by Adam Kopeć on 6/7/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef OSObject_hpp
#define OSObject_hpp

#include "OSRuntime.hpp"

// double linked list element
typedef struct dlelement dlelement_t;
struct dlelement {
    void*        data;
    dlelement_t* prev;
    dlelement_t* next;
};

typedef struct {
    dlelement_t* head;
    dlelement_t* tail;
} list_t;

#define      list_init() {0, 0}

static inline void list_append_elem(list_t* list, dlelement_t* elem) {
    elem->next = 0;
    elem->prev = list->tail;
    if (list->head == 0) {
        list->head = elem;
    } else {
        list->tail->next = elem;
    }
    list->tail = elem;
}

static inline dlelement_t* list_append(list_t* list, void* data) {
    dlelement_t* newElement = new dlelement_t();
    if (newElement) {
        newElement->data = data;
        list_append_elem(list, newElement);
    }
    return newElement;
}

class OSObject {
protected:
    virtual void release();
    //virtual void free();
    virtual OSObject* alloc();
    
public:
    virtual ~OSObject();
};

#endif /* OSObject_hpp */
