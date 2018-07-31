#pragma once
#include <cstdlib>
#include <utility>
#include <exception>

template <typename T, size_t number = 0>
class UserAllocator{
public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = UserAllocator<U>;
    };

    UserAllocator() = default;
    ~UserAllocator() = default;

    template<typename U>
    UserAllocator(const UserAllocator<U>&) {

    }

    T* allocate(std::size_t n)
    {        
        if(number)
            m_chunk_capacity = number;
        else
            m_chunk_capacity = n;

        free_chunks = m_chunk_capacity;
        auto p = malloc(m_chunk_capacity*sizeof (value_type));
        if(!p)
            throw std::bad_alloc();
        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(T* p, std::size_t n)
    {
        free(p);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U(std::forward(args)...);
        free_chunks++;
    }

    void destroy(T* p)
    {
        p->~value_type();
        free_chunks--;
    }
private:
    size_t m_chunk_capacity = 0;
    size_t free_chunks = 0;

};

