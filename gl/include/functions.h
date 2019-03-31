#include <iostream>

// Collections
#include <vector>
#include <map>

// Random numbers
#include <cstdlib>

// Algorithms library
#include <algorithm>

using namespace std;

// Road and Vehicle class
#include "road.h"
#include "vehicle.h"

// CLASS OF FUNCTIONS INVOLVING BOTH ROAD AND VEHICLES

/*
    The road process is carried by the following processes
        - Time is the main check
        - Vehicles move in accordance with other vehicles (front most are free of any constraint so starting from them)
        - Each action is defined here, these are called accordingly in the main simulator
        - Actions have certain assumptions, these must be kept in mind while implementing them
        - Currently the model assumes no collision

    Three important features of simulation
        - Vehicles come (ADD)
        - Time goes on, and present vehicles interact (PROCESS)
        - Vehicles go (SUB)
*/

#ifndef FN
#define FN

enum mv {front, lt, rt};

// ADD

// Adding and setting a vehicle's key position (** COMES AFTER PROCESS IN THE SAME TIME)
void add_vehicle(Vehicle& v, Road& r) {
    // Vehicles is set and key position must still be set
    vector<int> possible{};
    bool good, set = false;
    for(int i=0; i<r.heigth()-v.width()+1; i++) {
        good = true;
        for(int j=0; j<v.width(); j++)
            good = (good)&&(r.marks()[0][i+j] == "\033[1;100m \033[0m" || r.marks()[0][i+j] == "\033[1;33;100m_\033[0m");
        if(good) {
            set = true;
            possible.push_back(i);
        }
    }
    if(!set) {
        cout << "Not possible for vehicle to enter...\n";
        return;
    }
    int rand_pos = possible[rand()%possible.size()];
    v.update_pos(pair<int, int>(0, rand_pos));
    r.current_vcls().push_back(v);
    for(int i=0; i<v.width(); i++)
        r.marks()[0][i+v.pos().second] = v.sym();
}

