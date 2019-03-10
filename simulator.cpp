#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

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
    Road r = Road(1,30, 8, 15);
    // Setting the vehicles
    Vehicle car1 = Vehicle({2, 2, 3, 1}, 'c');
    Vehicle car2 = Vehicle({2, 2, 1, 1}, 'c');
    Vehicle bike = Vehicle({2, 1, 2, 1}, 'b');
    Vehicle Bus = Vehicle({3, 2, 1, 1}, 'B');
    Vehicle Truck = Vehicle({4, 2, 3, 1}, 'T');

    add_v(r, bike);
    add_v(r, Bus);
    add_v(r, car1);
    add_v(r, bike);
    add_v(r, Bus);
    for(int i=0; i<10; i++)
        pass_time(r);
    r.signal_red();
    add_v(r, Truck);
    add_v(r, car1);
    add_v(r, bike);
    add_v(r, Bus);
    for(int i=0; i<10; i++)
        pass_time(r);
    r.signal_green();
    add_v(r, bike);
    add_v(r, Bus);
    add_v(r, Truck);
    add_v(r, car2);
    while(r.current_vcls().size() != 0)
        pass_time(r);
    return 0;

    int default_max_speed = 0;
    int default_acceleration = 0;
    vector<Road> roads;
    map<string, Vehicle> vehicle_types;

    string filename = "config.ini";
    ifstream file;
    file.open(filename);
    string line;

    if(file.is_open()) {
        while( getline(file,line) ) {
            // Comment or free line
            if(line.at(0) == '#' || line.length() == 0)
                continue;

            istringstream token(line);  // The tokeniser
            string word;    // The latest token

            while(token >> word) {
                if(word.at(0)=='#')
                    break;

                if(word.compare("Road_Id") == 0) {
                    token >> word;
                    token >> word;
                    int id = stoi(word);
                    int ln, wd, sig;
                    for(int i=0; i<3; i++) {
                        if( getline(file,line) ){
                            istringstream token(line);
                            string word;
                            token >> word;

                            if(word.compare("Road_Length") == 0) {
                                token >> word;
                                token >> word;
                                ln = stoi(word);
                            }
                            else if(word.compare("Road_width") == 0) {
                                token >> word;
                                token >> word;
                                wd = stoi(word);
                            }
                            else if(word.compare("Road_Signal") == 0) {
                                token >> word;
                                token >> word;
                                sig = stoi(word);
                            }
                            else {
                                cout<<"Incomplete Parameters for Road "<<id<<"\n";
                            }
                        }
                        else {
                            cout<<"Incomplete Parameters for Road "<<id<<"\n";
                        }
                    }

                    roads.push_back(Road(id,ln,wd,sig));
                    break;
                }

                else if(word.compare("Default_MaxSpeed")) {
                    token >> word;
                    token >> word;
                    default_max_speed = stoi(word);
                }
                else if(word.compare("Default_Acceleration")) {
                    token >> word;
                    token >> word;
                    default_acceleration = stoi(word);
                }

                else if(word.compare("Vehicle_Type") == 0) {
                    token >>  word;
                    token >> word;
                    char symbol = word.at(0);
                    string vtype = word;

                    vector<int> param{0, 0, -1, -1};

                    for(int i=0; i<4; i++){
                        if( getline(file,line) ){

                            istringstream token(line);
                            if(line.size()==0) {
                                param[2] = default_max_speed;
                                param[3] = default_acceleration;
                                break;
                            }

                            string word;
                            token >> word;

                            if(word.compare("Vehicle_Length") == 0) {
                                token >> word;
                                token >> word;
                                param[0] = stoi(word);
                            }
                            else if(word.compare("Vehicle_width") == 0) {
                                token >> word;
                                token >> word;
                                param[1] = stoi(word);
                            }
                            else if(word.compare("Vehicle_MaxSpeed") == 0) {
                                token >> word;
                                token >> word;
                                param[2] = stoi(word);
                            }
                            else if(word.compare("Vehicle_Acceleration") == 0) {
                                token >> word;
                                token >> word;
                                param[3] = stoi(word);
                            }
                            else {
                                cout<<"Incomplete Parameters for Vehicle Type "<<vtype<<"\n";
                            }
                        }
                        else {
                            cout<<"Incomplete Parameters for Vehicle Type "<<vtype<<"\n";
                        }
                    }

                    vehicle_types.insert(pair<string, Vehicle>(vtype, Vehicle(param, symbol)));
                    break;
                }
                else if(word.compare("START")){

                }
                else if(word.compare("PASS")){

                }
                else if(vehicle_types.count(word)) {
                    token >> word; // Color of the vehicle
                    // Further processing of the vehicle
                }
                else if(word.compare("Signal")) {
                    token >> word;
                    Road r = roads.back();
                    r.set_signal(word);
                    if(word.compare("RED") == 0) {
                        r.signal_red();
                    }
                    else if(word.compare("GREEN") == 0) {
                        r.signal_green();
                    }
                    else {
                        cout<< "Undefined Signal"<<"\n";
                    }
                }
                else if (word.compare("END")) {
                    Road r = roads.back();
                    if(r.get_signal() == "RED")
                        break;
                    else if(r.get_signal() == "GREEN") {
                        while(r.current_vcls().size() != 0)
                            pass_time(r);
                    }
                }
                else {
                    cout << "Undefined Control- "<<word <<"\n";
                }
            }
        }
    }
    else {
        cout<<"Unable to open File- "<<filename<<"\n";
    }
}
