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

/* THIS FILE IS FOR INDEPENDENT RUN
    HERE, WE ONLY NEED TO DEFINE THE VEHICLE CATEGORIES AND THE REST OF THE SIMULATION WORKS WITH
    OUR INPUT FROM THE KEYBORAD...
*/


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
// The running check, allows whether to change the screen or not
bool live = false;
bool paused = false;
// Animation speed (usleep)
float animate_step = 10;

// For smooth animation, the fabrication loop
bool loop_set = false;
float loop_index = 0;
vector<char> vehicle_buffer{};

// --------------------------------
// CODE

// Predefinition for drawing funcation
void draw_frame();

// Increasing the time counter by 1s
// Making appropriate changes for that 1s (moving...)
void pass_time(Road& r) {
    move_vehicles(r);
    r.inc_time();
    // draw_frame();
}

// Add vehicles with previous configuration in place.
void add_v(Road& r, Vehicle v) {
    // Try adding vehicle, if not possible dont add...
    try {
        add_vehicle(v, r);
    }
    catch(...) {}
    r.inc_time();
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
                vehicle_types.insert(pair<string, Vehicle>(string(1, vtype[0]), Vehicle(param, symbol)));
                return;
            }

            // ------------------------------------------------
            // Start of time event
            else if(word.compare("START") == 0){
                // Calling draw for the first time, drawing only the road...
                live = true;
                read_more = false;
                draw_frame();
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
    		if ( animate_step < 50) {			// Avoid overflow problems
    			animate_step *= 2.0;		// Increase the angle increment
    		}
    		break;
        // Down key
    	case GLUT_KEY_UP:
    		if (animate_step > 0.005) {		// Avoid underflow problems.
    			animate_step /= 2.0;	// Decrease the angle increment
    		}
    		break;
	}
}

// Ading vehicles after smooth loop is complete, on action of a key
void add_vehicle_key(char key) {
    Vehicle temp = vehicle_types.find(string(1, key))->second;
    int i = random();
    if(i%4 == 0)
        temp.set_color("RED");
    else if(i%4 == 1)
        temp.set_color("GREEN");
    else if(i%4 == 2)
        temp.set_color("BLUE");
    else
        temp.set_color("YELLOW");
    // Add vehicle
    add_v(road, temp);
}

// Function for running the simulation, reading in the user inputs
void input_key( unsigned char key, int x, int y )
{
    string word;
	switch (key) {
        //----------------------------------------
        case 32:    // Space key
            // SIGNAL REVERT
            word = road.get_signal();
            if(word.compare("RED") != 0) {      // Signal is green
                    road.signal_red();
                    road.set_signal("RED");
                    draw_signal = true;
            }
            else {                      // Signal is red
                    road.signal_green();
                    road.set_signal("GREEN");
                    draw_signal = false;
            }
            break;
        //----------------------------------------
        case 13:
            paused = !paused;
            break;
        //----------------------------------------
        case 27:	// Escape key
            exit(1);
        //----------------------------------------
        default:    // All other keys (existing in vehicles)
            if(vehicle_types.find(string(1, key)) == vehicle_types.end())
                return;
            else {
                vehicle_buffer.push_back(key);
            }
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

    float x = xold*(loop_index/10.0)+xnew*(1.0 - (loop_index/10.0));
    float y = yold*(loop_index/10.0)+ynew*(1.0 - (loop_index/10.0));
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
    cout << loop_index << "\n";
    // Reading from the config file and performing the desired action
    if(read_more) {
        // Clear the rendering window
        read_config();
    }
    if(live && !paused && !loop_set) {
        pass_time(road);
        loop_set = true;
        loop_index = 9;
    }
    if(loop_set) {
        loop_index--;
        draw_frame();
        if(loop_index == 0) {
            loop_set = false;
            for(auto it=vehicle_buffer.begin(); it<vehicle_buffer.end(); it++)
                add_vehicle_key(*it);
            vehicle_buffer.clear();
        }
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
    glOrtho(0, road.length(), 0, road.heigth(), -2, 2);
}


// The main function
int main(int argc, char** argv) {

    // Taking the file input, for processing
    string filename = argv[1];
    file = ifstream(filename);

    read_config();  // This read shall establish the road details, now we can design the window with road prefferences

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
    glutKeyboardFunc(input_key);
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
