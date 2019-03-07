#include <iostream>

#include "block.h"

#ifndef RD
#define RD

class Road {
private:
    Block * map;
    int xs, ys;

    void initialise_road() {
        for(int i=0; i<xs; i++)
            for(int j=0; j<ys; j++)
                map[i*xs+j] = Block(i, j);
    }
public:
    Road(int len_x, int len_y) {
        xs = len_x;
        ys = len_y;
        map = (Block *)malloc(sizeof(Block)*len_x*len_y);
        initialise_road();
        show_road();
    }

    void show_road() {
        for(int i=0; i<xs; i++) {
            for(int j=0; j<ys; j++)
                std::cout << map[i*xs+j].symbol();
            std::cout << "\n";
        }
    }

    
};

#endif
