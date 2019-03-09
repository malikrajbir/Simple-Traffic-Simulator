#include <iostream>

#include "/Users/rajbirmalik/OneDrive - IIT Delhi/COP 290/Simple Traffic Simulator/include/vhc/vehicle.h"


#ifndef SP
#define SP

class Block {
private:
    bool blocked; // Whether filled or not
    char sym;   // Symbol for the char
    Vehicle* vcl = NULL;
public:
    Block() {
        blocked = false;
        sym = '.';
        vcl = NULL;
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

    void stepped(Vehicle* v1) {
        block();
        vcl = v1;
        if( v1 != NULL)
            sym = v1->symbol;
        else
            sym = '+';
    }

    void left() {
        unblock();
        sym = '.';
        vcl = NULL;
    }

    char symbol() {
        return sym;
    }
};

#endif
