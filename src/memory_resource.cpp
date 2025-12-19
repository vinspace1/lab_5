#include "../include/memory_resource.h"
#include <iostream>
#include <stdexcept>

DynamicBlockMemoryResource::BlockInfo::BlockInfo(void* p, std::size_t s) 
    : ptr(p), size(s) {}

DynamicBlockMemoryResource::DynamicBlockMemoryResource(
    std::pmr::memory_resource* upstream) 
    : upstream_(upstream) {}

void* DynamicBlockMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    if (bytes == 0) {
        return nullptr;
    }
    
    void* ptr = upstream_->allocate(bytes, alignment);
    allocated_blocks_.emplace(ptr, BlockInfo{ptr, bytes});
    
    std::cout << "Allocated block: " << ptr << ", size: " << bytes 
              << ", alignment: " << alignment << std::endl;
    return ptr;
}

void DynamicBlockMemoryResource::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) {
    // Проверяем, что указатель не nullptr (это разрешено стандартом для deallocate)
    if (ptr == nullptr) {
        return;
    }
    
    auto it = allocated_blocks_.find(ptr);
    if (it != allocated_blocks_.end()) {
        upstream_->deallocate(ptr, bytes, alignment);
        allocated_blocks_.erase(it);
        
        std::cout << "Deallocated block: " << ptr << ", size: " << bytes << std::endl;
    } else {
        throw std::runtime_error("Trying to deallocate unallocated block");
    }
}

bool DynamicBlockMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

DynamicBlockMemoryResource::~DynamicBlockMemoryResource() {
    for (const auto& [ptr, info] : allocated_blocks_) {
        std::cout << "Cleaning up leaked block: " << ptr << ", size: " << info.size << std::endl;
        upstream_->deallocate(ptr, info.size);
    }
    allocated_blocks_.clear();
}

std::size_t DynamicBlockMemoryResource::allocated_blocks_count() const {
    return allocated_blocks_.size();
}