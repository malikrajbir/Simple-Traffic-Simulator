#include <iostream>

// Collections
#include <vector>
#include <map>

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
    bool good, set = false;
    for(int i=0; i<r.heigth(); i++) {
        good = true;
        for(int j=0; j<v.width(); j++)
            good = (good)&&(r.marks()[0][i+j] == ' ');
        if(good) {
            v.update_pos(pair<int, int>(0, i));
            set = true;
            break;
        }
    }
    if(!set)
        throw runtime_error("Not possible for vehicle to enter");
    r.current_vcls().push_back(v);
}

// PROCESS
// Checking the status of vehicles movement
// The 'direction' parameter checks for the direction of movement
bool movable(Vehicle& v, Road& r, mv direction) {
    if(direction == front) {
        int x = v.pos().first+1;
        int y = v.pos().second;
        // If exiting (no other ahead)
        if(x >= r.length())
            return true;
        // checking the next complete row
        for(int j=0; j<v.width(); j++)
            if(r.marks()[x][y+j] != ' ')
                return false;
        return true;
    }
    else if (direction == rt) {
        int x = v.pos().first;
        int y = v.pos().second+v.width();
        // Exiting if stopped because of red light
        if(r.marks()[x+1][0] == '=')
            return false;
        // Exiting if vehicle on edge
        if(y >= r.heigth())
            return false;
        // Checking if possible now
        for(int j=0; j<v.length(); j++) {
            if(x-j<0)
                continue;
            if(r.marks()[x-j][y] != ' ')
                return false;
        }
        return true;
    }
    else {
        int x = v.pos().first;
        int y = v.pos().second-v.width();
        // Exiting if stopped because of red light
        if(r.marks()[x+1][0] == '=')
            return false;
        // Exiting if vehicle on edge
        if(y < 0)
            return false;
        // Checking if possible now
        for(int j=0; j<v.length(); j++) {
            if(x-j<0)
                continue;
            if(r.marks()[x-j][y] != ' ')
                return false;
        }
        return true;
    }
}
// Putting the vehicles in the predicted position
bool set_vehicle(Vehicle& temp, Road& r, mv direction) {
    // Resetting the current space occupied by vehicle
    for(int i=temp.pos().first; i>temp.pos().first-temp.length(); i--) {
        if(i >= r.length() || i < 0)
            continue;
        for(int j=temp.pos().second; j<temp.pos().second+temp.width(); j++)
            r.marks()[i][j] = ' ';
    }
    if(direction == front) {
        // Updating the position
        temp.update_pos(pair<int, int>(temp.pos().first+1, temp.pos().second));
        // Removing the vehicle if out
        if(temp.pos().first >= r.length()+temp.length()) {
            return false;
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
            r.marks()[i][j] = temp.sym();
    }
    return true;
}
// Predicting the next positions of vehicles
void move_vehicles(Road& r) {
    if(r.current_vcls().size() == 0)
        return;
    // Looping over all vehicles
    for(auto it = r.current_vcls().begin(); it < r.current_vcls().end(); it++) {
        Vehicle& temp = *it;
        // Updating the speed for the vehicle
        temp.update_speed(min(temp.acc()+temp.speed(), temp.max_speed()));
        // Working on each step of the vehicle
        for(int i=0; i<temp.speed(); i++)
            // Checking if movable, moving a step ahead if allowed
            if(movable(temp, r, front)) {
                if(!set_vehicle(temp, r, front)) {
                    r.current_vcls().erase(it);
                    it--;
                    break;
                }
            }
            // If movable in downward (right w.r.t. vehicle) direction, do once (eventual overtake)
            else if(movable(temp, r, rt)) {
                set_vehicle(temp, r, rt);
                if(i>0) {     // If blocked from ahead in the start only then 2 steps allowed, else 1
                    temp.update_speed(i+1);
                    break;
                }
            }
            // If movable in downward (right w.r.t. vehicle) direction, do once (eventual overtake)
            else if(movable(temp, r, lt)) {
                set_vehicle(temp, r, lt);
                if(i>0) {     // If blocked from ahead in the start only then 2 steps allowed, else 1
                    temp.update_speed(i+1);
                    break;
                }
            }
            // If not, stopping and setting the current movement as speed
            else {
                temp.update_speed(i);
                break;
            }
    }
}

#endif