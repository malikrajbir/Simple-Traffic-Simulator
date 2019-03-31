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
// The default lapse time
int lapse = 150;

// Increasing the time counter by 1s
// Making appropriate changes for that 1s (moving, printing...)
void pass_time(Road& r, ofstream& out) {
    move_vehicles(r);
    r.inc_time();
    r.show_road(lapse);
    r.wrt_road(out);
}

// Same as above but with vehicles add in place.
void add_v(Road& r, Vehicle v, ofstream& out) {
    move_vehicles(r);
    add_vehicle(v, r);
    r.inc_time();
    r.show_road(lapse);
    r.wrt_road(out);
}

// Function, defining the FSM for processing the 'Configuration file'
void process(ifstream& file, ofstream& out) {
    // Default parameters, defined in stage 2
    int default_max_speed = 0;
    int default_acceleration = 0;
    // Vehicle categories, defined in stage 3
    map<string, Vehicle> vehicle_types;
    // The main road(platform), defined in stage 1
    Road r;
    // General Token
    string line;

    while( getline(file,line) ) {
        // Free line
        if(line.length() == 0)
            continue;
        // Comment
        if(line.at(0)=='#')
            continue;

        istringstream token(line);  // The tokeniser
        string word;    // The latest token

        // Processing the line
        while(token >> word) {
            // Comment start in a line
            if(word.at(0)=='#')
                break;

            // ------------------------------------------------
            // Processing a ROAD
            if(word.compare("Road_Id") == 0) {
                // Extra '='
                token >> word;
                // ID
                token >> word;
                int id = stoi(word);
                int ln, wd, sig;
                for(int i=0; i<3; i++) {
                    // Processing the content
                    if(getline(file,line)){
                        istringstream token(line);
                        string word;
                        token >> word;
                        // Proccessing road length
                        if(word.compare("Road_Length") == 0) {
                            token >> word;
                            token >> word;
                            ln = stoi(word);
                        }
                        // Processing road width
                        else if(word.compare("Road_Width") == 0) {
                            token >> word;
                            token >> word;
                            wd = stoi(word);
                        }
                        // Processing road signal-coordinate
                        else if(word.compare("Road_Signal") == 0) {
                            token >> word;
                            token >> word;
                            sig = stoi(word);
                        }
                        // Unkown keyword - ERROR
                        else {
                            cout<<"Incorrect Parameters for Road "<<id<<"\n";
                        }
                    }
                    else {
                        // No more line possible, incomplete config file
                        cout<<"Incomplete Parameters for Road "<<id<<"\n";
                    }
                }

                // Designating and creating the road
                r = Road(id,ln,wd,sig);
                break;
            }

            // ------------------------------------------------
            // Processing default parameters
                // DEFAULT SPEED
            else if(word.compare("Default_MaxSpeed") == 0) {
                token >> word;
                token >> word;
                default_max_speed = stoi(word);
            }
                // DEFAULT ACCELERATION
            else if(word.compare("Default_Acceleration") == 0) {
                token >> word;
                token >> word;
                default_acceleration = stoi(word);
            }

            // ------------------------------------------------
            // Processing a new vehicle
            else if(word.compare("Vehicle_Type") == 0) {
                token >>  word;

                // Designating symbol and name to the type
                token >> word;
                string symbol = string(1, word.at(0));
                string vtype = word;

                vector<int> param{0, 0, -1, -1};    // Default parameters list

                for(int i=0; i<4; i++){
                    // Processing the content
                    if(getline(file,line)){
                        // Inputting file, if empty set default
                        istringstream token(line);
                        if(line.size()==0) {
                            param[2] = default_max_speed;
                            param[3] = default_acceleration;
                            break;
                        }
                        // Taking details
                        string word;
                        token >> word;
                        // Length of the vehicles
                        if(word.compare("Vehicle_Length") == 0) {
                            token >> word;
                            token >> word;
                            param[0] = stoi(word);
                        }
                        // Width of the vehicles
                        else if(word.compare("Vehicle_Width") == 0) {
                            token >> word;
                            token >> word;
                            param[1] = stoi(word);
                        }
                        // Max speed for the vehicle
                        else if(word.compare("Vehicle_MaxSpeed") == 0) {
                            token >> word;
                            token >> word;
                            param[2] = stoi(word);
                        }
                        // Acceleration of the vehicle
                        else if(word.compare("Vehicle_Acceleration") == 0) {
                            token >> word;
                            token >> word;
                            param[3] = stoi(word);
                        }
                        // Unkown keyword - ERROR
                        else {
                            cout<<"Incorrect Parameters for Vehicle Type "<<vtype<<"\n";
                        }
                    }
                    // Incomplete config file - ERROR
                    else {
                        cout<<"Incomplete Parameters for Vehicle Type "<<vtype<<"\n";
                    }
                }
                // Addiing the vehicle created to the vehicle type
                vehicle_types.insert(pair<string, Vehicle>(vtype, Vehicle(param, symbol)));
                break;
            }

            // ------------------------------------------------
            // Start of time event
            else if(word.compare("START") == 0){
                // Nothing to do here.
            }

            // ------------------------------------------------
            // Time runner
            else if(word.compare("Pass") == 0){
                // Creating a loop of time..
                token >> word;
                int tt = stoi(word);
                while(tt--)
                    pass_time(r, out);
            }

            // ------------------------------------------------
            // Change of road signal
            else if(word.compare("Signal") == 0) {
                // Taking signal input
                token >> word;
                r.set_signal(word);
                // Checking if red
                if(word.compare("RED") == 0) {
                    r.signal_red();
                }
                // Checking if green
                else if(word.compare("GREEN") == 0) {
                    r.signal_green();
                }
                // ERROR
                else {
                    cout<< "Undefined Signal"<<"\n";
                }
            }

            // ------------------------------------------------
            // End of control event (system left free for remaining ouput)
            else if (word.compare("END") == 0) {
                // If signal is red then nothing can be done so stop
                if(r.get_signal() == "RED")
                    break;
                // Otherwise run loop while road empty
                else if(r.get_signal() == "GREEN") {
                    while(r.current_vcls().size() != 0)
                        pass_time(r, out);
                }
            }

            // ------------------------------------------------
            // Creating an instance of vehicle and putting on the road
            else if(vehicle_types.count(word)) {
                // Taking out the parameters
                Vehicle temp = vehicle_types.find(word)->second;
                // Setting the color
                token >> word;
                temp.set_color(word);
                temp.color_sym(word);
                // Adding vehicles
                add_v(r, temp, out);
            }

            // ------------------------------------------------
            // No string matching - ERROR state - problem with config file
            else {
                cout << "Undefined control - "<<word <<"\n";
            }
        }
    }
}

