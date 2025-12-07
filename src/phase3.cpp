#include <GL/glut.h>
#include "phase3.h"
#include "game.h"
#include "menu.h"
#include "gameover.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include "audio.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int windowWidthP3 = 800;
int windowHeightP3 = 700;
int mouseXP3 = 400;
int mouseYP3 = 300;

int correctAnswersCountP3 = 0;
int correctAnswersTargetP3 = 10;
int playerHealthP3 = 100;
int maxPlayerHealthP3 = 100;

SpecialAmmoP3 currentAmmoP3 = {0, 0, '+', 0, "", 0, 1200, false, false, false, 0};  // 1200 frames = 20 segundos

// Botões da calculadora
std::vector<CalcButton> calcButtons;

void createNewAmmoP3();

void initCalcButtons() {
    calcButtons.clear();
    float calcCenterX = 550.0f;  // Centro da calculadora à direita
    float startY = 240.0f;  // Um pouco mais acima
    float btnW = 50.0f;
    float btnH = 40.0f;
    
    // Linha 1: 7 8 9
    calcButtons.push_back({calcCenterX - 90.0f, startY, btnW, btnH, "7", false});
    calcButtons.push_back({calcCenterX - 30.0f, startY, btnW, btnH, "8", false});
    calcButtons.push_back({calcCenterX + 30.0f, startY, btnW, btnH, "9", false});
    
    // Linha 2: 4 5 6
    calcButtons.push_back({calcCenterX - 90.0f, startY - 50.0f, btnW, btnH, "4", false});
    calcButtons.push_back({calcCenterX - 30.0f, startY - 50.0f, btnW, btnH, "5", false});
    calcButtons.push_back({calcCenterX + 30.0f, startY - 50.0f, btnW, btnH, "6", false});
    
    // Linha 3: 1 2 3
    calcButtons.push_back({calcCenterX - 90.0f, startY - 100.0f, btnW, btnH, "1", false});
    calcButtons.push_back({calcCenterX - 30.0f, startY - 100.0f, btnW, btnH, "2", false});
    calcButtons.push_back({calcCenterX + 30.0f, startY - 100.0f, btnW, btnH, "3", false});
    
    // Linha 4: 0 DEL ENTER
    calcButtons.push_back({calcCenterX - 90.0f, startY - 150.0f, btnW, btnH, "0", false});
    calcButtons.push_back({calcCenterX - 30.0f, startY - 150.0f, btnW, btnH, "DEL", false});
    calcButtons.push_back({calcCenterX + 30.0f, startY - 150.0f, btnW, btnH, "OK", false});
}

void drawText(const std::string &s, float x, float y, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : s) glutBitmapCharacter(font, c);
}

void createNewAmmoP3() {
    int opIdx = rand() % 4;
    char op = '+';
    if (opIdx == 0) op = '+';
    else if (opIdx == 1) op = '-';
    else if (opIdx == 2) op = '*';
    else op = '/';

    int x = 0;  // valor de X (resposta correta)
    int num = 0;  // o outro número na equação
    int result = 0;  // resultado da equação

    if (op == '+') {
        // X + num = result
        x = rand() % 50 + 1;
        num = rand() % 50 + 1;
        result = x + num;
    } else if (op == '-') {
        // X - num = result
        x = rand() % 50 + 20;
        num = rand() % 20 + 1;
        result = x - num;
    } else if (op == '*') {
        // X * num = result
        x = rand() % 12 + 2;
        num = rand() % 12 + 2;
        result = x * num;
    } else {
        // X / num = result (divisão exata)
        num = rand() % 12 + 2;
        result = rand() % 12 + 2;
        x = num * result;
    }

    currentAmmoP3.num1 = num;  // o número conhecido
    currentAmmoP3.num2 = result;  // o resultado
    currentAmmoP3.operation = op;
    currentAmmoP3.correctAnswer = x;  // X é a resposta
    currentAmmoP3.inputAnswer = "";
    currentAmmoP3.timer = currentAmmoP3.maxTimer;
    currentAmmoP3.active = true;
    currentAmmoP3.exploded = false;
    currentAmmoP3.isHit = false;
    currentAmmoP3.hitTimer = 0;
}

