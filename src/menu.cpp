#include <GL/glut.h>
#include "menu.h"
#include "game.h"
#include "phase2.h"
#include "phase3.h"
#include "gameover.h"
#include <string.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif




GameState currentState = MAIN_MENU;
int windowWidth = 800;
int windowHeight = 600;
// menu uses system cursor; custom pointer is drawn only in phase3
int menuMouseX = 400;
int menuMouseY = 300;

Button startButton = {300, 390, 200, 50, "Fase 1", false};
Button phase2Button = {300, 320, 200, 50, "Fase 2", false};
Button phase3Button = {300, 250, 200, 50, "Fase 3", false};
Button instructionsButton = {300, 190, 200, 50, "Como Jogar", false};
Button exitButton = {300, 130, 200, 50, "Sair", false};
Button backButton = {300, 80, 200, 50, "Voltar para o Menu", false};

void handleKeyboardUp(unsigned char key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        handleGameKeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        handlePhase2KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        handlePhase3KeyboardUp(key);
        glutPostRedisplay();
    }
}


// custom pointer removed from menu; phase3 draws its own pointer

void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawMenuPointer(int mx, int my) {
    float px = (float)mx;
    float py = (float)(windowHeight - my);
    float size = 10.0f;
    // glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.0f, 0.7f, 1.0f, 0.12f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(px, py);
    for (int i = 0; i <= 20; ++i) {
        float a = (float)i/20.0f * 2.0f * M_PI;
        glVertex2f(px + cos(a) * (size+6.0f), py + sin(a) * (size+6.0f));
    }
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(px, py);
    glVertex2f(px + size, py - size * 0.6f);
    glVertex2f(px + size * 0.2f, py + size * 0.2f);
    glEnd();
}

void drawButton(Button &button) {
    float radius = button.height / 2.0f;
    int segments = 20;
    if (button.isHovered) {
        glColor3f(0.7f, 0.7f, 0.9f);
    } else {
        glColor3f(0.5f, 0.5f, 0.8f);
    }
    glBegin(GL_POLYGON);
    glVertex2f(button.x + radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y + button.height);
    glVertex2f(button.x + radius, button.y + button.height);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = M_PI; // 180 graus
        theta += (M_PI * i) / segments;
        glVertex2f(button.x + radius + radius * cos(theta), button.y + radius + radius * sin(theta));
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = 0.0f;
        theta += (M_PI * i) / segments;
        glVertex2f(button.x + button.width - radius + radius * cos(theta), button.y + radius + radius * sin(theta));
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    float textX = button.x + (button.width - strlen(button.label) * 9) / 2.0f;
    float textY = button.y + (button.height / 2.0f) + 5;
    drawText(textX, textY, button.label);
}

bool isMouseOverButton(int x, int y, Button &button) {
    int invertedY = windowHeight - y;
    return (x >= button.x && x <= button.x + button.width &&
            invertedY >= button.y && invertedY <= button.y + button.height);
}


// --- Funções de Renderização das Telas ---

void drawMainMenu() {
    drawText(320, 500, "Jogo de navinha mais legal da sua vida");
    drawButton(startButton);
    drawButton(phase2Button);
    drawButton(phase3Button);
    drawButton(instructionsButton);
    drawButton(exitButton);
}

void drawInstructionsScreen() {
    drawText(330, 500, "Como Jogar");
    drawText(150, 440, "Instrucao 1: Pressione 'A' para mover a nave para a esquerda.");
    drawText(150, 420, "Instrucao 2: Pressione 'D' para mover a nave para a direita.");
    drawText(150, 400, "Instrucao 3: Na fase 1, clique nos asteroides com o botao esquerdo do mouse.");
    drawText(150, 380, "Instrucao 4: Digite o resultado da conta que aparecer no asteroide usando o teclado.");
    drawText(150, 360, "Instrucao 5: Se o asteroide te atinge, voce tomara dano. Se o dano for muito alto, voce morre.");
    drawText(150, 340, "Instrucao 6: Pressione 'ESC' no jogo para voltar ao menu.");
    drawButton(backButton);
}

// --- Funções Principais (Callbacks) ---

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (currentState) {
        case MAIN_MENU:
            drawMainMenu();
            drawMenuPointer(menuMouseX, menuMouseY);
            break;
        case INSTRUCTIONS_SCREEN:
            drawInstructionsScreen();
            break;
        case GAME_SCREEN:
            drawGame(); 
            break;
        case PHASE2_SCREEN:
            drawPhase2();
            break;
        case PHASE3_SCREEN:
            drawPhase3();
            break;
            break;
    }

    glutSwapBuffers();
}

