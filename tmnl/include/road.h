#include <iostream>
#include <ostream>

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
    int uid;    //Unique ID
    int ln, ht; // Length and Height
    int t;      // Time
    int sig;   // Signal positions (X-coordinate)
    string signal = "GREEN"; //Signal Color
    // Auto initialisation
    vector<Vehicle> vcls;   // Vector of vehicles
    vector<vector<string>> positions;  // Positions taken by other vehicles on map

public:

    /*
        Constructor
        - Takes the parameters and designs the road
        - Sets time to 0
    */
    Road(int id, int length, int height, int signal) {
        uid = id;
        ln = length;
        ht = height;
        sig = signal;
        t = 0;
        positions = vector<vector<string>>(ln, vector<string>(ht, "\033[1;100m \033[0m"));
        for(int i=0; i<length; i++)
            if(ht%2)    // Odd
                positions[i][(ht-1)/2] = "\033[1;33;100m-\033[0m";
            else        // Even
                positions[i][(ht-1)/2] = "\033[1;33;100m_\033[0m";
    }

    Road() {
        //Default Constructor
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

    //Current signal of road
    string get_signal(){
        return signal;
    }

    //Change Signal
    void set_signal(string s) {
        signal = s;
    }
    // Get the list of vehicles on road
    vector<Vehicle>& current_vcls() {
        return vcls;
    }

    // Get the position vector (grid) of road
    vector<vector<string>>& marks() {
        return positions;
    }

    // Visualising the road on the terminal
    void show_road(int pause=150) {
        cout << "\033[H\033[J";
        cout << "Time: " << t << "\n";
        //-------- Line
        for(int i=ln; i>0; --i)
            cout << "--";
        cout << "\n";
        // Main road
        for(int i=0; i<ht; i++) {
            for(int j=0; j<ln; j++)
                cout << positions[j][i] << "\033[1;100m \033[0m";
            cout << "\n";
        }
        //-------- Line
        for(int i=ln; i>0; --i)
            cout << "--";
        cout << "\n";
        usleep(pause*1000);
    }

    // Writing road contents on a file
    void wrt_road(ostream& out) {
        out << "Time: " << t << "\n";
        //-------- Line
        for(int i=ln; i>0; --i)
            out << "--";
        out << "\n";
        // Main road
        for(int i=0; i<ht; i++) {
            for(int j=0; j<ln; j++)
                if(positions[j][i].length() == 13)
                    out << positions[j][i].at(8) << " ";
                else if(positions[j][i].length() == 16)
                    out << positions[j][i].at(11) << " ";
                else
                    out << positions[j][i] << " ";
            out << "\n";
        }
        //-------- Line
        for(int i=ln; i>0; --i)
            out << "--";
        out << "\n";
    }

    // Turn signal to red
    void signal_red() {
        for(int i=0; i<ht; i++)
            positions[sig][i] = "\033[1;31;100m|\033[0m";
    }

    // Turn signal to green
    void signal_green() {
        for(int i=0; i<ht; i++)
            if(i == (ht-1)/2)
                if(ht%2)    // Odd
                    positions[sig][i] = "\033[1;33;100m-\033[0m";
                else        // Even
                    positions[sig][i] = "\033[1;33;100m_\033[0m";
            else
                positions[sig][i] = "\033[1;100m \033[0m";
    }

    // Increasing time
    void inc_time() {
        t++;
    }

};

#endif
