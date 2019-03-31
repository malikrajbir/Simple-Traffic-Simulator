## Traffic Simulator

A program to simulate simpleton traffic on the terminal or using a GUI API (OpenGL).

#### Configuration File

It takes in a _configuration file_ and processes it to output various simulation results. The format of configuration file is as followed

- **Road Features**
  - This includes four road parameters, namely, *length*, *height*, *signal-position* and *ID*
- **Default Parameters**
  - These include the default *acceleration* and *velocity.*
- **Vehicle Categories**
  - Here features of vehicles are defined.
  - These vehicles are called later during the simulation.
- **Time commands**
  - This is the main simulation. It controls the inflow of vehicles and handling of traffic lights.
  - It has three keywords
    - *START* - To commence the simulation
    - *END* - To end the simulation
    - *Pass* - To leap through time in the simulation



#### Compiling Instructions

Two formats are provided to run the code.

- **Terminal Mode**

  In this case the simulation is output on the terminal. The code is contained in the folder `tmnl`.

  Compiling : `cd tmnl && make`

  Running : `./terminal -in <infile> [-out <outfile>] [-lapse <sleeptime>]`

- **Animation Mode**

  In this case the simulation is output using the *OpenGL* API. The code is contained in the folder `gl`.

  Compiling : `cd gl && make`

  Running : `./opengl -in <infile> [-d <time_diff>]`

- **AUTO Mode**
  This case is similar to above, except for state transitions/vehicle-entry is determined by the user input from the keyboard. The configuration file for this case remains the same without the _Time Commands_.

  Compiling : `cd gl && make auto`

  Running : `./auto -in <infile> [-d <time_diff>]`

  Also, during the simulation the `up` and `down` keys can be used to adjust simulation speed.
