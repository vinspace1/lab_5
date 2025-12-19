#pragma once


#include <memory_resource>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <initializer_list>
#include <utility>
#include <iostream>

template<typename T>
class DynamicArray {
private:
    struct Node {
        T* data;
        Node* next;
        
        Node(T* d, Node* n = nullptr) : data(d), next(n) {}
    };
    
    Node* head_;
    Node* tail_;
    size_t size_;
    std::pmr::polymorphic_allocator<T> allocator_;
    
public:
    using value_type = T;
    using allocator_type = std::pmr::polymorphic_allocator<T>;
    
    class Iterator {
    private:
        Node* current_;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        explicit Iterator(Node* node = nullptr) : current_(node) {}
        
        reference operator*() const {
            return *(current_->data);
        }
        
        pointer operator->() const {
            return current_->data;
        }
        
        Iterator& operator++() {
            current_ = current_->next;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    
    using iterator = Iterator;
    using const_iterator = const Iterator;
    
    // Конструкторы
    explicit DynamicArray(std::pmr::polymorphic_allocator<T> alloc = {})
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {}
    
    DynamicArray(std::initializer_list<T> init, 
                std::pmr::polymorphic_allocator<T> alloc = {})
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // Конструктор копирования
    DynamicArray(const DynamicArray& other) 
        : head_(nullptr), tail_(nullptr), size_(0), 
          allocator_(other.allocator_) {
        for (const auto& item : other) {
            push_back(item);
        }
    }
    
    // Оператор присваивания
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            clear();
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }
    
    // Конструктор перемещения
    DynamicArray(DynamicArray&& other) noexcept
        : head_(other.head_), tail_(other.tail_), 
          size_(other.size_), allocator_(std::move(other.allocator_)) {  // Используем перемещение
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Оператор перемещения
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            // Аллокатор не присваиваем - сохраняем текущий
            // Или перемещаем, если поддерживается
            
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // Деструктор
    ~DynamicArray() {
        clear();
    }
    
    // Методы контейнера
    void push_back(const T& value) {
        T* new_data = allocator_.allocate(1);
        std::allocator_traits<std::pmr::polymorphic_allocator<T>>::construct(allocator_, new_data, value);
        
        Node* new_node = new Node(new_data);
        
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            tail_ = new_node;
        }
        size_++;
    }
    
    void push_back(T&& value) {
        T* new_data = allocator_.allocate(1);
        std::allocator_traits<std::pmr::polymorphic_allocator<T>>::construct(
            allocator_, new_data, std::move(value));
        
        Node* new_node = new Node(new_data);
        
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            tail_ = new_node;
        }
        size_++;
    }
    
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("DynamicArray is empty");
        }
        
        if (head_ == tail_) {
            std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(allocator_, tail_->data);
            allocator_.deallocate(tail_->data, 1);
            delete tail_;
            head_ = tail_ = nullptr;
        } else {
            Node* current = head_;
            while (current->next != tail_) {
                current = current->next;
            }
            
            std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(allocator_, tail_->data);
            allocator_.deallocate(tail_->data, 1);
            delete tail_;
            
            tail_ = current;
            tail_->next = nullptr;
        }
        size_--;
    }
    
    T& front() {
        if (empty()) {
            throw std::out_of_range("DynamicArray is empty");
        }
        return *(head_->data);
    }
    
    const T& front() const {
        if (empty()) {
            throw std::out_of_range("DynamicArray is empty");
        }
        return *(head_->data);
    }
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("DynamicArray is empty");
        }
        return *(tail_->data);
    }
    
    const T& back() const {
        if (empty()) {
            throw std::out_of_range("DynamicArray is empty");
        }
        return *(tail_->data);
    }
    
    bool empty() const {
        return size_ == 0;
    }
    
    size_t size() const {
        return size_;
    }
    
    void clear() {
        while (!empty()) {
            pop_back();
        }
    }
    
    // Итераторы
    iterator begin() {
        return iterator(head_);
    }
    
    iterator end() {
        return iterator(nullptr);
    }
    
    const_iterator begin() const {
        return iterator(head_);
    }
    
    const_iterator end() const {
        return iterator(nullptr);
    }
    
    const_iterator cbegin() const {
        return iterator(head_);
    }
    
    const_iterator cend() const {
        return iterator(nullptr);
    }
    
    // Получение аллокатора
    allocator_type get_allocator() const {
        return allocator_;
    }
};