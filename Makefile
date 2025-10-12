CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lglut -lGLU -lGL
TARGET = jogo_educativo
SRCS = src/main.cpp src/menu.cpp src/game.cpp
OBJS = $(SRCS:.cpp=.o)

# Windows-specific variables
TARGET_WIN = jogo_educativo.exe
LDFLAGS_WIN = -lopengl32 -lglu32 -lfreeglut -lglew32

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build: all

run: $(TARGET)
	./$(TARGET)

# Windows build and run
build-windows: $(TARGET_WIN)

$(TARGET_WIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_WIN) $(SRCS) $(LDFLAGS_WIN)

run-windows: $(TARGET_WIN)
	.\$(TARGET_WIN)

clean:
	rm -f $(TARGET) $(OBJS)

clean-windows:
	rm -f $(TARGET_WIN) $(OBJS)

.PHONY: all build run clean

.PHONY: build-windows run-windows clean-windows

