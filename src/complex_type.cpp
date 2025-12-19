#include "../include/complex_type.h"

ComplexType::ComplexType(int i, std::string n, double v) 
    : id(i), name(std::move(n)), value(v), data({i, i*2, i*3}) {}

ComplexType::ComplexType(const ComplexType& other) 
    : id(other.id), name(other.name), value(other.value), data(other.data) {}

ComplexType& ComplexType::operator=(const ComplexType& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        value = other.value;
        data = other.data;
    }
    return *this;
}

ComplexType::~ComplexType() {
    std::cout << "Destroying ComplexType: " << id << " - " << name << std::endl;
}

void ComplexType::print() const {
    std::cout << "ComplexType { id: " << id 
              << ", name: " << name 
              << ", value: " << value 
              << ", data: [";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << data[i];
        if (i < data.size() - 1) std::cout << ", ";
    }
    std::cout << "] }" << std::endl;
}