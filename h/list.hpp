#ifndef _list_hpp_
#define _list_hpp_

#include "../h/MemoryAllocator.hpp"

// Napomena: Klasa je u potpunosti preuzeta sa 7. vezbi - sinhrona promena konteksta.

template<typename T> class List {
private:
    struct Elem {
        T *data;
        Elem *next;
        Elem(T *data, Elem *next) : data(data), next(next) {}

        void* operator new(size_t size) {                                       // Preklopljeni operator new za Elem.
            uint64 adjSize = 0;
            if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;
            else adjSize = (size / MEM_BLOCK_SIZE) + 1;
            return MemoryAllocator::get()->mem_alloc(adjSize);
        }

        void operator delete(void* ptr) {                                       // Preklopljeni operator delete za Elem.
            MemoryAllocator::get()->mem_free(ptr);
        }
    };

    Elem *head, *tail;

public:
    void* operator new(size_t size) {                                           // Preklopljeni operator new za List.
        uint64 adjSize = 0;
        if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;
        else adjSize = (size / MEM_BLOCK_SIZE) + 1;
        return MemoryAllocator::get()->mem_alloc(adjSize);
    }

    void operator delete(void* ptr) {                                           // Preklopljeni operator delete za List.
        MemoryAllocator::get()->mem_free(ptr);
    }

    List() : head(nullptr), tail(nullptr) {}
    List(const List<T> &) = delete;
    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data) {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) tail = head;
    }

    void addLast(T *data) {
        Elem *elem = new Elem(data, nullptr);
        if (tail) {
            tail->next = elem;
            tail = elem;
        }
        else {
            head = tail = elem;
        }
    }

    T* removeFirst() {
        if (!head) return nullptr;

        Elem *elem = head;
        head = head->next;
        if (!head) tail = nullptr;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekFirst() {
        if (!head) return nullptr;
        return head->data;
    }

    T* removeLast() {
        if (!head) return nullptr;

        Elem *prev = nullptr;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next) prev = curr;

        Elem *elem = tail;
        if (prev) prev->next = nullptr;
        else head = nullptr;
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekLast() {
        if (!tail) return 0;
        return tail->data;
    }
};

#endif
