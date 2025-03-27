CC = clang++ $(CFLAGS) $(STD)
CFLAGS = -std=c++20

# INCLUDES = -I/usr/X11/include/GL
# LIBS = -L/usr/X11/lib -lglut -lGL -lGLU

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

PRODUCT = Minesweeper

$(PRODUCT): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.PHONY: clean all
all: $(PRODUCT)

clean:
	rm -f *.o
	rm -f $(PRODUCT)