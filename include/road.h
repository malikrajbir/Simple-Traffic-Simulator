#include <iostream>

// Collections
#include <vector>
#include <map>

// For time control
#include<unistd.h>

// Vehicle class
#include "vehicle.h"

using namespace std;

#ifndef RD
#define RD

class Road {

private:

    // Road Parameters
    int ln, ht; // Length and Height
    int t;      // Time
    int sig;   // Signal positions (X-coordinate)

    // Auto initialisation
    vector<Vehicle> vcls;   // Vector of vehicles
    vector<vector<char>> positions;  // Positions taken by other vehicles on map

public:

    /*
        Constructor
        - Takes the parameters and designs the road
        - Sets time to 0
    */
    Road(int length, int height, int signal) {
        ln = length;
        ht = height;
        sig = signal;
        t = 0;
        positions = vector<vector<char>>(ln, vector<char>(ht, ' '));
    }

    // Get the length of the road
    int length() {
        return ln;
    }

    // Get the heigth of the road
    int heigth() {
        return ht;
    }

    // Get the road-time
    int time() {
        return t;
    }

    // Get the list of vehicles on road
    vector<Vehicle>& current_vcls() {
        return vcls;
    }

    // Get the position vector (grid) of road
    vector<vector<char>>& marks() {
        return positions;
    }

    // Visualising the road on the terminal
    void show_road() {
        cout << "\033[H\033[J";
        cout << "Time: " << t << "\n";
        //-------- Line
        for(int i=ln; i>0; --i)
            cout << "--";
        cout << "\n";
        // Main road
        for(int i=0; i<ht; i++) {
            for(int j=0; j<ln; j++)
                cout << positions[j][i] << " ";
            cout << "\n";
        }
        //-------- Line
        for(int i=ln; i>0; --i)
            cout << "--";
        cout << "\n";
        usleep(250*1000);
    }

    // Turn signal to red
    void signal_red() {
        for(int i=0; i<ht; i++)
            positions[sig][i] = '=';
    }

    // Turn signal to green
    void signal_green() {
        for(int i=0; i<ht; i++)
            positions[sig][i] = ' ';
    }

    // Increasing time
    void inc_time() {
        t++;
    }

};

#endif