#pragma once
#include <cstdlib>
#include <utility>
#include <exception>
#include <map>
#include <utility>
#include <iostream>

namespace  {
    constexpr size_t MIN_RESERVE_MEMORY = 5;
}

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

    value_type* allocate(std::size_t n)
    {
        size_t quantity = 0;
        if(number)
            quantity  = number;
        else
            quantity  = n;

        auto p = malloc(quantity *sizeof (value_type));
        if(!p)
            throw std::bad_alloc();

        std::cout << "allocate: " << p << std::endl;

        size_t usedBytes = quantity *sizeof (value_type);
        m_chunkStore.emplace(p, ControllerBlock(usedBytes, usedBytes));
        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(value_type* p, std::size_t n)
    {
        auto result = m_chunkStore.find(p);
        if(result == m_chunkStore.end())
            return;

        m_chunkStore.erase(result);
        free(p);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        std::cout << "construct: " << p << std::endl;
        auto result = m_chunkStore.find(static_cast<void*>(p));
        if(result == m_chunkStore.end())
            return;

        if(result->second.m_freeBytes < sizeof(U))
            p = static_cast<U*>(static_cast<void*>(allocate(number? number : MIN_RESERVE_MEMORY)));

        new (p) U(std::forward<Args>(args)...);
        m_chunkStore.at(p).m_freeBytes-= sizeof (U);
    }

    void destroy(value_type* p)
    {
        auto result = m_chunkStore.find(p);
        if(result == m_chunkStore.end())
            return;
        result->second.m_freeBytes+= sizeof(value_type);
        p->~value_type();
    }
private:

    struct ControllerBlock {
        ControllerBlock() = default;
        ControllerBlock(size_t bytesCapacity, size_t freeBytes):
            m_bytesCapacity(bytesCapacity),
            m_freeBytes(freeBytes)
        {}

        size_t m_bytesCapacity = 0;
        size_t m_freeBytes = 0;
    };

    std::map<void*, ControllerBlock> m_chunkStore;
};

