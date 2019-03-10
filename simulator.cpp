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
ofstream out_file;

// Increasing the time counter by 1s
// Making appropriate changes for that 1s (moving, printing...)
void pass_time(Road& r, ofstream& out) {
    move_vehicles(r);
    r.inc_time();
    r.show_road();
    r.show_road(out);
}

// Same as above but with vehicles add in place.
void add_v(Road& r, Vehicle v, ofstream& out) {
    move_vehicles(r);
    add_vehicle(v, r);
    r.inc_time();
    r.show_road();
    r.show_road(out);
}

// The main function
int main(int argc, char const *argv[]) {
    // Default parameters, defined in stage 2
    int default_max_speed = 0;
    int default_acceleration = 0;
    // Vehicle categories, defined in stage 3
    map<string, Vehicle> vehicle_types;
    // The main road(platform), defined in stage 1
    Road r;

    // Taking the file input, for processing
    string filename = "config.ini";
    ifstream file(filename);
    string line;

    // Output stream for
    ofstream out("out.txt");

    // Processing further if the file is in place
    if(file.is_open()) {
        while( getline(file,line) ) {
            // Comment or free line
            if(line.length() == 0)
                continue;
            if(line.at(0)=='#')
                continue;

            istringstream token(line);  // The tokeniser
            string word;    // The latest token

            // Processing the line
            while(token >> word) {
                // Comment start in a line
                if(word.at(0)=='#')
                    break;

                // Processing a ROAD
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
                            else if(word.compare("Road_Width") == 0) {
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
                                cout<<"Incorrect Parameters for Road "<<id<<"\n";
                            }
                        }
                        else {
                            cout<<"Incomplete Parameters for Road "<<id<<"\n";
                        }
                    }

                    r = Road(id,ln,wd,sig);
                    break;
                }

                // Processing default parameters
                else if(word.compare("Default_MaxSpeed") == 0) {
                    token >> word;
                    token >> word;
                    default_max_speed = stoi(word);
                }
                else if(word.compare("Default_Acceleration") == 0) {
                    token >> word;
                    token >> word;
                    default_acceleration = stoi(word);
                }

                // Processing a new vehicle
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
                            else if(word.compare("Vehicle_Width") == 0) {
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
                                cout<<"Incorrect Parameters for Vehicle Type "<<vtype<<"\n";
                            }
                        }
                        else {
                            cout<<"Incomplete Parameters for Vehicle Type "<<vtype<<"\n";
                        }
                    }

                    vehicle_types.insert(pair<string, Vehicle>(vtype, Vehicle(param, symbol)));
                    break;
                }
                else if(word.compare("START") == 0){
                    // Nothing to do
                }

                else if(word.compare("Pass") == 0){
                    token >> word;
                    int tt = stoi(word);
                    while(tt--)
                        pass_time(r, out);
                }

                else if(word.compare("Signal") == 0) {
                    token >> word;
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
                else if (word.compare("END") == 0) {
                    if(r.get_signal() == "RED")
                        break;
                    else if(r.get_signal() == "GREEN") {
                        while(r.current_vcls().size() != 0)
                            pass_time(r, out);
                    }
                }

                else if(vehicle_types.count(word)) {
                    Vehicle temp = vehicle_types.find(word)->second;
                    token >> word;
                    temp.set_color(word);
                    add_v(r, temp, out);
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
