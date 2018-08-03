#include "user_allocator.h"
#include <map>
#include <iostream>
#include <vector>

int factorial (int n) {
    return (n == 0 || n ==1) ? 1: factorial(n-1)*n;
}

int main()
{
    std::vector<int> ex;
    std::map<int, int> testMap;
    for(int i = 0; i < 10; i++)
        testMap.emplace(i, factorial(i));

    std::map<int, int, std::less<int>, UserAllocator<std::pair<int,int>, 10>> testMapUserAllocator;
    for(int i = 0; i < 10; i++)
        testMapUserAllocator.emplace(i, factorial(i));

    for(auto& element: testMapUserAllocator)
        std::cout << element.first << " " << element.second << std::endl;

    return 0;
}
