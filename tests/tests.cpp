#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>


#include "complex_type.h"
#include "dynamic_array.h"
#include "memory_resource.h"

// ==================== Тесты для ComplexType ====================
TEST(ComplexTypeTest, DefaultConstructor) {
    ComplexType ct;
    EXPECT_EQ(ct.id, 0);
    EXPECT_EQ(ct.name, "");
    EXPECT_DOUBLE_EQ(ct.value, 0.0);
    EXPECT_EQ(ct.data.size(), 3);
}

TEST(ComplexTypeTest, ParameterizedConstructor) {
    ComplexType ct(42, "Test", 3.14);
    EXPECT_EQ(ct.id, 42);
    EXPECT_EQ(ct.name, "Test");
    EXPECT_DOUBLE_EQ(ct.value, 3.14);
    
    // Проверяем данные в векторе
    EXPECT_EQ(ct.data.size(), 3);
    EXPECT_EQ(ct.data[0], 42);
    EXPECT_EQ(ct.data[1], 84);
    EXPECT_EQ(ct.data[2], 126);
}

TEST(ComplexTypeTest, CopyConstructor) {
    ComplexType original(1, "Original", 2.5);
    original.data = {1, 2, 3, 4};
    
    ComplexType copy = original;
    EXPECT_EQ(copy.id, original.id);
    EXPECT_EQ(copy.name, original.name);
    EXPECT_DOUBLE_EQ(copy.value, original.value);
    EXPECT_EQ(copy.data.size(), original.data.size());
    EXPECT_EQ(copy.data[0], original.data[0]);
    EXPECT_EQ(copy.data[1], original.data[1]);
    EXPECT_EQ(copy.data[2], original.data[2]);
    EXPECT_EQ(copy.data[3], original.data[3]);
}

TEST(ComplexTypeTest, AssignmentOperator) {
    ComplexType a(1, "A", 1.0);
    ComplexType b(2, "B", 2.0);
    b.data = {5, 6, 7};
    
    a = b;
    EXPECT_EQ(a.id, b.id);
    EXPECT_EQ(a.name, b.name);
    EXPECT_DOUBLE_EQ(a.value, b.value);
    EXPECT_EQ(a.data.size(), b.data.size());
    EXPECT_EQ(a.data[0], b.data[0]);
    EXPECT_EQ(a.data[1], b.data[1]);
    EXPECT_EQ(a.data[2], b.data[2]);
}

TEST(ComplexTypeTest, SelfAssignment) {
    ComplexType a(1, "A", 1.0);
    a.data = {1, 2, 3};
    
    // Самоприсваивание
    a = a;
    EXPECT_EQ(a.id, 1);
    EXPECT_EQ(a.name, "A");
    EXPECT_DOUBLE_EQ(a.value, 1.0);
    EXPECT_EQ(a.data.size(), 3);
    EXPECT_EQ(a.data[0], 1);
    EXPECT_EQ(a.data[1], 2);
    EXPECT_EQ(a.data[2], 3);
}

TEST(ComplexTypeTest, PrintMethod) {
    ComplexType ct(10, "TestObject", 99.9);
    ct.data = {1, 2, 3};
    
    EXPECT_NO_THROW(ct.print());
}

// ==================== Тесты для DynamicBlockMemoryResource ====================
TEST(DynamicBlockMemoryResourceTest, BasicAllocation) {
    DynamicBlockMemoryResource resource;
    
    void* ptr1 = resource.allocate(100, 8);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = resource.allocate(200, 16);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_NE(ptr1, ptr2);
    
    resource.deallocate(ptr1, 100, 8);
    resource.deallocate(ptr2, 200, 16);
}

TEST(DynamicBlockMemoryResourceTest, ZeroSizeAllocation) {
    DynamicBlockMemoryResource resource;
    
    void* ptr = resource.allocate(0, 1);
    EXPECT_EQ(ptr, nullptr);
}

TEST(DynamicBlockMemoryResourceTest, ReuseMemory) {
    DynamicBlockMemoryResource resource;
    
    void* ptr1 = resource.allocate(100, 8);
    resource.deallocate(ptr1, 100, 8);
    
    void* ptr2 = resource.allocate(100, 8);
    EXPECT_NE(ptr2, nullptr);
    
    resource.deallocate(ptr2, 100, 8);
}

TEST(DynamicBlockMemoryResourceTest, CountAllocatedBlocks) {
    DynamicBlockMemoryResource resource;
    
    EXPECT_EQ(resource.allocated_blocks_count(), 0);
    
    void* ptr1 = resource.allocate(50, 4);
    EXPECT_EQ(resource.allocated_blocks_count(), 1);
    
    void* ptr2 = resource.allocate(50, 4);
    EXPECT_EQ(resource.allocated_blocks_count(), 2);
    
    resource.deallocate(ptr1, 50, 4);
    EXPECT_EQ(resource.allocated_blocks_count(), 1);
    
    resource.deallocate(ptr2, 50, 4);
    EXPECT_EQ(resource.allocated_blocks_count(), 0);
}

