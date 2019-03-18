## Traffic Simulator

Working of a simple traffic simulator.

#### Configuration File

It takes in a _configuration file_ and processes it to output various simulation results. The format of configuration file is as followed

- **Road Features**
  - This includes four road parameters, namely, *length*, *height*, *signal-position* and *ID*
- **Default Parameters**
  - These include the default *acceleration* and *velocity.*
- **Vehicle Categories**
  - Here features of vehicles are defined.
  - These vehicles are called later during the simualtion.
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

  Running : `./terminal <config_file>`

- **Animation Mode**

  In this case the simulation is output using the *OpenGL* API. The code is contained in the folder `gl`.

  Compiling : `cd gl && make`

  Running : `./opengl <config_file>`

  Also, during the simulation the `up` and `down` keys can be used to adjust simulation speed.

### Interactive Animation Module

We have also designed a model where the configuration file provides information only till vehicle categories. The time commands are provided manually by the user inputting from the keyboard.
This gives more power to the user to test the simulation and *enjoy!*