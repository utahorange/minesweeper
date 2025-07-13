CC = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -pthread

# INCLUDES = -I/usr/X11/include/GL
# LIBS = -L/usr/X11/lib -lglut -lGL -lGLU

# All objects including networking
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.hpp)

%.o: %.cpp $(HEADERS)
	$(CC) -c $(CXXFLAGS) $< -o $@

PRODUCT = Minesweeper

$(PRODUCT): $(OBJECTS) 
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@

.PHONY: clean all
all: $(PRODUCT)
	rm -f *.o

clean:
	rm -f *.o
	rm -f $(PRODUCT)