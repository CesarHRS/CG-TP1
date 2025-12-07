#include <GL/glut.h>
#include "gameover.h"
#include <stdio.h>

// Variáveis globais
int windowWidth_gameover = 800;
int windowHeight_gameover = 700;
bool isGameOver = false;
bool isVictory = false; // Se é vitória ou derrota
bool gamePaused = false; // Se o jogo está pausado
int pausedPhase = 0; // Qual fase está pausada
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
    
    if (isVictory && victoryPhase == 7) {
        // Tela final da fase 7 - jogo completo
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

// Funções de pausa
void setPaused(bool paused, int phase) {
    gamePaused = paused;
    pausedPhase = phase;
}

bool getPaused() {
    return gamePaused;
}

void drawPauseScreen() {
    if (!gamePaused) return;
    
    // Fundo semi-transparente
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
    
    // Título
    glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
    drawGameOverText("JOGO PAUSADO", windowWidth_gameover/2.0f - 100, windowHeight_gameover/2.0f + 150, GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Instruções específicas da fase
    glColor3f(0.8f, 0.8f, 0.8f);
    char phaseTitle[50];
    sprintf(phaseTitle, "FASE %d - COMO JOGAR:", pausedPhase);
    drawGameOverText(phaseTitle, windowWidth_gameover/2.0f - 120, windowHeight_gameover/2.0f + 100, GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    if (pausedPhase == 1) {
        drawGameOverText("- Clique no asteroide para ativar a questao", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Digite o resultado da operacao matematica", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Destrua 10 asteroides para vencer", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 2) {
        drawGameOverText("- Mova o MOUSE para mirar", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- CLIQUE no asteroide correto da equacao", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Acerte 10 asteroides corretos para vencer", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 3) {
        drawGameOverText("- CLIQUE nos botoes da calculadora (direita)", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Resolva a equacao e pressione OK", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Acerte 10 vezes para vencer a fase", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 4) {
        drawGameOverText("- Use 'W', 'A', 'S', 'D' para mover", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- CLIQUE para disparar no boss", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Desvie dos projeteis e asteroides", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 5) {
        drawGameOverText("- Use 'W', 'A', 'S', 'D' para mover", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Use o MOUSE para olhar ao redor", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Pressione 'E' para pegar/soltar objetos", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 6) {
        drawGameOverText("- Use 'A' e 'D' para mover entre os campos", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Calcule: Area = PI * raio^2 (use PI = 3)", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Va para o campo com o valor correto", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    } else if (pausedPhase == 7) {
        drawGameOverText("- Use 'W', 'A', 'S', 'D' para mover", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 60, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- Use o MOUSE para mirar", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 40, GLUT_BITMAP_HELVETICA_12);
        drawGameOverText("- ESPACO ou CLIQUE para atirar no monstro", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f + 20, GLUT_BITMAP_HELVETICA_12);
    }
    
    // Opções
    glColor3f(0.2f, 1.0f, 0.2f); // Verde
    drawGameOverText("Pressione 'C' para continuar jogando", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f - 40, GLUT_BITMAP_HELVETICA_18);
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    drawGameOverText("Pressione 'M' para voltar ao menu", windowWidth_gameover/2.0f - 150, windowHeight_gameover/2.0f - 70, GLUT_BITMAP_HELVETICA_18);
}

void handlePauseKeyboard(unsigned char key) {
    if (!gamePaused) return;
    
    switch (key) {
        case 'c':
        case 'C':
            // Continuar jogando
            gamePaused = false;
            pausedPhase = 0;
            break;
        case 'm':
        case 'M':
            // Voltar ao menu
            if (onMenuCallback != nullptr) {
                gamePaused = false;
                pausedPhase = 0;
                onMenuCallback();
            }
            break;
    }
}