void initPhase3() {
    srand(time(0));
    windowWidthP3 = 800;
    windowHeightP3 = 700;
    mouseXP3 = windowWidthP3/2;
    mouseYP3 = windowHeightP3/2;
    
    correctAnswersCountP3 = 0;
    playerHealthP3 = maxPlayerHealthP3;
    
    // Iniciar contagem regressiva
    showCountdown = true;
    countdownTimer = 0;
    countdownValue = 3;
    
    initCalcButtons();
    createNewAmmoP3();
    setGameOver(false);

    initGameOver(windowWidthP3, windowHeightP3);
    registerRestartCallback(restartPhase3);
    registerMenuCallback(returnToMenuFromPhase3);

    glutSetCursor(GLUT_CURSOR_INHERIT);  // Cursor padrão do Windows
}

void drawPanelButton(float x, float y, float w, float h, float r, float g, float b, bool lit) {
    glColor3f(r * 0.3f, g * 0.3f, b * 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
    
    if (lit) {
        glColor3f(r, g, b);
    } else {
        glColor3f(r * 0.5f, g * 0.5f, b * 0.5f);
    }
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 2);
    glVertex2f(x + w - 2, y + 2);
    glVertex2f(x + w - 2, y + h - 2);
    glVertex2f(x + 2, y + h - 2);
    glEnd();
    
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
    glLineWidth(1.0f);
}

void drawShipPanel() {
    float shakeX = 0.0f;
    float shakeY = 0.0f;
    
    // Tremor quando a bomba explode
    if (!getGameOver() && currentAmmoP3.exploded && currentAmmoP3.hitTimer > 0) {
        float intensity = (float)currentAmmoP3.hitTimer / 30.0f * 20.0f;  // Intensidade maior
        shakeX = (rand() % 200 - 100) / 100.0f * intensity;
        shakeY = (rand() % 200 - 100) / 100.0f * intensity;
    }
    
    glPushMatrix();
    glTranslatef(shakeX, shakeY, 0.0f);
    
    // Cor do painel (vermelho quando explode)
    if (!getGameOver() && currentAmmoP3.exploded && currentAmmoP3.hitTimer > 0) {
        glColor3f(0.7f, 0.0f, 0.0f);  // Vermelho mais forte
    } else {
        glColor3f(0.12f, 0.15f, 0.18f);  // Normal
    }
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidthP3, 0);
    glVertex2f(windowWidthP3, windowHeightP3);
    glVertex2f(0, windowHeightP3);
    glEnd();
    
    glColor3f(0.08f, 0.10f, 0.12f);
    glLineWidth(2.0f);
    for (int i = 0; i < 10; i++) {
        float x = i * 80.0f;
        glBegin(GL_LINES);
        glVertex2f(x, 0);
        glVertex2f(x + 30, windowHeightP3);
        glEnd();
    }
    glLineWidth(1.0f);
    
    // Botões do painel - piscar muito mais sutil (parar no game over)
    int currentTime = getGameOver() ? 0 : glutGet(GLUT_ELAPSED_TIME);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
            float bx = 30.0f + i * 90.0f;
            float by = 400.0f + j * 35.0f;
            // Pisca apenas se o tempo for múltiplo de 3000ms E chance de 5% (não no game over)
            bool lit = !getGameOver() && ((currentTime / 3000) % 2 == 0) && ((i + j) % 20 == 0);
            drawPanelButton(bx, by, 60.0f, 25.0f, 0.0f, lit ? 0.6f : 0.3f, lit ? 0.8f : 0.5f, lit);
        }
    }
    
    // Botões principais - piscar lento e sequencial (não no game over)
    for (int i = 0; i < 4; i++) {
        float bx = 50.0f + i * 180.0f;
        float by = 20.0f;
        bool lit = !getGameOver() && (i == (int)(currentTime / 2000) % 4);
        drawPanelButton(bx, by, 140.0f, 40.0f, 1.0f, lit ? 0.4f : 0.2f, 0.0f, lit);
    }
    
    glColor3f(0.2f, 0.25f, 0.28f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, 380);
    glVertex2f(windowWidthP3 - 20, 380);
    glVertex2f(windowWidthP3 - 20, windowHeightP3 - 20);
    glVertex2f(20, windowHeightP3 - 20);
    glEnd();
    glLineWidth(1.0f);
    
    glPopMatrix();
}

