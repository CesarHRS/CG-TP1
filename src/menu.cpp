#include <GL/glut.h>
#include "menu.h"
#include "game.h"
#include "gameover.h"
#include <string.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif




GameState currentState = MAIN_MENU;
int windowWidth = 800;
int windowHeight = 600;

Button startButton = {300, 350, 200, 50, "Iniciar", false};
Button instructionsButton = {300, 280, 200, 50, "Como Jogar", false};
Button exitButton = {300, 210, 200, 50, "Sair", false};
Button backButton = {300, 150, 200, 50, "Voltar para o Menu", false};

void handleKeyboardUp(unsigned char key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        handleGameKeyboardUp(key);
        glutPostRedisplay();
    }
}


void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
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
    drawButton(instructionsButton);
    drawButton(exitButton);
}

void drawInstructionsScreen() {
    drawText(330, 500, "Como Jogar");
    drawText(150, 420, "Instrucao 1: Pressione 'A' para mover a nave para a esquerda.");
    drawText(150, 390, "Instrucao 2: Pressione 'D' para mover a nave para a direita.");
    drawText(150, 360, "Instrucao 3: Colete os itens de lixo espacial!");
    drawText(150, 330, "Instrucao 4: Pressione 'ESC' no jogo para voltar ao menu.");
    
    drawButton(backButton);
}

// --- Funções Principais (Callbacks) ---

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (currentState) {
        case MAIN_MENU:
            drawMainMenu();
            break;
        case INSTRUCTIONS_SCREEN:
            drawInstructionsScreen();
            break;
        case GAME_SCREEN:
            drawGame(); 
            break;
    }

    glutSwapBuffers();
}

void updateScene() {
    if (currentState == GAME_SCREEN) {
        updateGame();
        glutPostRedisplay(); 
    }
}


void handleMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (currentState) {
            case MAIN_MENU:
                if (isMouseOverButton(x, y, startButton)) {
                    currentState = GAME_SCREEN;
                    initGame(); 
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
    bool needsRedraw = false;
    
    switch (currentState) {
        case MAIN_MENU:
            if (startButton.isHovered != isMouseOverButton(x, y, startButton)) {
                startButton.isHovered = !startButton.isHovered;
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
        } else {
            // Se está em Game Over, usar handleGameOverKeyboard
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handleGameKeyboard(key);
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
}

void changeState(int newState) {
    currentState = (GameState)newState;
    glutPostRedisplay();
}
