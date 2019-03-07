#include <iostream>

#include "../vhc/vehicle.h"

#ifndef SP
#define SP

class Block {
private:
    bool blocked; // Whether filled or not
    int x, y;   // X & Y coordinates
    char sym;   // Symbol for the char
    Vehicle vcl;
public:
    Block(int xs, int ys) {
        blocked = false;
        x = xs;
        y = ys;
        sym = '.';
    }

    void block() {
        blocked = true;
    }
    void unblock() {
        blocked = false;
    }

    bool is_blocked() {
        return blocked;
    }

    void stepped(Vehicle v1) {
        block();
        vcl = v1;
        sym = v1.symbol;
    }

    void left() {
        unblock();
        sym = '.';
    }

    char symbol() {
        return sym;
    }
};

#endif
