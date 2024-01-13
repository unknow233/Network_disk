

#include <iostream>

class MyClass {
public:
    static void staticFunction1() {
        std::cout << "Inside staticFunction1" << std::endl;
        staticFunction2();  // 静态成员函数直接调用另一个静态成员函数
    }

    static void staticFunction2() {
        std::cout << "Inside staticFunction2" << std::endl;
    }
};

int main() {
    MyClass::staticFunction1();  // 通过类名调用静态成员函数

    return 0;
}