void updateScene() {
    if (currentState == GAME_SCREEN) {
        updateGame();
        glutPostRedisplay(); 
    } else if (currentState == PHASE2_SCREEN) {
        updatePhase2();
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        updatePhase3();
        glutPostRedisplay();
    }
}


void handleMouseClick(int button, int state, int x, int y) {
    // Se estiver no jogo, passar clique para o handler do jogo
    if (currentState == GAME_SCREEN) {
        handleGameMouseClick(button, state, x, y);
        return;
    }
    
    // Se estiver na Fase 2, passar clique para o handler da Fase 2
    if (currentState == PHASE2_SCREEN) {
        handlePhase2MouseClick(button, state, x, y);
        return;
    }

    // Se estiver na Fase 3, passar clique para o handler da Fase 3
    if (currentState == PHASE3_SCREEN) {
        handlePhase3MouseClick(button, state, x, y);
        return;
    }
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (currentState) {
            case MAIN_MENU:
                if (isMouseOverButton(x, y, startButton)) {
                    currentState = GAME_SCREEN;
                    initGame();
                    // Registrar callback de movimento do mouse no jogo
                    glutPassiveMotionFunc(handleGameMouseMove);
                    glutMotionFunc(handleGameMouseMove);
                } else if (isMouseOverButton(x, y, phase2Button)) {
                    currentState = PHASE2_SCREEN;
                    initPhase2();
                    // Registrar callback de movimento do mouse na Fase 2
                    glutPassiveMotionFunc(handlePhase2MouseMove);
                    glutMotionFunc(handlePhase2MouseMove);
                } else if (isMouseOverButton(x, y, phase3Button)) {
                    currentState = PHASE3_SCREEN;
                    initPhase3();
                    // Registrar callback de movimento do mouse na Fase 3
                    glutPassiveMotionFunc(handlePhase3MouseMove);
                    glutMotionFunc(handlePhase3MouseMove);
                } else if (isMouseOverButton(x, y, instructionsButton)) {
                    currentState = INSTRUCTIONS_SCREEN;
                } else if (isMouseOverButton(x, y, exitButton)) {
                    exit(0); // Sair do jogo
                }
                break;
            case INSTRUCTIONS_SCREEN:
                if (isMouseOverButton(x, y, backButton)) {
                    currentState = MAIN_MENU;
                }
                break;
            case GAME_SCREEN:
                break;
        }
        glutPostRedisplay();
    }
}

void handleMouseHover(int x, int y) {
    // update menu pointer
    menuMouseX = x;
    menuMouseY = y;
    bool needsRedraw = false;
    
    switch (currentState) {
        case MAIN_MENU:
            if (startButton.isHovered != isMouseOverButton(x, y, startButton)) {
                startButton.isHovered = !startButton.isHovered;
                needsRedraw = true;
            }
            if (phase2Button.isHovered != isMouseOverButton(x, y, phase2Button)) {
                phase2Button.isHovered = !phase2Button.isHovered;
                needsRedraw = true;
            }
            if (phase3Button.isHovered != isMouseOverButton(x, y, phase3Button)) {
                phase3Button.isHovered = !phase3Button.isHovered;
                needsRedraw = true;
            }
            if (instructionsButton.isHovered != isMouseOverButton(x, y, instructionsButton)) {
                instructionsButton.isHovered = !instructionsButton.isHovered;
                needsRedraw = true;
            }
            if (exitButton.isHovered != isMouseOverButton(x, y, exitButton)) {
                exitButton.isHovered = !exitButton.isHovered;
                needsRedraw = true;
            }
            break;
        case INSTRUCTIONS_SCREEN:
            if (backButton.isHovered != isMouseOverButton(x, y, backButton)) {
                backButton.isHovered = !backButton.isHovered;
                needsRedraw = true;
            }
            break;
        case GAME_SCREEN:
            break;
    }

    if (needsRedraw) {
        glutPostRedisplay();
    }
}



void handleKeyboard(unsigned char key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        if (key == 27) { 
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            // Se está em Game Over, usar handleGameOverKeyboard
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handleGameKeyboard(key);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            // Se está em Game Over, usar handleGameOverKeyboard
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase2Keyboard(key);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase3Keyboard(key);
            }
        }
        glutPostRedisplay();
    }
}


void setup() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    // hide system cursor so custom pointer is visible
    glutSetCursor(GLUT_CURSOR_NONE);
}

void changeState(int newState) {
    currentState = (GameState)newState;
    
    // Se voltando ao menu, restaurar cursor e callbacks
    if (newState == MAIN_MENU) {
        glutSetCursor(GLUT_CURSOR_INHERIT);
        glutPassiveMotionFunc(handleMouseHover);
    }
    
    glutPostRedisplay();
}
