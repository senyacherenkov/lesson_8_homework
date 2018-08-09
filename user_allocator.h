#pragma once
#include <cstdlib>
#include <utility>
#include <exception>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <stdint.h>

namespace {
    constexpr int MEM_MULTIPLIER = 2;
}

template <typename T, std::size_t chunkCapacity>
class UserAllocator{
public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = UserAllocator<U, chunkCapacity>;
    };

    UserAllocator() = default;
    ~UserAllocator() = default;

    template<typename U>
    UserAllocator(const UserAllocator<U, chunkCapacity>&) {

    }

    value_type* allocate(std::size_t n)
    {        
        size_t quantity = 0;
        size_t requestedBytes = 0;
        if(!m_currentPointer && chunkCapacity)
            quantity  = chunkCapacity;
        else
            quantity  = n;

        requestedBytes = quantity * sizeof (value_type);

        if((requestedBytes <= (chunkCapacity*sizeof (value_type) - m_offset)) && m_currentPointer)
        {
            auto pValidMemory = reinterpret_cast<value_type*>(reinterpret_cast<uint8_t*>(m_currentPointer) + m_offset);
            m_offset += requestedBytes;
            return pValidMemory;
        }

        auto p = malloc(requestedBytes);
        if(!p)
            throw std::bad_alloc();        

        m_offset = sizeof (value_type);
        m_currentPointer = p;

        m_storage.push_back(m_currentPointer);

        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(value_type* p, std::size_t n)
    {
        if(std::find(m_storage.begin(), m_storage.end(), p) == m_storage.end())
            return;
        if(p == m_currentPointer)
            m_currentPointer = nullptr;
        free(p);
        m_offset -= sizeof (value_type) * n;
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U(std::forward<Args>(args)...);        
    }

    void destroy(value_type* p)
    {
        p->~value_type();        
    }

    void reserve(std::size_t n) {
        if((chunkCapacity - m_offset/sizeof (value_type)) < n) {
            auto p = malloc(n <= chunkCapacity ? MEM_MULTIPLIER*chunkCapacity*sizeof (value_type):
                        std::ceil(static_cast<double>(n)/static_cast<double>(chunkCapacity))
                        *chunkCapacity*sizeof (value_type));
            if(!p)
                throw std::bad_alloc();
            memcpy(p, m_currentPointer, m_offset);
            free(m_currentPointer);
            m_currentPointer = p;
        }
    }

private:
    std::vector<void*> m_storage;
    std::size_t m_offset = 0;
    void* m_currentPointer = nullptr;
};

