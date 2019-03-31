# MAKE FILE
OPEN = opengl # tag for opengl basic
AUTO = auto # tag for basic
#------------------------------------
# Linux (Default)
LDFLAGS = -lGL -lGLU -lglut
#------------------------------------
# Windows (cygwin)
ifeq ($(OS), "Windows_NT")
		AUTO = auto.exe
        OPEN = opengl.exe
        LDFLAGS = -lopengl32 -lglu32 -lglut32
endif
#------------------------------------
# OS X
ifeq "$(shell uname)" "Darwin"
        LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
endif
#------------------------------------
# COMPILE STAGE
$(OPEN) : simulator.cpp
		@g++ -std=c++11 -o $@ $< $(CFLAGS) $(LDFLAGS)
#------------------------------------
$(AUTO) : automated.cpp
		@g++ -std=c++11 -o $@ $< $(CFLAGS) $(LDFLAGS)
#------------------------------------
# CLEANING
clean:
		@rm $(OPEN) $(AUTO)
