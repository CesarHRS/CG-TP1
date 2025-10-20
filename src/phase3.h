#ifndef PHASE3_H
#define PHASE3_H

#include <string>
#include <vector>

// Globals for phase 3
extern int windowWidthP3;
extern int windowHeightP3;
extern int mouseXP3;
extern int mouseYP3;

// Calculator state
struct CalculatorP3 {
    std::string display; // texto atual
    int selectedRow;
    int selectedCol;
    bool enterPressed;
};

extern CalculatorP3 calcP3;

struct EquationP3 {
    int x; // pode ser -1 se for variável
    int y; // pode ser -1 se for variável
    int result;
    char op; // '+','-','*','/'
    bool active;
    bool xIsVariable; // se x é a incógnita
};

extern EquationP3 currentEquationP3;

// Functions
void initPhase3();
void drawPhase3();
void updatePhase3();
void handlePhase3MouseMove(int x, int y);
void handlePhase3MouseClick(int button, int state, int x, int y);
void handlePhase3Keyboard(unsigned char key);
void handlePhase3KeyboardUp(unsigned char key);

// Callbacks
void restartPhase3();
void returnToMenuFromPhase3();

#endif // PHASE3_H
