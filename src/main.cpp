#include "../include/memory_resource.h"
#include "../include/dynamic_array.h"
#include "../include/complex_type.h"
#include <iostream>

void demonstrate_int_types() {
    std::cout << "\n=== Демонстрация работы с int ===" << std::endl;
    
    DynamicBlockMemoryResource mem_resource;
    std::pmr::polymorphic_allocator<int> alloc(&mem_resource);
    
    DynamicArray<int> int_array({1, 2, 3, 4, 5}, alloc);
    
    std::cout << "Size: " << int_array.size() << std::endl;
    std::cout << "Elements: ";
    for (const auto& elem : int_array) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    
    int_array.push_back(6);
    int_array.push_back(7);
    
    std::cout << "After push_back: ";
    for (const auto& elem : int_array) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    
    int_array.pop_back();
    std::cout << "After pop_back: ";
    for (const auto& elem : int_array) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Front: " << int_array.front() << std::endl;
    std::cout << "Back: " << int_array.back() << std::endl;
    
    std::cout << "Using iterator: ";
    for (auto it = int_array.begin(); it != int_array.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void demonstrate_complex_types() {
    std::cout << "\n=== Демонстрация работы с ComplexType ===" << std::endl;
    
    DynamicBlockMemoryResource mem_resource;
    std::pmr::polymorphic_allocator<ComplexType> alloc(&mem_resource);
    
    DynamicArray<ComplexType> complex_array(alloc);
    
    complex_array.push_back(ComplexType{1, "First", 10.5});
    complex_array.push_back(ComplexType{2, "Second", 20.7});
    complex_array.push_back(ComplexType{3, "Third", 30.9});
    
    std::cout << "Complex array size: " << complex_array.size() << std::endl;
    
    std::cout << "Elements in complex array:" << std::endl;
    for (const auto& elem : complex_array) {
        elem.print();
    }
    
    auto it = complex_array.begin();
    ++it;
    it->name = "Second Modified";
    
    std::cout << "\nAfter modification:" << std::endl;
    for (const auto& elem : complex_array) {
        elem.print();
    }
    
    std::cout << "\nCopying container:" << std::endl;
    DynamicArray<ComplexType> copied_array = complex_array;
    std::cout << "Copied array size: " << copied_array.size() << std::endl;
    
    std::cout << "\nMoving container:" << std::endl;
    DynamicArray<ComplexType> moved_array = std::move(complex_array);
    std::cout << "Moved array size: " << moved_array.size() << std::endl;
    std::cout << "Original array size after move: " << complex_array.size() << std::endl;
}

void demonstrate_memory_resource() {
    std::cout << "\n=== Демонстрация работы memory_resource ===" << std::endl;
    
    {
        DynamicBlockMemoryResource mem_resource;
        std::pmr::polymorphic_allocator<int> alloc(&mem_resource);
        
        DynamicArray<int> array(alloc);
        
        std::cout << "Allocating 5 ints..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            array.push_back(i * 10);
        }
        
        std::cout << "Allocated blocks: " << mem_resource.allocated_blocks_count() << std::endl;
        
        std::cout << "\nDeallocating 2 ints..." << std::endl;
        array.pop_back();
        array.pop_back();
        
        std::cout << "Allocated blocks after deallocation: " 
                  << mem_resource.allocated_blocks_count() << std::endl;
        
        std::cout << "\nAllocating 3 more ints..." << std::endl;
        for (int i = 5; i < 8; ++i) {
            array.push_back(i * 10);
        }
        
        std::cout << "Allocated blocks: " << mem_resource.allocated_blocks_count() << std::endl;
        
        std::cout << "\nElements in array: ";
        for (const auto& elem : array) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
        
        std::cout << "\nGoing out of scope - memory_resource will clean up..." << std::endl;
    }
    
    std::cout << "Scope ended, all memory should be cleaned up" << std::endl;
}

int main() {
    std::cout << "=== Лабораторная работа 5: Динамический массив с memory_resource ===" << std::endl;
    
    try {
        demonstrate_int_types();
        demonstrate_complex_types();
        demonstrate_memory_resource();
        
        std::cout << "\n=== Все демонстрации завершены успешно ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}