CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --cflags --libs`
SRCS = main.cpp CubicBezier.cpp Spline.cpp
HEADERS = CubicBezier.h Spline.h
OBJS = $(SRCS:.cpp=.o)
TARGET = main 

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean