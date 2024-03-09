CXX = g++
CXXFLAGS = -std=c++11 -Wall
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
SRCS = main.cpp
TARGET = main.out
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(SFML_LIBS)