TEST(DynamicBlockMemoryResourceTest, IsEqual) {
    DynamicBlockMemoryResource resource1;
    DynamicBlockMemoryResource resource2;
    
    EXPECT_TRUE(resource1.is_equal(resource1));
    EXPECT_TRUE(resource2.is_equal(resource2));
    EXPECT_FALSE(resource1.is_equal(resource2));
    EXPECT_FALSE(resource2.is_equal(resource1));
}

TEST(DynamicBlockMemoryResourceTest, ExceptionOnDoubleFree) {
    DynamicBlockMemoryResource resource;
    
    void* ptr = resource.allocate(100, 8);
    resource.deallocate(ptr, 100, 8);
    
    EXPECT_THROW(resource.deallocate(ptr, 100, 8), std::runtime_error);
}

TEST(DynamicBlockMemoryResourceTest, ExceptionOnInvalidFree) {
    DynamicBlockMemoryResource resource;
    
    int dummy = 42;
    void* invalid_ptr = &dummy;
    
    EXPECT_THROW(resource.deallocate(invalid_ptr, 100, 8), std::runtime_error);
}

TEST(DynamicBlockMemoryResourceTest, CleanupOnDestruction) {
    {
        DynamicBlockMemoryResource resource;
        (void)resource.allocate(100, 8);
        (void)resource.allocate(200, 16);
        EXPECT_EQ(resource.allocated_blocks_count(), 2);
    }
    
    DynamicBlockMemoryResource resource2;
    EXPECT_EQ(resource2.allocated_blocks_count(), 0);
}

// ==================== Тесты для DynamicArray ====================
class DynamicArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        resource = new DynamicBlockMemoryResource();
        alloc = new std::pmr::polymorphic_allocator<int>(resource);
    }
    
    void TearDown() override {
        delete alloc;
        delete resource;
    }
    
    DynamicBlockMemoryResource* resource;
    std::pmr::polymorphic_allocator<int>* alloc;
};

TEST_F(DynamicArrayTest, DefaultConstructor) {
    DynamicArray<int> array(*alloc);
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.size(), 0);
}

TEST_F(DynamicArrayTest, InitializerListConstructor) {
    DynamicArray<int> array({1, 2, 3, 4, 5}, *alloc);
    EXPECT_FALSE(array.empty());
    EXPECT_EQ(array.size(), 5);
}

TEST_F(DynamicArrayTest, PushBack) {
    DynamicArray<int> array(*alloc);
    
    array.push_back(1);
    EXPECT_EQ(array.size(), 1);
    EXPECT_EQ(array.front(), 1);
    EXPECT_EQ(array.back(), 1);
    
    array.push_back(2);
    EXPECT_EQ(array.size(), 2);
    EXPECT_EQ(array.front(), 1);
    EXPECT_EQ(array.back(), 2);
    
    array.push_back(3);
    EXPECT_EQ(array.size(), 3);
    EXPECT_EQ(array.back(), 3);
}

TEST_F(DynamicArrayTest, PopBack) {
    DynamicArray<int> array({1, 2, 3}, *alloc);
    
    array.pop_back();
    EXPECT_EQ(array.size(), 2);
    EXPECT_EQ(array.back(), 2);
    
    array.pop_back();
    EXPECT_EQ(array.size(), 1);
    EXPECT_EQ(array.back(), 1);
    
    array.pop_back();
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.size(), 0);
}

TEST_F(DynamicArrayTest, PopBackEmpty) {
    DynamicArray<int> array(*alloc);
    EXPECT_THROW(array.pop_back(), std::out_of_range);
}

TEST_F(DynamicArrayTest, FrontBackEmpty) {
    DynamicArray<int> array(*alloc);
    EXPECT_THROW(array.front(), std::out_of_range);
    EXPECT_THROW(array.back(), std::out_of_range);
}

