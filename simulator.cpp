#include <iostream>

#include <fstream>
#include <sstream>

#include <pthread.h>

#include <vector>
#include <map>

#include "include/pln/road.h"
#include "include/pln/block.h"
#include "include/vehicle.h"

using namespace std;

int main(int argc, char const *argv[])
{
    map<string, vector<int>> vhs;

    Road rd = Road(30, 5, 15);

    int msp = 1, mac = 1;

    vhs.insert(pair<string, int>("Car", vector<int>{2, 2, 1, 1}));
    vhs.insert(pair<string, int>("bike", vector<int>{2, 1, 1, 1}));
    vhs.insert(pair<string, int>("Bus", vector<int>{3, 2, 1, 1}));
    vhs.insert(pair<string, int>("Truck", vector<int>{4, 2, 1, 1}));

    // Start
    rd.signal_red();
    sleep(1);
    rd.show();
    Vehicle('C', vhs["Car"], &rd);
    sleep(1);
    rd.time_step();
    Vehicle('b', vhs["Bike"], &rd);
    sleep(1);
    rd.time_step();
    for(int i=0; i<15; i++) {
        sleep(1);
        rd.time_step();
    }
    rd.signal_green();
    sleep(1);
    rd.show();
    for(int i=0; i<15; i++) {
        sleep(1);
        rd.time_step();
    }
    return 0;
}
