#include <string>
#include <vector>
#include <iostream>

struct ComplexType {
    int id;
    std::string name;
    double value;
    std::vector<int> data;
    
    ComplexType(int i = 0, std::string n = "", double v = 0.0);
    ComplexType(const ComplexType& other);
    ComplexType& operator=(const ComplexType& other);
    ~ComplexType();
    
    void print() const;
};