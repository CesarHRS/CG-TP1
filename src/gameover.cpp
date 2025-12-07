#include <GL/glut.h>
#include "gameover.h"

// Variáveis globais
int windowWidth_gameover = 800;
int windowHeight_gameover = 700;
bool isGameOver = false;
bool isVictory = false; // Se é vitória ou derrota
RestartCallback onRestartCallback = nullptr;
MenuCallback onMenuCallback = nullptr;
NextPhaseCallback onNextPhaseCallback = nullptr; // Callback para próxima fase
int victoryPhase = 0;

void initGameOver(int windowWidth, int windowHeight) {
    windowWidth_gameover = windowWidth;
    windowHeight_gameover = windowHeight;
    isGameOver = false;
    isVictory = false;
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
    
    if (isVictory) {
        // Vitória — se for a fase 7 mostramos uma tela de jogo finalizado
        if (victoryPhase == 7) {
            glColor3f(0.0f, 1.0f, 0.0f); // Verde
            drawGameOverText("JOGO FINALIZADO COM SUCESSO!", windowWidth_gameover/2.0f - 200, windowHeight_gameover/2.0f + 120, GLUT_BITMAP_TIMES_ROMAN_24);
            glColor3f(0.9f, 0.9f, 0.2f);
            drawGameOverText("PARABENS!", windowWidth_gameover/2.0f - 80, windowHeight_gameover/2.0f + 80, GLUT_BITMAP_TIMES_ROMAN_24);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawGameOverText("Desenvolvedores:", windowWidth_gameover/2.0f - 60, windowHeight_gameover/2.0f + 30, GLUT_BITMAP_HELVETICA_18);
            drawGameOverText("Lara", windowWidth_gameover/2.0f - 20, windowHeight_gameover/2.0f + 5, GLUT_BITMAP_HELVETICA_18);
            drawGameOverText("Cesar", windowWidth_gameover/2.0f - 20, windowHeight_gameover/2.0f - 15, GLUT_BITMAP_HELVETICA_18);
            drawGameOverText("Pressione 'M' para voltar ao menu", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 60, GLUT_BITMAP_HELVETICA_18);
        } else {
            // Tela de Vitória padrão
            glColor3f(0.0f, 1.0f, 0.0f); // Verde
            drawGameOverText("FASE COMPLETA!", windowWidth_gameover/2.0f - 120, windowHeight_gameover/2.0f + 50, GLUT_BITMAP_TIMES_ROMAN_24);
            
            // Instruções em branco
            glColor3f(1.0f, 1.0f, 1.0f);
            drawGameOverText("Pressione 'N' para proxima fase", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 20, GLUT_BITMAP_HELVETICA_18);
            drawGameOverText("Pressione 'M' para voltar ao menu", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 50, GLUT_BITMAP_HELVETICA_18);
        }
    } else {
        // Tela de Game Over (derrota)
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
        drawGameOverText("GAME OVER", windowWidth_gameover/2.0f - 120, windowHeight_gameover/2.0f + 50, GLUT_BITMAP_TIMES_ROMAN_24);
        
        // Instruções em branco
        glColor3f(1.0f, 1.0f, 1.0f);
        drawGameOverText("Pressione 'R' para reiniciar a fase", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 20, GLUT_BITMAP_HELVETICA_18);
        drawGameOverText("Pressione 'M' para voltar ao menu", windowWidth_gameover/2.0f - 160, windowHeight_gameover/2.0f - 50, GLUT_BITMAP_HELVETICA_18);
    }
}

void handleGameOverKeyboard(unsigned char key) {
    if (!isGameOver) {
        return;
    }
    
    switch (key) {
        case 'n':
        case 'N':
            // Ir para próxima fase (somente se for vitória e não for fase 7)
            if (isVictory && victoryPhase != 7 && onNextPhaseCallback != nullptr) {
                // somente permitir ir para próxima fase quando não for a última "vitória final"
                isGameOver = false;
                isVictory = false;
                onNextPhaseCallback();
            }
            break;
        case 'r':
        case 'R':
            // Reiniciar a fase atual usando o callback registrado
            if (onRestartCallback != nullptr) {
                isGameOver = false;
                isVictory = false;
                onRestartCallback();
            }
            break;
        case 'm':
        case 'M':
            // Voltar ao menu usando o callback registrado
            if (onMenuCallback != nullptr) {
                isGameOver = false;
                isVictory = false;
                onMenuCallback();
            }
            break;
    }
}

void setGameOver(bool active) {
    isGameOver = active;
}

void setVictory(bool victory) {
    isVictory = victory;
}

void setVictoryPhase(int phase) {
    victoryPhase = phase;
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
 
void registerNextPhaseCallback(NextPhaseCallback callback) {
    onNextPhaseCallback = callback;
}
