#include <iostream>
#include <map>
#include <vector>

#include "include/road.h"
#include "include/vehicle.h"
#include "include/functions.h"

using namespace std;

// NOT PREPARED CURRENTLY ONLY FOR TESTING

void pass_time(Road& r) {
    move_vehicles(r);
    r.inc_time();
    r.show_road();
}

void add_v(Road& r, Vehicle v) {
    move_vehicles(r);
    add_vehicle(v, r);
    r.inc_time();
    r.show_road();
}

int main(int argc, char const *argv[]) {
    // Setting the road
    Road r = Road(30, 8, 15);
    // Setting the vehicles
    Vehicle car1 = Vehicle({2, 2, 3, 1}, 'c');
    Vehicle car2 = Vehicle({2, 2, 1, 1}, 'c');
    Vehicle bike = Vehicle({2, 1, 2, 1}, 'b');
    Vehicle Bus = Vehicle({3, 2, 1, 1}, 'B');
    Vehicle Truck = Vehicle({4, 2, 2, 1}, 'T');

    add_v(r, car1);
    add_v(r, bike);
    add_v(r, Bus);
    for(int i=0; i<10; i++)
        pass_time(r);
    r.signal_red();
    for(int i=0; i<10; i++)
        pass_time(r);
    r.signal_green();
    add_v(r, Truck);
    add_v(r, car2);
    for(int i=0; i<10; i++)
        pass_time(r);
    return 0;
}
