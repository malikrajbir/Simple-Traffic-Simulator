#include <iostream>
#include <vector>

using namespace std;

#include "/Users/rajbirmalik/OneDrive - IIT Delhi/COP 290/Simple Traffic Simulator/include/pln/block.h"
#include "/Users/rajbirmalik/OneDrive - IIT Delhi/COP 290/Simple Traffic Simulator/include/pln/road.h"


#ifndef VEH
#define VEH

class Vehicle {
protected:
    // Vehicle parameters
    int len, wid, sp, ac;
    // Max parameters
    int msp, mac;

    // Key position
    int key_x, key_y;

    // Associated Road
    Road* rda;

    // Picking the current position for the vehicle
    int set_pos(Road rd) {
        for(int i=rd.ys-1; i>=1; i--) {
            if(rd.is_free(i, 0) && rd.is_free(i-1, 0))
                return i;
        }
        throw runtime_error("No position free for the vehicle to enter...");
    }
    // Check whether next step can be taken or not
    bool movable(Road rd) {
        int i = key_x;
        if(i >= rd.xs)
            return true;
        for(int j=key_y; j>=key_y-wid; j--) {
            if(!rd.is_free(i, j))
                return false;
        }
        return true;
    }
    void unblock(Road rd) {
        for(int i=key_x; i>=key_x-len; i--)
            for(int j=key_y; j>=key_y-wid; j--) {
                if(i > rd.xs || i <= 0 || j > rd.ys || j <= 0)
                    continue;
                rd.unblock(i-1, j-1);
            }
        return true;
    }
    // Take the step (assuming possible)
    void set(Road rd) {
        for(int i=key_x; i>=key_x-len; i--)
            for(int j=key_y; j>=key_y-wid; j--) {
                if(i >= rd.xs || i < 0 || j >= rd.ys || j < 0)
                    continue;
                rd.block(this, i, j);

            }
    }
public:
    char symbol;
    Vehicle() {
    }
    Vehicle(char sym, vector<int> param, Road* rd) {
        // Parameters
        len = param[0];
        wid = param[1];
        msp = param[2];
        sp = msp;
        mac = param[3];
        ac = mac;
        // Road
        rda = rd;
        rd->vcls.push_back(*this);
        // Symbol
        symbol = sym;
        // Current positions (top most and right most)
        key_y = set_pos(*rd);
        key_x = 0;
        // start
        set(*rd);
    }
    void move() {
        sp = min(msp, sp+mac);
        for(int i=0; i<sp; i++) {
            key_x++;
            if(movable(*rda)) {
                unblock(*rda)
                set(*rda);
            }
            else {
                sp = i;
                key_x--;
                break;
            }
        }
    }
};

#endif
