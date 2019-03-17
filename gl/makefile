# Linux (default)
EXE = opengl
LDFLAGS = -lGL -lGLU -lglut

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
    EXE = opengl.exe
        LDFLAGS = -lopengl32 -lglu32 -lglut32
endif

# OS X
ifeq "$(OSTYPE)" "darwin"
        LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
endif

$(EXE) : simulator.cpp
		@g++ -o $@ $< $(CFLAGS) $(LDFLAGS)