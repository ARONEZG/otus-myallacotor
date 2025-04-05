#include "container.h"
#include <map>


int factorial(int n) {
    return (n == 0) ? 1 : n * factorial(n - 1);
}

int main() {

    std::map<int, int> std_map;
    for (int i = 0; i < 10; ++i) {
        std_map[i] = factorial(i);
    }

    using MapAllocator = myAlloc<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<int>, MapAllocator> custom_alloc_map;
    for (int i = 0; i < 10; ++i) {
        custom_alloc_map[i] = factorial(i);
    }

    std::cout << "std::map with custom allocator:\n";
    for (const auto& pair : custom_alloc_map) {
        std::cout << pair.first << " " << pair.second << "\n";
    }


    SimpleContainer<int> my_container;
    for (int i = 0; i < 10; ++i) {
        my_container.push_back(i);
    }

    using ContainerAllocator = myAlloc<int, 10>;
    SimpleContainer<int, ContainerAllocator> custom_alloc_container;
    for (int i = 0; i < 10; ++i) {
        custom_alloc_container.push_back(i);
    }

    std::cout << "\nOur container with custom allocator:\n";
    for (auto it = custom_alloc_container.begin(); it != custom_alloc_container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    return 0;
}
