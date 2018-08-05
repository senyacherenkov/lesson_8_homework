#pragma once
#include <vector>

namespace {
    constexpr int MIN_CAPACITY = 10;
    constexpr int MEM_MULTIPLIER = 2;
}

template <typename T, typename Alloc = std::allocator<T>>
class UserContainer {
public:
    Alloc local_allocator;
    UserContainer() {
        auto pointer = local_allocator.allocate(MIN_CAPACITY);
        m_data = reinterpret_cast<T*>(pointer);
    }

    struct iterator_ {
        T* m_data = nullptr;
        std::size_t m_pointer = 0;

        iterator_(T* data):
            m_data(data),
            m_pointer(0)
        {}

        iterator_(T* data, std::size_t pointer):
            m_data(data),
            m_pointer(pointer)
        {}

        bool operator!=(const iterator_& other) { return !(*this == other); }
        bool operator==(const iterator_& other) { return m_pointer == other.m_pointer; }

        iterator_& operator++() {
            m_pointer++;
            return *this;
        }

        iterator_ operator++(int) {
            iterator_ result(*this);
            ++(*this);
            return result;
        }
        T& operator*() { return *(static_cast<T*>(m_data + m_pointer)); }
    };

    template <typename... Args>
    void emplace_back(Args&&... args){
        if(m_size >= MIN_CAPACITY)
            m_data = local_allocator.allocate(m_size * MEM_MULTIPLIER);

        local_allocator.construct(reinterpret_cast<T*>(m_data + m_size), std::forward<Args>(args)...);
        m_size++;
    }

    iterator_ begin() { return iterator_(m_data); }
    iterator_ end() {return iterator_(m_data, m_size); }

private:
    T* m_data = nullptr;
    std::size_t m_size = 0;
};
