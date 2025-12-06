#include <iostream>

// Base class D with required fields for testing
class D {
public:
    int x;
    int y;
};

// A inherits publicly from D
class A : public D {
};

// B inherits publicly from D  
class B : public D {
};

// W inherits directly from A and B (diamond, duplication)
class W : public A, public B {
public:
    int y;
};

// V inherits virtually from A and B (shared D subobject)
class V : virtual public A, virtual public B {
public:
    int x;
};

int main() {
    // Test 1: D instantiation and field access
    D d;
    d.x = 1;
    d.y = 2;
    std::cout << "D: x=" << d.x << ", y=" << d.y << std::endl;
    
    // Test 2: V instantiation (valid with virtual inheritance)
    V v;
    std::cout << "V created successfully" << std::endl;
    
    // Test 3: V member access
    v.x = 3;
    std::cout << "V: x=" << v.x << std::endl;
    
    return 0;
}