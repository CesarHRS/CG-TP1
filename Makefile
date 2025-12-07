CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
# Use pkg-config to add SDL2 / SDL2_mixer flags when available
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_mixer 2>/dev/null)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer 2>/dev/null)

LDFLAGS = -lglut -lGLU -lGL $(SDL_LIBS)
CXXFLAGS += $(SDL_CFLAGS)
TARGET = jogo_educativo
SRCS = src/main.cpp src/menu.cpp src/game.cpp src/gameover.cpp src/phase2.cpp src/phase3.cpp src/phase4.cpp src/phase5.cpp src/phase6.cpp src/phase7.cpp src/audio.cpp
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
	del /Q $(TARGET_WIN) src\*.o 2>nul || echo Limpeza concluida

.PHONY: all build run clean

.PHONY: build-windows run-windows clean-windows
