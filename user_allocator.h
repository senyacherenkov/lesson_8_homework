#pragma once
#include <cstdlib>
#include <utility>
#include <exception>
#include <set>
#include <cmath>

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
        if(!m_currentPointer && chunkCapacity)
            quantity  = chunkCapacity;
        else
            quantity  = n;

        if((quantity *sizeof (value_type) <= (chunkCapacity*sizeof (value_type) - m_offset)) && m_currentPointer)
            return reinterpret_cast<value_type*>(reinterpret_cast<uint8_t*>(m_currentPointer) + m_offset);

        auto p = malloc(quantity *sizeof (value_type));
        if(!p)
            throw std::bad_alloc();        

        m_offset = 0;
        m_currentPointer = p;

        m_controlBlock.insert(m_currentPointer);
        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(value_type* p, std::size_t n)
    {
        auto result = m_controlBlock.find(p);
        if(result == m_controlBlock.end())
            return;
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
    std::set<void*> m_controlBlock;
    std::size_t m_offset = 0;
    void* m_currentPointer = nullptr;
};