void drawAmmoDisplay() {
    if (!currentAmmoP3.active) return;
    
    // Bomba à esquerda, abaixo dos botões do painel
    float bombX = 200.0f;
    float bombY = 220.0f;  // Um pouco mais abaixo
    
    // Offset de tremida se errou
    float shakeX = 0.0f;
    float shakeY = 0.0f;
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        float shakeIntensity = (float)currentAmmoP3.hitTimer / 30.0f * 5.0f;
        shakeX = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
        shakeY = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
    }
    
    glPushMatrix();
    glTranslatef(bombX + shakeX, bombY + shakeY, 0.0f);
    
    float timePercent = (float)currentAmmoP3.timer / (float)currentAmmoP3.maxTimer;
    
    // Desenhar bomba
    float bombRadius = 80.0f;
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= 36; i++) {
        float angle = (float)i / 36.0f * 2.0f * M_PI;
        glVertex2f(cos(angle) * bombRadius, sin(angle) * bombRadius);
    }
    glEnd();
    
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 36; i++) {
        float angle = (float)i / 36.0f * 2.0f * M_PI;
        glVertex2f(cos(angle) * bombRadius, sin(angle) * bombRadius);
    }
    glEnd();
    glLineWidth(1.0f);
    
    // Pavio da bomba
    float fuseX = 0.0f;
    float fuseY = bombRadius;
    glColor3f(0.4f, 0.3f, 0.2f);
    glLineWidth(6.0f);
    glBegin(GL_LINES);
    glVertex2f(fuseX, fuseY);
    glVertex2f(fuseX, fuseY + 30.0f);
    glEnd();
    glLineWidth(1.0f);
    
    // Faísca no pavio (quando tempo crítico) - parar no game over
    if (!getGameOver() && timePercent < 0.5f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float sparkSize = 8.0f + (rand() % 100) / 100.0f * 4.0f;
        glColor4f(1.0f, 0.6f, 0.0f, 0.9f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(fuseX, fuseY + 30.0f);
        for (int i = 0; i <= 8; i++) {
            float angle = (float)i / 8.0f * 2.0f * M_PI;
            glVertex2f(fuseX + cos(angle) * sparkSize, fuseY + 30.0f + sin(angle) * sparkSize);
        }
        glEnd();
        
        glColor4f(1.0f, 1.0f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(fuseX, fuseY + 30.0f);
        for (int i = 0; i <= 8; i++) {
            float angle = (float)i / 8.0f * 2.0f * M_PI;
            glVertex2f(fuseX + cos(angle) * sparkSize * 0.5f, fuseY + 30.0f + sin(angle) * sparkSize * 0.5f);
        }
        glEnd();
        
        glDisable(GL_BLEND);
    }
    
    // Arco de tempo na bomba
    float r = 1.0f - timePercent;
    float g = timePercent;
    if (timePercent < 0.2f) {
        r = 1.0f;
        g = 0.0f;
    }
    
    glColor3f(r, g, 0.0f);
    float timerArcRadius = bombRadius - 15.0f;
    glLineWidth(8.0f);
    glBegin(GL_LINE_STRIP);
    int segments = (int)(36 * timePercent);
    for (int i = 0; i <= segments; i++) {
        float angle = -M_PI / 2.0f + (float)i / 36.0f * 2.0f * M_PI;
        glVertex2f(cos(angle) * timerArcRadius, sin(angle) * timerArcRadius);
    }
    glEnd();
    glLineWidth(1.0f);
    
    // Equação dentro da bomba (antes do glPopMatrix)
    std::ostringstream equation;
    equation << "X " << currentAmmoP3.operation << " " 
             << currentAmmoP3.num1 << " = " << currentAmmoP3.num2;
    glColor3f(1.0f, 1.0f, 0.0f);
    std::string eqStr = equation.str();
    float eqWidth = eqStr.length() * 9.0f;
    drawText(eqStr, -eqWidth/2.0f, 5.0f, GLUT_BITMAP_HELVETICA_18);
    
    glPopMatrix();
}

void drawCalcButtons() {
    // Tremor da calculadora quando errar
    float shakeX = 0.0f;
    float shakeY = 0.0f;
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        float shakeIntensity = (float)currentAmmoP3.hitTimer / 30.0f * 8.0f;
        shakeX = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
        shakeY = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
    }
    
    glPushMatrix();
    glTranslatef(shakeX, shakeY, 0.0f);
    
    // Display field (visor da calculadora) - à direita, abaixo dos botões do painel
    float calcCenterX = 550.0f;  // Centro da calculadora à direita
    float displayW = 200.0f;
    float displayH = 40.0f;
    float displayX = calcCenterX - displayW/2.0f;
    float displayY = 300.0f;  // Um pouco mais acima
    
    // Cor de fundo do display (vermelho se errou)
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        glColor3f(0.3f, 0.0f, 0.0f);  // Vermelho escuro
    } else {
        glColor3f(0.05f, 0.08f, 0.05f);  // Verde escuro normal
    }
    glBegin(GL_QUADS);
    glVertex2f(displayX, displayY);
    glVertex2f(displayX + displayW, displayY);
    glVertex2f(displayX + displayW, displayY + displayH);
    glVertex2f(displayX, displayY + displayH);
    glEnd();
    
    // Borda do display (vermelha se errou)
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        glColor3f(1.0f, 0.0f, 0.0f);  // Vermelho
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);  // Verde
    }
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(displayX, displayY);
    glVertex2f(displayX + displayW, displayY);
    glVertex2f(displayX + displayW, displayY + displayH);
    glVertex2f(displayX, displayY + displayH);
    glEnd();
    glLineWidth(1.0f);
    
    // Cor do texto (vermelho se errou, verde se normal)
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        glColor3f(1.0f, 0.3f, 0.3f);  // Vermelho claro
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);  // Verde
    }
    std::string display = currentAmmoP3.inputAnswer;
    // Cursor piscante
    if (!getGameOver() && (glutGet(GLUT_ELAPSED_TIME) / 500) % 2 == 0) {
        display += "|";
    }
    // Centralizar texto no display
    float textWidth = display.length() * 9.0f;
    drawText(display, displayX + displayW/2.0f - textWidth/2.0f, displayY + 25.0f);
    
    // Botões da calculadora
    
    for (const auto& btn : calcButtons) {
        // Cor de fundo do botão (vermelho se errou)
        if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
            glColor3f(0.3f, 0.0f, 0.0f);  // Vermelho escuro
        } else if (btn.isHovered && !getGameOver()) {
            glColor3f(0.2f, 0.3f, 0.4f);
        } else {
            glColor3f(0.1f, 0.15f, 0.2f);
        }
        
        glBegin(GL_QUADS);
        glVertex2f(btn.x, btn.y);
        glVertex2f(btn.x + btn.w, btn.y);
        glVertex2f(btn.x + btn.w, btn.y + btn.h);
        glVertex2f(btn.x, btn.y + btn.h);
        glEnd();
        
        // Borda do botão (vermelha se errou)
        if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
            glColor3f(1.0f, 0.0f, 0.0f);  // Vermelho
        } else if (btn.label == "OK") {
            glColor3f(0.0f, 1.0f, 0.0f);
        } else if (btn.label == "DEL") {
            glColor3f(1.0f, 0.5f, 0.0f);
        } else {
            glColor3f(0.0f, 0.8f, 1.0f);
        }
        
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(btn.x, btn.y);
        glVertex2f(btn.x + btn.w, btn.y);
        glVertex2f(btn.x + btn.w, btn.y + btn.h);
        glVertex2f(btn.x, btn.y + btn.h);
        glEnd();
        glLineWidth(1.0f);
        
        // Texto do botão (branco sempre)
        glColor3f(1.0f, 1.0f, 1.0f);
        float labelWidth = btn.label.length() * 9.0f;
        drawText(btn.label, btn.x + btn.w/2.0f - labelWidth/2.0f, btn.y + btn.h/2.0f + 5.0f, GLUT_BITMAP_HELVETICA_18);
    }
    
    glPopMatrix();
}

void drawHealthBarP3() {
    float barX = 20.0f;
    float barY = windowHeightP3 - 35.0f;
    float barW = 150.0f;
    float barH = 25.0f;
    
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barW, barY);
    glVertex2f(barX + barW, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();
    
    float healthPercent = (float)playerHealthP3 / (float)maxPlayerHealthP3;
    float fillW = barW * healthPercent;
    
    if (healthPercent > 0.5f) {
        glColor3f(0.0f, 1.0f, 0.0f);
    } else if (healthPercent > 0.25f) {
        glColor3f(1.0f, 1.0f, 0.0f);
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + fillW, barY);
    glVertex2f(barX + fillW, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barW, barY);
    glVertex2f(barX + barW, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();
    glLineWidth(1.0f);
    
    std::ostringstream oss;
    oss << "VIDA: " << playerHealthP3 << "/" << maxPlayerHealthP3;
    glColor3f(1.0f, 1.0f, 1.0f);  // Texto branco
    drawText(oss.str(), barX + 10.0f, barY + 8.0f, GLUT_BITMAP_HELVETICA_12);
}

void drawProgressCounter() {
    float barW = 250.0f;
    float barH = 25.0f;
    float x = windowWidthP3 - barW - 20.0f;
    float y = windowHeightP3 - 35.0f;
    
    // Fundo do retângulo
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + barW, y);
    glVertex2f(x + barW, y + barH);
    glVertex2f(x, y + barH);
    glEnd();
    
    // Borda branca
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + barW, y);
    glVertex2f(x + barW, y + barH);
    glVertex2f(x, y + barH);
    glEnd();
    glLineWidth(1.0f);
    
    // Texto branco
    glColor3f(1.0f, 1.0f, 1.0f);
    std::ostringstream oss;
    oss << "MUNICOES COLETADAS: " << correctAnswersCountP3 << "/" << correctAnswersTargetP3;
    drawText(oss.str(), x + 10.0f, y + 8.0f, GLUT_BITMAP_HELVETICA_12);
}

