CC = g++
CFLAGS = -Wall -g
LDFLAGS = `sdl2-config --cflags --libs`
TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
