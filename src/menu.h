#ifndef MENU_H
#define MENU_H


struct Button {
    float x, y, width, height;
    const char* label;
    bool isHovered;
};

enum GameState {
    MAIN_MENU,
    INSTRUCTIONS_SCREEN,
    GAME_SCREEN,
    PHASE2_SCREEN
};

extern GameState currentState;
extern int windowWidth;
extern int windowHeight;
extern Button startButton;
extern Button phase2Button;
extern Button instructionsButton;
extern Button exitButton;
extern Button backButton;

// Funções de desenho
void drawText(float x, float y, const char *text);
void drawButton(Button &button);
bool isMouseOverButton(int x, int y, Button &button);
void drawMainMenu();
void drawInstructionsScreen();

void renderScene();
void handleMouseClick(int button, int state, int x, int y);
void handleMouseHover(int x, int y);
void handleKeyboard(unsigned char key, int x, int y);
void handleKeyboardUp(unsigned char key, int x, int y);
void setup();
void updateScene();
void changeState(int newState);

#endif // MENU_H
