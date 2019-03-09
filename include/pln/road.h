#include <iostream>

using namespace std;

#include "/Users/rajbirmalik/OneDrive - IIT Delhi/COP 290/Simple Traffic Simulator/include/vhc/vehicle.h"
#include "/Users/rajbirmalik/OneDrive - IIT Delhi/COP 290/Simple Traffic Simulator/include/pln/block.h"



#ifndef RD
#define RD

class Road {
protected:
    void initialise_road() {
        for(int i=0; i<xs; i++)
            for(int j=0; j<ys; j++)
                map[i*xs+j] = Block();
    }
public:
    Block * map;
    int xs, ys;
    int sx;
    int time = 0;
    vector<Vehicle> vcls;

    Road(int len_x, int len_y, int sig_x) {
        // Road dimensions
        xs = len_x;
        ys = len_y;
        // x-coordinate of the signal line
        sx = sig_x;
        // Creating the blocks, road is composed of
        map = (Block *)malloc(sizeof(Block)*len_x*len_y);
        // Set road
        initialise_road();
        // Print road
        show_road();
        // Set time to start (0)
        time = 0;
    }

    // Block a particular coordinate (either red light or a vehicle has stepped on it)
    void block(Vehicle* v1, int i, int j) {
        map[i*xs+j].stepped(v1);
    }

    // Unblock a particular coordinate (either green light or a vehicle has left it)
    void unblock(int i, int j) {
        map[i*xs+j].left();
    }

    // Checking whether a particular block is free or not
    bool is_free(int i, int j) {
        return !map[i*xs+j].is_blocked();
    }

    // Signal is red
    void signal_red() {
        for(int i=0; i<ys; i++)
            block(NULL, sx, i);
    }

    // Signal is green
    void signal_green() {
        for(int i=0; i<ys; i++)
            unblock(sx, i);
    }
        void show_road() {
        cout << "\033[H\033[J";
        cout << "Time: " << time << "\n\n";
        for(int i=0; i<xs; i++)
            cout << "-";
        cout << "\n";
        for(int i=0; i<xs; i++) {
            for(int j=0; j<ys; j++)
                cout << map[i*xs+j].symbol();
            cout << "\n";
        }
        for(int i=0; i<xs; i++)
            cout << "-";
        cout << "\n";
    }

    // Increase time
    void time_step() {
        time++;
        for(Vehicle vcl : vcls)
            vcl.move();
        show_road();
    }
};

#endif
