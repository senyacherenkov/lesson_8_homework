#pragma once
#include <cstdlib>
#include <utility>
#include <exception>
#include <map>
#include <utility>
#include <iostream>
#include <cstring>
#include <stdint.h>

namespace  {
    constexpr size_t MIN_RESERVE_MEMORY = 5;
    constexpr size_t MEMORY_MULTIPLIER = 2;
}

template <typename T, std::size_t number>
class UserAllocator{
public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = UserAllocator<U, number>;
    };

    UserAllocator() = default;
    ~UserAllocator() = default;

    template<typename U>
    UserAllocator(const UserAllocator<U, number>&) {

    }

    value_type* allocate(std::size_t n)
    {        
        size_t quantity = 0;
        if(!m_currentPointer && number)
            quantity  = number;
        else
            quantity  = n;

        if((quantity *sizeof (value_type) < (number*sizeof (value_type) - m_offset)) && m_currentPointer)
            return reinterpret_cast<value_type*>(reinterpret_cast<uint8_t*>(m_currentPointer) + m_offset);

        if(m_currentPointer && m_offset) {
            auto p = reallocate(MEMORY_MULTIPLIER*number);
            return reinterpret_cast<value_type*>(reinterpret_cast<uint8_t*>(p) + m_offset);
        }

        auto p = malloc(quantity *sizeof (value_type));
        if(!p)
            throw std::bad_alloc();        

        m_currentPointer = p;

        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(value_type* p, std::size_t n)
    {
        if(p == m_currentPointer)
            m_currentPointer = nullptr;
        free(p);        
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {     
        std::size_t diff = reinterpret_cast<uint8_t*>(p) - reinterpret_cast<uint8_t*>(m_currentPointer) - m_offset;
        if(diff)
            m_offset += diff + sizeof(U);
        else if(diff == 0)
            m_offset += sizeof(U);

        new (p) U(std::forward<Args>(args)...);        
    }

    void destroy(value_type* p)
    {
        p->~value_type();
        m_offset -= sizeof (value_type);
    }

    void reserve(std::size_t n) {
        if(m_offset/sizeof (value_type) < n)
            reallocate(n);
    }
private:
    void* reallocate(std::size_t n) {
        auto p = malloc(n*sizeof (value_type));
        if(!p)
            throw std::bad_alloc();
        memcpy(p, m_currentPointer, m_offset);
        m_currentPointer = p;
        return m_currentPointer;
    }

private:

    std::size_t m_offset = 0;
    void* m_currentPointer = nullptr;
};

