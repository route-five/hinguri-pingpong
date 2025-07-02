//
// Created by Hyunseung Ryu on 25. 6. 24.
//

#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>

template<typename T = double>
class Random {
public:
    T start;
    T end;

    Random(T start, T end): start{start}, end{end} {
    }

    Random(): start{0.}, end{1.} {
    }

    T generate() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<T> dis(start, end);

        return dis(gen);
    }
};


#endif //RANDOM_HPP