// PROCESS
// Checking the status of vehicles movement
// The 'direction' parameter checks for the direction of movement
bool movable(Vehicle& v, Road& r, mv direction) {
    if(direction == front) {
        int x = v.pos().first+1;
        int y = v.pos().second;
        int len = v.length();
        // If exiting (no other ahead)
        if(x >= r.length())
            return true;
        // checking the next complete row
        for(int j=0; j<v.width(); j++) {
            if(r.marks()[x][y+j] == "\033[1;31;100m|\033[0m")
                if(v.width() == 1)
                    continue;
            if((r.marks()[x][y+j] != "\033[1;100m \033[0m" && r.marks()[x][y+j] != "\033[1;33;100m_\033[0m") || (r.marks()[x-1][y+j] == "\033[1;31;100mX\033[0m" && v.width() != 1))
                return false;
            if((v.width() == 1) && (x-len >= 0))
            try {
                if((r.marks()[x-len][y+j] == "\033[1;31;100mX\033[0m"))
                    return false;
            }
            catch(...) {
                continue;
            }
        }
        return true;
    }
    else if (direction == rt) {
        int x = v.pos().first;
        int y = v.pos().second+v.width();
        // Exiting if stopped because of red light
        if(r.marks()[x+1][0] == "\033[1;31;100m|\033[0m" || r.marks()[x+1][0] == "\033[1;31;100mX\033[0m")
            return false;
        // Exiting if vehicle on edge
        if(y >= r.heigth())
            return false;
        // Checking if possible now
        for(int j=0; j<v.length(); j++) {
            if(x-j<0)
                continue;
            if(r.marks()[x-j][y] != "\033[1;100m \033[0m" && r.marks()[x-j][y] != "\033[1;33;100m_\033[0m")
                return false;
        }
        return true;
    }
    else {
        int x = v.pos().first;
        int y = v.pos().second-1;
        // Exiting if stopped because of red light
        if(r.marks()[x+1][0] == "\033[1;31;100m|\033[0m" || r.marks()[x+1][0] == "\033[1;31;100mX\033[0m")
            return false;
        // Exiting if vehicle on edge
        if(y < 0)
            return false;
        // Checking if possible now
        for(int j=0; j<v.length(); j++) {
            if(x-j<0)
                continue;
            if(r.marks()[x-j][y] != "\033[1;100m \033[0m" && r.marks()[x-j][y] != "\033[1;33;100m_\033[0m")
                return false;
        }
        return true;
    }
}
// Putting the vehicles in the predicted position
bool set_vehicle(Vehicle& temp, Road& r, mv direction) {
    // Removing vehicle if its last position has exited
    if(temp.last_pos().first+1 >= r.length()+temp.length())
            return false;
    // Resetting the current space occupied by vehicle
    for(int i=temp.pos().first; i>temp.pos().first-temp.length(); i--) {
        if(i >= r.length() || i < 0)
            continue;
        for(int j=temp.pos().second; j<temp.pos().second+temp.width(); j++)
            if(r.marks()[i][j] == "\033[1;31;100mX\033[0m")
                r.marks()[i][j] = "\033[1;31;100m|\033[0m";
            else if(r.marks()[i][j] != "\033[1;31;100m|\033[0m") {
                if(j == (r.heigth()-1)/2)
                    r.marks()[i][j] = "\033[1;33;100m_\033[0m";
                else
                    r.marks()[i][j] = "\033[1;100m \033[0m";
                }
    }
    if(direction == front) {
        // Updating the position
        temp.update_pos(pair<int, int>(temp.pos().first+1, temp.pos().second));
        // Removing the vehicle if out
        if(temp.pos().first+1 >= r.length()+temp.length()) {
            return true;
        }
    }
    else if(direction == rt) {
        temp.update_pos(pair<int, int>(temp.pos().first, temp.pos().second+1));
    }
    else {
        temp.update_pos(pair<int, int>(temp.pos().first, temp.pos().second-1));
    }
    // Continuing and placing the vehicle
    for(int i=temp.pos().first; i>temp.pos().first-temp.length(); i--) {
        if(i >= r.length() || i < 0)
            continue;
        for(int j=temp.pos().second; j<temp.pos().second+temp.width(); j++)
            if(r.marks()[i][j] != "\033[1;31;100m|\033[0m")
                r.marks()[i][j] = temp.sym();
            else
                r.marks()[i][j] = "\033[1;31;100mX\033[0m";
    }
    return true;
}
// Predicting the next positions of vehicles
void move_vehicles(Road& r) {
    if(r.current_vcls().size() == 0)
        return;
    // Sorting the vehicles giving priority to vehicles in front
    sort(r.current_vcls().begin(), r.current_vcls().end(), [ ](Vehicle& v1, Vehicle& v2) {
        return v1.pos().first > v2.pos().first;
    });
    // Looping over all vehicles
    for(auto it = r.current_vcls().begin(); it < r.current_vcls().end(); it++) {
        Vehicle& temp = *it;
        temp.update_last_pos(temp.pos());
        // Updating the speed for the vehicle
        temp.update_speed(min(temp.acc()+temp.speed(), temp.max_speed()));
        // Working on each step of the vehicle
        for(int i=0; i<temp.speed(); i++) {
            // Checking if movable, moving a step ahead if allowed
            if(movable(temp, r, front)) {
                if(!set_vehicle(temp, r, front)) {
                    r.current_vcls().erase(it);
                    it--;
                    break;
                }
            }
            // If movable in downward (right w.r.t. vehicle) direction, do once (eventual overtake)
            else if(movable(temp, r, rt) && (temp.last_move() != lmv::l)) {
                temp.update_move(lmv::r);
                set_vehicle(temp, r, rt);
                // if(i>0) {     // If blocked from ahead in the start only then 2 steps allowed, else 1
                    temp.update_speed(i+1);
                    break;
                // }
            }
            // If movable in downward (right w.r.t. vehicle) direction, do once (eventual overtake)
            else if(movable(temp, r, lt) && (temp.last_move() != lmv::r)) {
                temp.update_move(lmv::l);
                set_vehicle(temp, r, lt);
                // if(i>0) {     // If blocked from ahead in the start only then 2 steps allowed, else 1
                    temp.update_speed(i+1);
                    break;
                // }
            }
            // If not, stopping and setting the current movement as speed
            else {
                if(random()%100 < 5)
                    temp.update_move(lmv::f);
                temp.update_speed(i);
                break;
            }
        }
    }
}

#endif
