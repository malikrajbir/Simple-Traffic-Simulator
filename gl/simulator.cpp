#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "./include/road_auto.h"
#include "./include/vehicle_auto.h"
#include "./include/functions_auto.h"

#define GL_SILENCE_DEPRECATION

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;

// --------------------------------
// FILE POINTER FOR READING GLOBALLY
ifstream file;

// --------------------------------
// GLOBAL PARAMETERS FOR SIMULATION
// Road entity
Road road;

// Default parameters, defined in stage 2
int default_max_speed = 0;
int default_acceleration = 0;

// Vehicle categories, defined in stage 3
map<string, Vehicle> vehicle_types;

// --------------------------------
// FSM HANDLE FOR ANIMATION
// Boolean for signal
bool draw_signal = false;
// Boolean for further reading of config file
bool read_more = true;
// Loop handle (to take care of 'Pass' & END parameters)
bool loop = false;
int loop_pass = 0;
// Animation speed (usleep)
int animate_step = 600;

// For smooth animation, the fabrication loop
bool in_transit = false;
float loop_index = 0;
float fine_diff = 200.0; // The no. of fabrics per time


// --------------------------------
// CODE

// Predefinition for drawing funcation
void draw_frame();

// Increasing the time counter by 1s
// Making appropriate changes for that 1s (moving...)
void pass_time(Road& r) {
    in_transit = true;
    loop_index = fine_diff-1;
    move_vehicles(r);
    r.inc_time();
    draw_frame();
}

// Same as above but with vehicles add in place.
void add_v(Road& r, Vehicle v) {
    in_transit = true;
    loop_index = fine_diff-1;
    move_vehicles(r);
    add_vehicle(v, r);
    r.inc_time();
    draw_frame();
}

// Function, defining the FSM for processing the 'Configuration file'
void read_config() {
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
                road = Road(id,ln,wd,sig);
                return;
            }

            // ------------------------------------------------
            // Processing default parameters
                // DEFAULT SPEED
            else if(word.compare("Default_MaxSpeed") == 0) {
                token >> word;
                token >> word;
                default_max_speed = stoi(word);
                return;
            }
                // DEFAULT ACCELERATION
            else if(word.compare("Default_Acceleration") == 0) {
                token >> word;
                token >> word;
                default_acceleration = stoi(word);
                return;
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
                return;
            }

            // ------------------------------------------------
            // Start of time event
            else if(word.compare("START") == 0){
                // Calling draw for the first time, drawing only the road...
                draw_frame();
                return;
            }

            // ------------------------------------------------
            // Time runner
            else if(word.compare("Pass") == 0){
                // Creating a loop of time..
                token >> word;
                int tt = stoi(word);
                loop = true;
                loop_pass = tt;
                return;
            }

            // ------------------------------------------------
            // Change of road signal
            else if(word.compare("Signal") == 0) {
                // Taking signal input
                token >> word;
                road.set_signal(word);
                // Checking if red
                if(word.compare("RED") == 0) {
                    road.signal_red();
                    draw_signal = true;
                }
                // Checking if green
                else if(word.compare("GREEN") == 0) {
                    road.signal_green();
                    draw_signal = false;
                }
                // ERROR
                else {
                    cout<< "Undefined Signal"<<"\n";
                }
                // Calling draw for change of signal
                // draw_frame();
                return;
            }

            // ------------------------------------------------
            // End of control event (system left free for remaining ouput)
            else if (word.compare("END") == 0) {
                // If signal is red then nothing can be done so stop
                if(road.get_signal() == "RED")
                    break;
                // Otherwise run loop while road empty
                else if(road.get_signal() == "GREEN") {
                    // Calling time indefinitely
                    loop = true;
                    loop_pass = -1; // (-1) for indefinite loop...
                }
                // No need to read config file further
                read_more = false;
                return;
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
                // Adding vehicles, and calling for drawing...
                add_v(road, temp);
                return;
            }

            // ------------------------------------------------
            // No string matching - ERROR state - problem with config file
            else {
                cout << "Undefined control - "<<word <<"\n";
            }
        }
    }
}

// Function for handling the key input to increase or decrease animation speed
void speed_maintain( int key, int x, int y )
{
	switch ( key ) {
        // Up key
    	case GLUT_KEY_DOWN:
    		if ( animate_step < 200) {			// Avoid overflow problems
    			animate_step += 10;		// Increase the angle increment
    		}
    		break;
        // Down key
    	case GLUT_KEY_UP:
    		if (animate_step > 50) {		// Avoid underflow problems.
    			animate_step -= 10;	// Decrease the angle increment
    		}
    		break;
	}
}

void draw_vehicle(Vehicle& current) {
    // Vehicle current = *it;
    // Setting the color of vehicle
    if(current.real_color() == "RED")
        glColor4f(1, 0, 0, 1);
    else if(current.real_color() == "BLUE")
        glColor4f(0, 0, 1, 1);
    else if(current.real_color() == "GREEN")
        glColor4f(0, 1, 0, 1);
    else if(current.real_color() == "YELLOW")
        glColor4f(1, 1, 0, 1);
    else
        glColor4f(0, 0, 0, 1);
        // Getting the coordinates
        int xold = current.last_pos().first;
        int yold = road.heigth() - current.last_pos().second;
        int xnew = current.pos().first;
        int ynew = road.heigth() - current.pos().second;
        // Placing in the right fabric
        float x = xold*(loop_index/fine_diff)+xnew*(1.0 - (loop_index/fine_diff));
        float y = yold*(loop_index/fine_diff)+ynew*(1.0 - (loop_index/fine_diff));
    // First base
    glVertex3f(x-current.length()+0.1, y-0.1, 2);
    glVertex3f(x-0.5, y-0.1, 2);
    glVertex3f(x-0.5, y-current.width()+0.1, 2);
    glVertex3f(x-current.length()+0.1, y-current.width()+0.1, 2);
    // Second base
    glVertex3f(x-current.length()+0.1, y-0.4, 2);
    glVertex3f(x-0.1, y-0.4, 2);
    glVertex3f(x-0.1, y-current.width()+0.4, 2);
    glVertex3f(x-current.length()+0.1, y-current.width()+0.4, 2);
    // Head lights (Trapezoid)
    glColor4f(1, 1, 0, 1); // Yellow color for head lights
    glVertex3f(x-0.5, y-0.1, 2);
    glVertex3f(x-0.1, y-0.4, 2);
    glVertex3f(x-0.1, y-current.width()+0.4, 2);
    glVertex3f(x-0.5, y-current.width()+0.1, 2);
    return;
}

