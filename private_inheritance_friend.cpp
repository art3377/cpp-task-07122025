#include <iostream>

struct Granny {
    int a = 42;
};

struct Mom : private Granny {
    int b = 100;
    // Son gets access to Mom's private members, including inherited Granny
    friend struct Son;
};

struct Son : public Mom {
    int c = 200;
    
    void f(Granny g) {
        std::cout << "Parameter Granny a: " << g.a << std::endl;
        std::cout << "Mom b: " << b << std::endl;
        std::cout << "Son c: " << c << std::endl;
        // Access to Granny::a from Son via this pointer despite private inheritance
        std::cout << "Granny a via this: " << this->Granny::a << std::endl;
    }
};

int main() {
    Son s;
    s.f(Granny{});
    return 0;
}
