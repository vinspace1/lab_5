#pragma once

#include <memory_resource>
#include <map>
#include <cstddef>

class DynamicBlockMemoryResource : public std::pmr::memory_resource {
private:
    struct BlockInfo {
        void* ptr;
        std::size_t size;
        
        BlockInfo(void* p = nullptr, std::size_t s = 0);
    };
    
    std::map<void*, BlockInfo> allocated_blocks_;
    std::pmr::memory_resource* upstream_;
    
protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
    
public:
    explicit DynamicBlockMemoryResource(std::pmr::memory_resource* upstream = 
                                        std::pmr::get_default_resource());
    
    DynamicBlockMemoryResource(const DynamicBlockMemoryResource&) = delete;
    DynamicBlockMemoryResource& operator=(const DynamicBlockMemoryResource&) = delete;
    
    ~DynamicBlockMemoryResource() override;
    
    std::size_t allocated_blocks_count() const;
};