// Function for error indication in command line
void show_error_exit(int pass = 0) {
    if(pass) // Illegal flag indicated
    {
        cerr << "Illegal flag present.\n";
    }
    else {
        cerr << "Flags not correctly set.\n";
        cerr << "./opengl -in <infile> [-out <outfile>] [-lapse <sleeptime>]\n" ;
    }
    exit(0);
}

// The main function
int main(int argc, char const *argv[]) {
    bool out_set = false;
    int fi=0;

    ifstream file;
    ofstream out;

    // Processing the terminal flags
    for(int i=1; i<argc; i++) {
        string arg = argv[i];
        if(arg == "-in")
        // Input file
        {
            if(i == argc-1)
                show_error_exit();
            file = ifstream(argv[++i]);
            fi = i;  
        }
        else if(arg == "-out")
        // Output file
        {
            if(i == argc-1)
                show_error_exit();
            out = ofstream(argv[++i]);
            out_set = true;
        }
        else if(arg == "-lapse")
        // Lapse time
        {
            if(i == argc-1)
                show_error_exit();
            lapse = stoi(argv[++i]);
        }
        else
        // Invalid Flag
        {
            cout << arg << "\n";
            show_error_exit(1);
        }
    }

    // -in flag not in command line
    if(fi == 0) {
        cout << "No input (config) file indicated. Exiting...\n";
        exit(0);
    }

    // -out flag not in command line, open default out file
    if(!out_set) {
        out = ofstream("out.txt");
    }

    // Processing further if the file is in place
    if(file.is_open()) {
        process(file, out);
    }
    else {
        cout<<"Unable to open file : "<<argv[fi]<<"\n";
    }
}
