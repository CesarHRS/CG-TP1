#ifndef PHASE3_H
#define PHASE3_H

#include <string>
#include <vector>

// Globals for phase 3
extern int windowWidthP3;
extern int windowHeightP3;
extern int mouseXP3;
extern int mouseYP3;

// Botão da calculadora
struct CalcButton {
    float x, y, w, h;
    std::string label;
    bool isHovered;
};

// Munição instável (Special Ammo)
struct SpecialAmmoP3 {
    int num1;
    int num2;
    char operation;
    int correctAnswer;
    std::string inputAnswer;
    int timer;           // Timer em frames (decrementa até 0)
    int maxTimer;        // Timer máximo (ex: 600 = 10 segundos)
    bool active;
    bool exploded;       // Se explodiu por timeout
    bool isHit;          // Se está mostrando efeito de erro
    int hitTimer;        // Timer do efeito de erro
};

extern SpecialAmmoP3 currentAmmoP3;
extern int correctAnswersCountP3;
extern int correctAnswersTargetP3;
extern int playerHealthP3;
extern int maxPlayerHealthP3;

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