// The draw function used to draw the complete layout, independent of the GLUTMainLoop
void draw_frame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Road hasn't been initialised (NOT NEEDED)
    if(road.length() == 0)
        return;

    // Drawing the road...
    glBegin(GL_QUADS);
    glColor4f(0.7, 0.7, 0.7, 0.5);
    glVertex3i(0, road.heigth(), 0);
    glVertex3i(road.length(), road.heigth(), 0);
    glVertex3i(road.length(), 0, 0);
    glVertex3i(0, 0, 0);

    // Drawing the zebra-crossing
    glColor4f(1, 1, 1, 1);
    for(float i=0; i<=road.heigth(); i+=1.5) {
        glVertex3f(road.signal_position()-0.5, i, 1);
        glVertex3f(road.signal_position()+1.5, i, 1);
        glVertex3f(road.signal_position()+1.5, i+1, 1);
        glVertex3f(road.signal_position()-0.5, i+1, 1);
    }

    // Drawing the divider
    glColor4f(1, 1, 0, 0.15);
    for(float i=0.5; i<=road.length(); i+=2) {
        if(i>road.signal_position()-1.5 && i<road.signal_position()+1.5)
            continue;
        glVertex3f(i, road.heigth()/2.0-0.1, 1);
        glVertex3f(i+1, road.heigth()/2.0-0.1, 1);
        glVertex3f(i+1, road.heigth()/2.0+0.1, 1);
        glVertex3f(i, road.heigth()/2.0+0.1, 1);
    }

    if(draw_signal)  {// Draw the red light signal
        glColor4f(1, 0, 0, 2);
        glVertex3f(road.signal_position()-0.7, road.heigth(), 2);
        glVertex3f(road.signal_position()-0.4, road.heigth(), 2);
        glVertex3f(road.signal_position()-0.4, 0, 2);
        glVertex3f(road.signal_position()-0.7, 0, 2);
    }

    // Drawing the vehicles
    for(auto it = road.current_vcls().begin(); it<road.current_vcls().end(); it++) {
        draw_vehicle(*it);
    }
    // Ending the deisgnation
    glEnd();

    // Flushing the pipeline
    glFlush();
    // Swapping the buffers
    glutSwapBuffers();
    usleep(animate_step);
    return;
}

// The default redrawing function for LOOP
void drawScene(void)
{
    // Reading from the config file and performing the desired action
    if(read_more && !loop && !in_transit) {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        read_config();
    }
    if(loop && !in_transit) {
        if(loop_pass < 0)
            if(road.current_vcls().size() == 0)
                exit(0);
        pass_time(road);
        loop_pass--;
        if(loop_pass == 0)
            loop = false;
    }
    if(in_transit) {
        loop_index--;
        draw_frame();
        if(loop_index == 0)
            in_transit = false;
    }
    // Trigger automatic redraw for animation...
    glutPostRedisplay();
}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glShadeModel(GL_SMOOTH);    // Shade model
    glEnable(GL_DEPTH_TEST);    // Depth testing is turned on
}

// Called when the window is resized
void resizeWindow(int w, int h)
{
    // Define the portion of the window used for OpenGL rendering.
    glViewport(0, 0, w, h);    // View port uses whole window

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, road.length()-2, 0, road.heigth(), -2, 2);
}

// Function for error indication in command line
void show_error_exit(int pass = 0) {
    if(pass) // Illegal flag indicated
    {
        cerr << "Illegal flag present.\n";
    }
    else {
        cerr << "Flags not correctly set.\n";
        cerr << "./opengl -in <infile> [-q <time_diff>]\n" ;
    }
    exit(0);
}


// The main function
int main(int argc, char** argv) {
    int fi=0;

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
        else if(arg == "-q")
        // Lapse time
        {
            if(i == argc-1)
                show_error_exit();
            fine_diff = stof(argv[++i]);        
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

    // Processing further if the file is in place
    if(file.is_open()) {
        read_config();    // This read shall establish the road details, now we can design the window with road prefferences
    }
    else {
        cout<<"Unable to open file : "<<argv[fi]<<"\n";
    }

    int x = road.length();
    int y = road.heigth();

    // ANIMATION PART WITH OPEN GL
    // Initialising OpenGL
    glutInit(&argc,argv);
    // Double buffer for animation
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Window position (from top corner), and size (width and height)
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1500, 1500*y/x);
    glutCreateWindow("Traffic Simulation");
    // Set up callback functions for key presses
	glutSpecialFunc(speed_maintain);
    // Initialising OpenGL rendering
    initRendering();
    // Function for redrawing
    glutDisplayFunc(drawScene);
    // Function for resizing windows
    glutReshapeFunc(resizeWindow);
    // Main loop for animation
    glutMainLoop();
}
