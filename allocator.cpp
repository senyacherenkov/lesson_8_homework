#include "user_allocator.h"
#include "user_container.h"
#include <map>
#include <iostream>

namespace {
    constexpr std::size_t CHUNK_SIZE = 10;

    int factorial (int n) {
        return (n == 0 || n ==1) ? 1: factorial(n-1)*n;
    }
}

int main()
{    
    std::map<int, int> myMapStdAlloc;
    for(int i = 0; i < 10; i++)
        myMapStdAlloc.emplace(i, factorial(i));

    std::map<int, int, std::less<int>, UserAllocator<std::pair<int,int>, CHUNK_SIZE>> MapUserAlloc;
    for(int i = 0; i < 10; i++)
        MapUserAlloc.emplace(i, factorial(i));

    MapUserAlloc.get_allocator().reserve(20);

    for(auto& element: MapUserAlloc)
        std::cout << element.first << " " << element.second << std::endl;

    UserContainer<int> myContainerStdAlloc;
    for(int i = 0; i < 10; i++)
        myContainerStdAlloc.emplace_back(i);

    UserContainer<int, UserAllocator<int, CHUNK_SIZE>, CHUNK_SIZE> myContainerUserAlloc;
    for(int i = 0; i < 10; i++)
        myContainerUserAlloc.emplace_back(i);

    for(auto i = myContainerUserAlloc.begin(); i != myContainerUserAlloc.end(); i++)
        std::cout << *i << std::endl;
    
    return 0;
}
