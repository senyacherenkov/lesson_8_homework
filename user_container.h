#pragma once
#include <vector>

template <typename T, typename Alloc = std::allocator<T>>
class UserContainer {
    UserContainer() = default;
    Alloc local_allocator;

    void add(T element){
        auto pointer = local_allocator.allocate(1);
        local_allocator.construct(pointer, element);
    }
};
