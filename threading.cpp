#include <iostream>
#include <thread>

int foo() {
    std::cout << "foo" << std::endl;
}

int main() {
    std::thread t1(foo);
	std::thread t2(foo);
    t2.join();
    t1.join();
    std::cout << "done" << std::endl;
    return 0;
}
