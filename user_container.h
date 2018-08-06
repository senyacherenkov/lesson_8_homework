#pragma once
#include <vector>

template <typename T, typename Alloc = std::allocator<T>, std::size_t chunkSize = 1>
class UserContainer {
public:
    Alloc local_allocator;
    UserContainer() {}

    ~UserContainer(){
        for(std::size_t i = 0; i < m_storage.size(); i+=chunkSize) {
            local_allocator.destroy(m_storage.at(i));
            local_allocator.deallocate(m_storage.at(i), 1);
        }
    }
    struct iterator_ {
        std::vector<T*>& m_data;
        std::size_t m_pointer = 0;

        iterator_(std::vector<T*>& data):
            m_data(data),
            m_pointer(0)
        {}

        iterator_(std::vector<T*>& data, std::size_t pointer):
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
        T& operator*() { return *(m_data.at(m_pointer)); }
    };

    template <typename... Args>
    void emplace_back(Args&&... args){
        auto pointer = local_allocator.allocate(1);
        m_storage.push_back(reinterpret_cast<T*>(pointer));

        local_allocator.construct(reinterpret_cast<T*>(m_storage.back()), std::forward<Args>(args)...);
    }

    iterator_ begin() { return iterator_(m_storage); }
    iterator_ end() {return iterator_(m_storage, m_storage.size()); }

private:
    std::vector<T*> m_storage;
};