TEST_F(DynamicArrayTest, CopyConstructor) {
    DynamicArray<int> original({1, 2, 3}, *alloc);
    DynamicArray<int> copy(original);
    
    EXPECT_EQ(original.size(), copy.size());
    
    auto it1 = original.begin();
    auto it2 = copy.begin();
    while (it1 != original.end() && it2 != copy.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(DynamicArrayTest, AssignmentOperator) {
    DynamicArray<int> array1({1, 2, 3}, *alloc);
    DynamicArray<int> array2(*alloc);
    
    array2 = array1;
    EXPECT_EQ(array1.size(), array2.size());
    
    auto it1 = array1.begin();
    auto it2 = array2.begin();
    while (it1 != array1.end() && it2 != array2.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(DynamicArrayTest, MoveConstructor) {
    DynamicArray<int> original({1, 2, 3}, *alloc);
    size_t original_size = original.size();
    
    DynamicArray<int> moved(std::move(original));
    
    EXPECT_EQ(moved.size(), original_size);
    EXPECT_TRUE(original.empty());
    EXPECT_EQ(original.size(), 0);
}

TEST_F(DynamicArrayTest, MoveAssignment) {
    DynamicArray<int> array1({1, 2, 3}, *alloc);
    size_t array1_size = array1.size();
    
    DynamicArray<int> array2(*alloc);
    array2 = std::move(array1);
    
    EXPECT_EQ(array2.size(), array1_size);
    EXPECT_TRUE(array1.empty());
    EXPECT_EQ(array1.size(), 0);
}

TEST_F(DynamicArrayTest, Clear) {
    DynamicArray<int> array({1, 2, 3, 4, 5}, *alloc);
    EXPECT_FALSE(array.empty());
    
    array.clear();
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.size(), 0);
}

TEST_F(DynamicArrayTest, Iterator) {
    DynamicArray<int> array({1, 2, 3, 4, 5}, *alloc);
    
    // Проверяем префиксный инкремент
    auto it = array.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    
    // Проверяем постфиксный инкремент
    auto it2 = it++;
    EXPECT_EQ(*it2, 3);
    EXPECT_EQ(*it, 4);
    
    // Проверяем сравнение
    auto begin = array.begin();
    auto end = array.end();
    EXPECT_NE(begin, end);
}

TEST_F(DynamicArrayTest, IteratorRangeBasedFor) {
    DynamicArray<int> array({10, 20, 30}, *alloc);
    
    int sum = 0;
    int count = 0;
    for (const auto& item : array) {
        sum += item;
        count++;
    }
    
    EXPECT_EQ(count, 3);
    EXPECT_EQ(sum, 60);
}

TEST_F(DynamicArrayTest, ConstIterator) {
    const DynamicArray<int> array({1, 2, 3}, *alloc);
    
    int sum = 0;
    for (auto it = array.cbegin(); it != array.cend(); ++it) {
        sum += *it;
    }
    
    EXPECT_EQ(sum, 6);
}

TEST_F(DynamicArrayTest, ComplexTypeInDynamicArray) {
    std::pmr::polymorphic_allocator<ComplexType> complex_alloc(resource);
    DynamicArray<ComplexType> array(complex_alloc);
    
    array.push_back(ComplexType(1, "First", 10.5));
    array.push_back(ComplexType(2, "Second", 20.7));
    
    EXPECT_EQ(array.size(), 2);
    EXPECT_EQ(array.front().id, 1);
    EXPECT_EQ(array.back().id, 2);
    EXPECT_EQ(array.front().name, "First");
    EXPECT_EQ(array.back().name, "Second");
}

TEST_F(DynamicArrayTest, MovePushBack) {
    std::pmr::polymorphic_allocator<std::string> string_alloc(resource);
    DynamicArray<std::string> string_array(string_alloc);
    
    std::string str = "Hello";
    string_array.push_back(std::move(str));
   
    EXPECT_EQ(string_array.front(), "Hello");
}

TEST_F(DynamicArrayTest, GetAllocator) {
    DynamicArray<int> array(*alloc);
    auto returned_alloc = array.get_allocator();
    
    EXPECT_EQ(returned_alloc.resource(), alloc->resource());
}

// ==================== Интеграционные тесты ====================
TEST(IntegrationTest, DynamicArrayWithCustomMemoryResource) {
    DynamicBlockMemoryResource resource;
    std::pmr::polymorphic_allocator<int> alloc(&resource);
    
    DynamicArray<int> array(alloc);
    
    for (int i = 0; i < 10; ++i) {
        array.push_back(i * 10);
    }
    
    EXPECT_GT(resource.allocated_blocks_count(), 0);
    
    while (!array.empty()) {
        array.pop_back();
    }
    
    EXPECT_EQ(resource.allocated_blocks_count(), 0);
}

TEST(IntegrationTest, ComplexTypeWithDynamicArrayAndMemoryResource) {
    DynamicBlockMemoryResource resource;
    std::pmr::polymorphic_allocator<ComplexType> alloc(&resource);
    
    {
        DynamicArray<ComplexType> array(alloc);
        
        array.push_back(ComplexType(1, "One", 1.1));
        array.push_back(ComplexType(2, "Two", 2.2));
        array.push_back(ComplexType(3, "Three", 3.3));
        
        EXPECT_EQ(array.size(), 3);
        EXPECT_EQ(resource.allocated_blocks_count(), 3);
        
        auto it = array.begin();
        ++it;
        it->name = "TwoModified";
        EXPECT_EQ(array.begin()->name, "One");
        EXPECT_EQ(it->name, "TwoModified");
    }
    
    EXPECT_EQ(resource.allocated_blocks_count(), 0);
}

TEST(IntegrationTest, MemoryReuseInDynamicArray) {
    DynamicBlockMemoryResource resource;
    std::pmr::polymorphic_allocator<int> alloc(&resource);
    
    {
        DynamicArray<int> array(alloc);
        
        array.push_back(1);
        array.push_back(2);
        array.push_back(3);
        size_t initial_blocks = resource.allocated_blocks_count();
        EXPECT_GT(initial_blocks, 0);
        
        array.pop_back();
        array.pop_back();
        
        array.push_back(4);
        array.push_back(5);
        
        EXPECT_LE(resource.allocated_blocks_count(), initial_blocks + 2);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}