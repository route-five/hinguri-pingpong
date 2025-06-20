#include <iostream>
#include <NumCpp.hpp>

int main() {
    const auto a = nc::random::randInt({4, 4}, 10, 100);

    std::cout << a << std::endl;

    return 0; 
}