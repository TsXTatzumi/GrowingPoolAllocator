#include <iostream>
#include "GrowingPoolAllocator.h"
// Maxi, Flo, Simeon, Nadine

struct testStruct{
    int test1;
    int test2;
    float test3;

    testStruct(int one, int two, float three) { test1 = one; test2 = two; test3 = three; }
};

int main()
{
    int numberOfelements = 1000;
    GrowingPoolAllocator gpa(sizeof(int), numberOfelements, 17);
    std::vector<int*> allocatedStuff;

    for (int i = 0; i < numberOfelements; ++i) {
        allocatedStuff.push_back(gpa.alloc<int>(i));
    }

    std::cout << "First Test:" << std::endl;

    for (int i = 0; i < numberOfelements; ++i) {
        if (*allocatedStuff[i] != i) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        *allocatedStuff[i] *= -1;
    }

    std::cout << "Second Test:" << std::endl;

    for (int i = 0; i < numberOfelements; ++i) {
        if (*allocatedStuff[i] != -i) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        gpa.free(allocatedStuff[i]);
    }

    allocatedStuff.clear();

    for (int i = 0; i < numberOfelements; ++i) {
        allocatedStuff.push_back(gpa.alloc<int>());
    }

    std::cout << "Third Test:" << std::endl;

    for (int i = 0; i < numberOfelements; ++i) {
        if (*allocatedStuff[i] != 0) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
    }

    GrowingPoolAllocator gpa2(sizeof(testStruct), 912, 200);
    std::vector<testStruct*> allocatedStuff2;
    std::vector<long double*> allocatedStuff3;

    for (int i = 0; i < 456; ++i) {
        allocatedStuff2.push_back(gpa2.alloc<testStruct>(i, -i, (i / 1000.0f)));
        allocatedStuff3.push_back(gpa2.alloc<long double>(i / 10000.0L));
    }

    std::cout << "Forth Test:" << std::endl;

    for (int i = 0; i < 456; ++i) {
        if (allocatedStuff2[i]->test1 != i || allocatedStuff2[i]->test2 != -i || allocatedStuff2[i]->test3 != (i / 1000.0f)) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        allocatedStuff2[i]->test1 *= -1;
    }

    std::cout << "Fifth Test:" << std::endl;

    for (int i = 0; i < 456; ++i) {
        if (*allocatedStuff3[i] != (i / 10000.0L)) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        *allocatedStuff3[i] *= -1;
    }

    std::cout << "Sixth Test:" << std::endl;

    for (int i = 0; i < numberOfelements; ++i) {
        if (*allocatedStuff[i] != 0) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        gpa.free(allocatedStuff[i]);
    }

    std::cout << "Seventh Test:" << std::endl;

    for (int i = 0; i < 456; ++i) {
        if (allocatedStuff2[i]->test1 != -i) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        gpa2.free(allocatedStuff2[i]);
    }

    std::cout << "Eight Test:" << std::endl;

    for (int i = 0; i < 456; ++i) {
        if (*allocatedStuff3[i] != (i / -10000.0L)) {
            std::cerr << "unexpeted value at position: " << i << std::endl;
        }
        gpa2.free(allocatedStuff3[i]);
    }

    return 0;
}