void drawCrosshairP3() {
    float px = (float)mouseXP3;
    float py = (float)(windowHeightP3 - mouseYP3);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 5; i >= 1; --i) {
        float a = 0.06f * i;
        glColor4f(0.0f, 0.7f, 1.0f, a);
        float rad = (float)(i * 3 + 6);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(px, py);
        for (int s = 0; s <= 20; ++s) {
            float aang = (float)s / 20.0f * 2.0f * M_PI;
            glVertex2f(px + cos(aang) * rad, py + sin(aang) * rad);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
    
    float size = 15.0f;
    glColor3f(0.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(px - size, py);
    glVertex2f(px + size, py);
    glVertex2f(px, py - size);
    glVertex2f(px, py + size);
    glEnd();
    glLineWidth(1.0f);
    
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; i++) {
        float angle = (float)i / 20.0f * 2.0f * M_PI;
        glVertex2f(px + cos(angle) * 5.0f, py + sin(angle) * 5.0f);
    }
    glEnd();
}

void drawPhase3() {
    drawShipPanel();
    drawAmmoDisplay();
    drawCalcButtons();
    drawHealthBarP3();
    drawProgressCounter();
    drawCountdown(); // Contagem regressiva
    
    if (getPaused()) {
        drawPauseScreen();
        return;
    }
    
    drawGameOver();
}

void updatePhase3() {
    if (getGameOver() || getPaused()) return;
    
    // Atualizar contagem regressiva
    if (showCountdown) {
        countdownTimer++;
        if (countdownTimer >= 60) {
            countdownTimer = 0;
            countdownValue--;
            if (countdownValue <= 0) {
                showCountdown = false;
            }
        }
        return; // Não atualizar o jogo enquanto está contando
    }
    
    if (currentAmmoP3.active && !currentAmmoP3.exploded) {
        currentAmmoP3.timer--;
        
        if (currentAmmoP3.timer <= 0) {
            currentAmmoP3.exploded = true;
            currentAmmoP3.hitTimer = 30;
            currentAmmoP3.active = false;  // Desativa a bomba atual
            playerHealthP3 -= 20;
            
            if (playerHealthP3 <= 0) {
                playerHealthP3 = 0;
                setGameOver(true);
                setVictory(false);
            }
            // tocar som de explosão e dano
            Audio::getInstance().play(Audio::SOUND_EXPLOSION);
            Audio::getInstance().play(Audio::SOUND_DAMAGE);
        }
        
        glutPostRedisplay();
    }
    
    if (currentAmmoP3.exploded && currentAmmoP3.hitTimer > 0) {
        currentAmmoP3.hitTimer--;
        if (currentAmmoP3.hitTimer <= 0) {
            currentAmmoP3.exploded = false;
            // Só cria nova munição se não está em game over
            if (!getGameOver()) {
                createNewAmmoP3();
            }
        }
        glutPostRedisplay();
    }
    
    if (currentAmmoP3.isHit && currentAmmoP3.hitTimer > 0) {
        currentAmmoP3.hitTimer--;
        if (currentAmmoP3.hitTimer <= 0) {
            currentAmmoP3.isHit = false;
        }
        glutPostRedisplay();
    }
}

void handlePhase3MouseMove(int x, int y) {
    mouseXP3 = x;
    mouseYP3 = y;
    
    // Atualizar hover dos botões
    int invertedY = windowHeightP3 - y;
    for (auto& btn : calcButtons) {
        btn.isHovered = (x >= btn.x && x <= btn.x + btn.w &&
                        invertedY >= btn.y && invertedY <= btn.y + btn.h);
    }
    
    glutPostRedisplay();
}

void handlePhase3MouseClick(int button, int state, int x, int y) {
    if (getGameOver() || button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
        return;
    }
    
    // Verificar clique nos botões
    int invertedY = windowHeightP3 - y;
    for (const auto& btn : calcButtons) {
        if (x >= btn.x && x <= btn.x + btn.w &&
            invertedY >= btn.y && invertedY <= btn.y + btn.h) {
            
            if (btn.label == "OK") {
                // Processar resposta
                if (!currentAmmoP3.inputAnswer.empty()) {
                    int answer = atoi(currentAmmoP3.inputAnswer.c_str());
                    
                    if (answer == currentAmmoP3.correctAnswer) {
                        correctAnswersCountP3++;
                        
                        // tocar som de tiro/sucesso
                        Audio::getInstance().play(Audio::SOUND_LASER);
                        Audio::getInstance().play(Audio::SOUND_VICTORY);

                        if (correctAnswersCountP3 >= correctAnswersTargetP3) {
                            // Completou a Fase 3! Mostrar história da Fase 4
                            showStoryForPhase(4);
                        } else {
                            createNewAmmoP3();
                        }
                    } else {
                        currentAmmoP3.isHit = true;
                        currentAmmoP3.hitTimer = 30;
                        currentAmmoP3.inputAnswer = "";
                        // tocar som de erro
                        Audio::getInstance().play(Audio::SOUND_ERROR);
                    }
                }
            } else if (btn.label == "DEL") {
                // Apagar último caractere
                if (!currentAmmoP3.inputAnswer.empty()) {
                    currentAmmoP3.inputAnswer.pop_back();
                }
            } else {
                // Adicionar dígito
                if (currentAmmoP3.inputAnswer.length() < 6) {
                    currentAmmoP3.inputAnswer += btn.label;
                }
            }
            
            glutPostRedisplay();
            break;
        }
    }
}

void handlePhase3Keyboard(unsigned char key) {
    if (getGameOver()) {
        handleGameOverKeyboard(key);
        return;
    }
    
    // ESC para pausar
    if (key == 27) {
        if (!getPaused()) {
            setPaused(true, 3);
        } else {
            handlePauseKeyboard(key);
        }
        glutPostRedisplay();
        return;
    }
    
    // Se está pausado, tratar teclas de pausa
    if (getPaused()) {
        handlePauseKeyboard(key);
        glutPostRedisplay();
        return;
    }
}

void handlePhase3KeyboardUp(unsigned char key) {
    (void)key;
}

void restartPhase3() {
    initPhase3();
}

void returnToMenuFromPhase3() {
    setGameOver(false);
    setVictory(false);
    setPaused(false, 0);
    glutSetCursor(GLUT_CURSOR_INHERIT);
    glutPassiveMotionFunc(handleMouseHover);
    currentState = MAIN_MENU;
    glutPostRedisplay();
}
