#include <iostream>
#include <NumCpp.hpp>

int main() {
    auto a = nc::random::randInt({3, 3}, 1, 10);

    std::cout << a(2, 10) << std::endl;

    return 0;
}