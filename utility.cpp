#include "utility.h"


unsigned int randInt (unsigned int low, unsigned int high) {
    static std::random_device rd;
    static std::seed_seq seed{1, 2, 3, 300};
    static std::mt19937 e2(seed);
    /* static std::mt19937 e2(rd()); */
    std::uniform_int_distribution<unsigned int> dist(low, high);

    return dist(e2);
};
