#include <iostream>

#include "../pln/block.h"

#ifndef VEH
#define VEH

class Vehicle {
protected:
    int sx, sy;
    Block* steps;
public:
    char symbol = 'x';
    Vehicle() {
        
    }
};

#endif
