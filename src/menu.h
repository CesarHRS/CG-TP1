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
    PHASE2_SCREEN,
    PHASE3_SCREEN,
    PHASE4_SCREEN,
    PHASE5_SCREEN
};

extern GameState currentState;
extern int windowWidth;
extern int windowHeight;
extern Button startButton;
extern Button phase2Button;
extern Button phase3Button;
extern Button phase4Button;
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
void handleSpecialKey(int key, int x, int y); // Teclas especiais (setas)
void handleSpecialKeyUp(int key, int x, int y); // Teclas especiais up
void setup();
void updateScene();
void changeState(int newState);

// Story overlay when clicking phases
extern bool showPhaseStory;
extern int storyPhase; // 1..5 -> which phase's story is pending
extern int storyPage; // pagination index (0-based, two paragraphs per page)
void drawStoryOverlay();
void showStoryForPhase(int phase); // Mostrar história antes de iniciar fase

// Phase5 hooks
void initPhase5();
void drawPhase5();
void updatePhase5();
void handlePhase5MouseClick(int button, int state, int x, int y);
void handlePhase5MouseMove(int x, int y);
void handlePhase5Keyboard(unsigned char key);
void handlePhase5KeyboardUp(unsigned char key);
void handlePhase5SpecialKey(int key, int x, int y);
void handlePhase5SpecialKeyUp(int key, int x, int y);

#endif // MENU_H
