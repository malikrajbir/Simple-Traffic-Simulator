#include <iostream>

// Collections
#include <map>
#include <vector>

using namespace std;

#ifndef VHC
#define VHC

class Vehicle {

private:

    // Parameters
    int ln, wd; // Length and Width
    int sp, ac; // Speed and Acceleration (current)
    int msp; // Speed (maximum)
    string vtype; //Vehicle Type

    // Key coordinate (right and top-most)
    pair<int, int> key;

    // Representing symbol for the vehicle
    char symbol;

public:

    /*
        Constructor
        -  Parameter Vector (param) contains all the required information for the vehicle
        -  The character 's' represents the designated symbol for the vehicle
    */
    Vehicle(vector<int> param, char s, string type) {
        ln = param[0];
        wd = param[1];
        msp = param[2];
        sp = msp;
        ac = param[3];
        symbol = s;
        vtype = type;
    }

    //Type of the Vehicle
    string type(){
        return vtype;
    }

    // Symbol for the vehicle
    char sym() {
        return symbol;
    }

    // Get the length of the vehicle
    int length() {
        return ln;
    }

    // Get the width of the vehicle
    int width() {
        return wd;
    }

    // SPEED
    // Get max-speed
    int max_speed() {
        return msp;
    }
    // Get current speed
    int speed() {
        return sp;
    }
    // Set current speed
    void update_speed(int nsp) {
        sp = nsp;
    }

    // Get Acceleration
    int acc() {
        return ac;
    }

    // KEY COORDINATE
    // Get the key coordinate of the vehicle
    pair<int, int> pos() {
        return key;
    }
    // Set new key position
    void update_pos(pair<int, int> np) {
        key = np;
    }
};

#endif
