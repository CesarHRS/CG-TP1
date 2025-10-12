#include <GL/glut.h>
#include "gameover.h"

// Variáveis globais
int windowWidth_gameover = 800;
int windowHeight_gameover = 600;
bool isGameOver = false;
RestartCallback onRestartCallback = nullptr;
MenuCallback onMenuCallback = nullptr;

void initGameOver(int windowWidth, int windowHeight) {
    windowWidth_gameover = windowWidth;
    windowHeight_gameover = windowHeight;
    isGameOver = false;
}

// Função auxiliar para desenhar texto
void drawGameOverText(const char* text, float x, float y, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        ++text;
    }
}

// Tela de Game Over
void drawGameOver() {
    if (!isGameOver) {
        return;
    }
    
    // Fundo semi-transparente escuro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth_gameover, 0);
    glVertex2f(windowWidth_gameover, windowHeight_gameover);
    glVertex2f(0, windowHeight_gameover);
    glEnd();
    glDisable(GL_BLEND);
    
    // Texto "GAME OVER" em vermelho grande
    glColor3f(1.0f, 0.0f, 0.0f);
    drawGameOverText("GAME OVER", windowWidth_gameover/2.0f - 120, windowHeight_gameover/2.0f + 50, GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Instruções em branco
    glColor3f(1.0f, 1.0f, 1.0f);
    drawGameOverText("Pressione 'R' para reiniciar a fase", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 20, GLUT_BITMAP_HELVETICA_18);
    drawGameOverText("Pressione 'M' para voltar ao menu", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f - 60, GLUT_BITMAP_HELVETICA_18);
}

void handleGameOverKeyboard(unsigned char key) {
    if (!isGameOver) {
        return;
    }
    
    switch (key) {
        case 'r':
        case 'R':
            // Reiniciar a fase atual usando o callback registrado
            if (onRestartCallback != nullptr) {
                isGameOver = false;
                onRestartCallback();
            }
            break;
        case 'm':
        case 'M':
            // Voltar ao menu usando o callback registrado
            if (onMenuCallback != nullptr) {
                isGameOver = false;
                onMenuCallback();
            }
            break;
    }
}

void setGameOver(bool active) {
    isGameOver = active;
}

bool getGameOver() {
    return isGameOver;
}

void registerRestartCallback(RestartCallback callback) {
    onRestartCallback = callback;
}

void registerMenuCallback(MenuCallback callback) {
    onMenuCallback = callback;
}
