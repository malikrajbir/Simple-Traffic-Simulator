#include <iostream>

// Collections
#include <map>
#include <vector>

using namespace std;

#ifndef VHC
#define VHC

// Enumeration history for movement
enum lmv {f, l, r, not_set};

class Vehicle {

private:

    // Parameters
    int ln, wd; // Length and Width
    int sp, ac; // Speed and Acceleration (current)
    int msp; // Speed (maximum)

    string color;

    // Key coordinate (right and top-most)
    pair<int, int> key;

    // Representing symbol for the vehicle
    string symbol;

    // History, keeping check of the last move
    lmv last;

    // Getting the desired head for color
    string get_color(string clr) {
        if(clr == "BLUE")
            return "\033[1;34m";
        else if(clr == "GREEN")
            return "\033[1;32m";
        else if(clr == "RED")
            return "\033[1;31m";
        else if(clr == "YELLOW")
            return "\033[1;33m";
        return "";
    }
    // Default string for normalisation
    string norm = "\033[0m";

public:

    /*
        Constructor
        -  Parameter Vector (param) contains all the required information for the vehicle
        -  The character 's' represents the designated symbol for the vehicle
    */
    Vehicle(vector<int> param, string s) {
        ln = param[0];
        wd = param[1];
        msp = param[2];
        sp = msp;
        ac = param[3];
        symbol = s;
        last = not_set;
    }

    // Symbol for the vehicle
    string sym() {
        return symbol;
    }
    // Give color to the symbol
    void color_sym(string color) {
        string c = get_color(color);
        if(c == "")
            return;
        symbol = c+symbol+norm;
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

    // Setting the color of the vehicle for Visual
    void set_color(string clr) {
        color = clr;
    }

    // REGARGING HISTORY
    // Returing last move
    lmv last_move() {
        return last;
    }
    // Updating last move
    void update_move(lmv nmv) {
        last = nmv;
    }
};

#endif
