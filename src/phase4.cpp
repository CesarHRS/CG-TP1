#include <GL/glut.h>
#include "phase4.h"
#include "game.h"
#include "menu.h"
#include "gameover.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include "audio.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Globals
PlayerPhase4 playerP4;
BossPhase4 bossP4;
std::vector<ProjectileP4> bossProjectilesP4;
std::vector<AsteroidP4> asteroidsP4;
std::vector<LaserShotP4> laserShotsP4;
std::vector<ParallaxLayer> parallaxLayersP4;

int windowWidthP4 = 800;
int windowHeightP4 = 700;
int mouseXP4 = 400;
int mouseYP4 = 300;

int shotsRemainingP4 = 10;
int hitsNeededP4 = 6;
bool bossDefeatedP4 = false;

// Contagem regressiva
bool showCountdownP4 = false;
int countdownTimerP4 = 0;
int countdownValueP4 = 3;

// Internal timers and parameters
static int gameFrameCounterP4 = 0;
static int asteroidSpawnCooldownP4 = 0;
static int bossProjectileCooldownP4 = 0;

// Movement state for smooth continuous movement
static bool isMovingUpP4 = false;
static bool isMovingDownP4 = false;
static bool isMovingLeftP4 = false;
static bool isMovingRightP4 = false;

// Helper functions
static float randf(float a, float b) { return a + (b - a) * (rand() / (float)RAND_MAX); }

// Simple AABB collision
static bool rectCircleCollision(float rx, float ry, float rw, float rh, float cx, float cy, float cr) {
    float nearestX = std::max(rx, std::min(cx, rx + rw));
    float nearestY = std::max(ry, std::min(cy, ry + rh));
    float dx = cx - nearestX;
    float dy = cy - nearestY;
    return (dx*dx + dy*dy) <= (cr*cr);
}

void initParallax() {
    parallaxLayersP4.clear();
    // 3 camadas: mais distante (estrelas pequenas), média (estrelas maiores), planetas (visuais)
    ParallaxLayer l1 = {0.2f, 0.0f, 0.0f};
    ParallaxLayer l2 = {0.5f, 0.0f, 0.0f};
    ParallaxLayer l3 = {0.9f, 0.0f, 0.0f};
    parallaxLayersP4.push_back(l1);
    parallaxLayersP4.push_back(l2);
    parallaxLayersP4.push_back(l3);
}

void resetPhase4State() {
    shotsRemainingP4 = 15; // aumentado de 10 para 15
    bossDefeatedP4 = false;
    gameFrameCounterP4 = 0;
    asteroidSpawnCooldownP4 = 0;
    bossProjectileCooldownP4 = 120; // 2 segundos (assumindo 60FPS) - mais devagar
    bossProjectilesP4.clear();
    asteroidsP4.clear();
    laserShotsP4.clear();
    
    // Reset movement state
    isMovingUpP4 = false;
    isMovingDownP4 = false;
    isMovingLeftP4 = false;
    isMovingRightP4 = false;
}

void initPhase4() {
    srand((unsigned int)time(0));
    windowWidthP4 = 800;
    windowHeightP4 = 700;
    mouseXP4 = windowWidthP4 / 2;
    mouseYP4 = windowHeightP4 / 2;

    // Player no topo
    // Use the same player representation as Phase 1/2 so the ship looks identical
    // Place player at bottom of the screen (y = 0 baseline in drawPlayer)
    // BUT use smaller size like Phase 2 (80x60) so the whole ship is visible
        playerP4.width = 80.0f;   // smaller ship like Phase 2
        playerP4.height = 60.0f;
        playerP4.x = windowWidthP4 / 2.0f - playerP4.width / 2.0f; // center horizontally at bottom
        playerP4.y = 50.0f; // slightly above bottom edge so it's visible
        playerP4.speed = 5.0f; // movement speed for WASD
        playerP4.health = 100;
        playerP4.maxHealth = 100;
        playerP4.isHit = false;
        playerP4.hitTimer = 0;
        playerP4.shakeOffsetX = 0.0f;
        playerP4.shakeOffsetY = 0.0f;

    // Ensure the global game window sizes match phase4 so drawPlayer uses correct coords
    windowWidth_game = windowWidthP4;
    windowHeight_game = windowHeightP4;

    // Boss na parte de cima da tela
    bossP4.width = 80.0f;  // same size as player
    bossP4.height = 60.0f;
    bossP4.x = windowWidthP4 / 2.0f - bossP4.width / 2.0f;
    bossP4.y = windowHeightP4 - bossP4.height - 50.0f; // parte de cima
    bossP4.vx = 40.0f; // velocidade inicial aumentada (era 20.0f)
    bossP4.baseSpeed = 1.0f; // multiplicador inicial (era 0.5f)
    bossP4.hitCount = 0;
    bossP4.isHit = false;
    bossP4.hitTimer = 0;
    bossP4.active = true;
    bossP4.changeDirTimer = 0;
    bossP4.colorR = 0.8f; bossP4.colorG = 0.2f; bossP4.colorB = 0.2f; // cor vermelha

    initParallax();
    resetPhase4State();
    
    // Iniciar contagem regressiva
    showCountdownP4 = true;
    countdownTimerP4 = 0;
    countdownValueP4 = 3;

    setGameOver(false);
    initGameOver(windowWidthP4, windowHeightP4);
    registerRestartCallback(restartPhase4);
    registerMenuCallback(returnToMenuFromPhase4);

    // Play boss music for Phase 4
    Audio::getInstance().playMusic("assets/music/boss1.mp3");
}

void drawParallaxBackground() {
    // Fundo preto
    glColor3f(0.0f, 0.0f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(0,0);
    glVertex2f(windowWidthP4,0);
    glVertex2f(windowWidthP4,windowHeightP4);
    glVertex2f(0,windowHeightP4);
    glEnd();

    // Estrelas simples: diferentes camadas desenhadas como pontos
    for (size_t layer = 0; layer < parallaxLayersP4.size(); ++layer) {
        float factor = parallaxLayersP4[layer].speedFactor;
        (void)factor; // suppress unused-variable warning; kept for future use
        float alpha = 0.6f + 0.4f * (float)layer / parallaxLayersP4.size();
        glColor3f(1.0f*alpha, 1.0f*alpha, 1.0f*alpha);
        glPointSize((GLfloat)(1 + layer));
        glBegin(GL_POINTS);
        // deterministico based on layer to avoid random each frame
        int seed = 100 + (int)layer * 999;
        for (int i = 0; i < 80; ++i) {
            float sx = fmod((seed * (i+1) * 23.57f + parallaxLayersP4[layer].offsetX), (float)windowWidthP4);
            float sy = fmod((seed * (i+3) * 17.23f + parallaxLayersP4[layer].offsetY), (float)windowHeightP4);
            if (sx < 0) sx += windowWidthP4;
            if (sy < 0) sy += windowHeightP4;
            glVertex2f(sx, sy);
        }
        glEnd();
    }

    // Planetas (camada mais próxima) - círculos decorativos
    glColor3f(0.4f, 0.6f, 0.8f);
    float px = fmod(250.0f + parallaxLayersP4.back().offsetX * 0.2f, (float)windowWidthP4);
    float py = fmod(420.0f + parallaxLayersP4.back().offsetY * 0.1f, (float)windowHeightP4);
    float r = 40.0f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(px, py);
    for (int i = 0; i <= 24; ++i) {
        float a = (float)i / 24.0f * 2.0f * M_PI;
        glVertex2f(px + cos(a) * r, py + sin(a) * r);
    }
    glEnd();
}

void drawPlayerP4() {
    // Nave EXATAMENTE igual à da Fase 2, mas ROTACIONADA 90 graus anti-horário
    // Ponta agora aponta para CIMA (norte) em vez de direita (leste)
    
    float shakeX = playerP4.isHit ? playerP4.shakeOffsetX : 0.0f;
    float shakeY = playerP4.isHit ? playerP4.shakeOffsetY : 0.0f;
    
    float redIntensity = 0.0f;
    if (playerP4.isHit) {
        redIntensity = (playerP4.hitTimer % 10 < 5) ? 0.6f : 0.3f;
    }
    
    // Centro da nave (ponto de rotação)
    float centerX = playerP4.x + playerP4.width / 2.0f;
    float centerY = playerP4.y + playerP4.height / 2.0f;
    
    // Salvar estado atual da matriz
    glPushMatrix();
    
    // Aplicar tremor primeiro (antes da rotação)
    glTranslatef(shakeX, shakeY, 0.0f);
    
    // Mover para o centro da nave
    glTranslatef(centerX, centerY, 0.0f);
    
    // Rotacionar 90 graus anti-horário (ponta para cima)
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    
    // Agora desenhar a nave com centro em (0,0) - será rotacionada automaticamente
    float naveWidth = 80.0f;
    float naveHeight = 60.0f;
    
    // Corpo principal da nave (visão lateral)
    glColor3f(0.2f + redIntensity, 0.2f, 0.6f - redIntensity * 0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 0.0f); // Ponta frontal (centro)
    glVertex2f(-naveWidth * 0.3f, naveHeight * 0.5f);
    glVertex2f(-naveWidth * 0.5f, naveHeight * 0.4f);
    glVertex2f(-naveWidth * 0.5f, -naveHeight * 0.4f);
    glVertex2f(-naveWidth * 0.3f, -naveHeight * 0.5f);
    glEnd();
    
    // Asa superior
    glColor3f(0.15f + redIntensity, 0.15f, 0.5f - redIntensity * 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-naveWidth * 0.2f, naveHeight * 0.3f);
    glVertex2f(-naveWidth * 0.4f, naveHeight * 0.3f);
    glVertex2f(-naveWidth * 0.3f, naveHeight * 0.7f);
    glEnd();
    
    // Asa inferior
    glBegin(GL_TRIANGLES);
    glVertex2f(-naveWidth * 0.2f, -naveHeight * 0.3f);
    glVertex2f(-naveWidth * 0.4f, -naveHeight * 0.3f);
    glVertex2f(-naveWidth * 0.3f, -naveHeight * 0.7f);
    glEnd();
    
    // Cockpit/janela
    glColor3f(0.3f + redIntensity, 0.7f - redIntensity * 0.3f, 0.9f - redIntensity * 0.7f);
    glBegin(GL_POLYGON);
    glVertex2f(-naveWidth * 0.1f, 0.0f);
    glVertex2f(-naveWidth * 0.25f, naveHeight * 0.15f);
    glVertex2f(-naveWidth * 0.35f, naveHeight * 0.1f);
    glVertex2f(-naveWidth * 0.35f, -naveHeight * 0.1f);
    glVertex2f(-naveWidth * 0.25f, -naveHeight * 0.15f);
    glEnd();
    
    // Restaurar matriz
    glPopMatrix();
}

void drawBossP4() {
    if (!bossP4.active) return;
    float bx = bossP4.x;
    float by = bossP4.y;
    float shakeX = 0.0f;
    if (bossP4.isHit) {
        shakeX = (rand()%9 - 4);
    }

    // flash color when hit
    float r = bossP4.colorR;
    float g = bossP4.colorG;
    float b = bossP4.colorB;
    if (bossP4.isHit && (bossP4.hitTimer % 6 < 3)) {
        r = 1.0f; g = 1.0f; b = 1.0f;
    }
    
    // Draw boss ship similar to player but inverted (pointing down) and red
    // Main body (triangle pointing down)
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    // Nose pointing down
    glVertex2f(bx + shakeX + bossP4.width/2.0f, by); // bottom center (nose pointing down)
    glVertex2f(bx + shakeX, by + bossP4.height); // top left
    glVertex2f(bx + shakeX + bossP4.width, by + bossP4.height); // top right
    glEnd();
    
    // Wings (small triangles on sides) - darker red
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    // Left wing
    glBegin(GL_TRIANGLES);
    glVertex2f(bx + shakeX + bossP4.width * 0.2f, by + bossP4.height * 0.5f);
    glVertex2f(bx + shakeX, by + bossP4.height * 0.7f);
    glVertex2f(bx + shakeX + bossP4.width * 0.1f, by + bossP4.height * 0.8f);
    glEnd();
    // Right wing
    glBegin(GL_TRIANGLES);
    glVertex2f(bx + shakeX + bossP4.width * 0.8f, by + bossP4.height * 0.5f);
    glVertex2f(bx + shakeX + bossP4.width, by + bossP4.height * 0.7f);
    glVertex2f(bx + shakeX + bossP4.width * 0.9f, by + bossP4.height * 0.8f);
    glEnd();

    // Cockpit window (yellow-orange for enemy)
    glColor3f(1.0f, 0.7f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(bx + bossP4.width*0.4f, by + bossP4.height*0.4f);
    glVertex2f(bx + bossP4.width*0.6f, by + bossP4.height*0.4f);
    glVertex2f(bx + bossP4.width*0.55f, by + bossP4.height*0.7f);
    glVertex2f(bx + bossP4.width*0.45f, by + bossP4.height*0.7f);
    glEnd();
}

void drawBossProjectilesP4() {
    glColor3f(1.0f, 0.6f, 0.0f);
    for (auto &p : bossProjectilesP4) {
        if (!p.active) continue;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(p.x, p.y);
        int seg = 8;
        for (int i = 0; i <= seg; ++i) {
            float a = (float)i / seg * 2.0f * M_PI;
            glVertex2f(p.x + cos(a) * p.radius, p.y + sin(a) * p.radius);
        }
        glEnd();
    }
}

void drawAsteroidsP4() {
    glColor3f(0.5f, 0.45f, 0.35f);
    for (auto &a : asteroidsP4) {
        if (!a.active) continue;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(a.x, a.y);
        int seg = 10;
        for (int i = 0; i <= seg; ++i) {
            float ang = (float)i / seg * 2.0f * M_PI;
            glVertex2f(a.x + cos(ang) * a.radius, a.y + sin(ang) * a.radius);
        }
        glEnd();
    }
}

void drawLaserShotsP4() {
    // Desenhar tiros do jogador como projéteis verdes
    glColor3f(0.0f, 1.0f, 0.0f);
    for (auto &s : laserShotsP4) {
        if (!s.active) continue;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(s.x, s.y);
        int seg = 8;
        for (int i = 0; i <= seg; ++i) {
            float a = (float)i / seg * 2.0f * M_PI;
            glVertex2f(s.x + cos(a) * s.radius, s.y + sin(a) * s.radius);
        }
        glEnd();
    }
}

void drawHealthBarP4() {
    float barWidth = 200.0f;
    float barHeight = 25.0f;
    float barX = 10.0f;
    float barY = windowHeightP4 - 40.0f;
    
    // Fundo da barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Barra de vida
    float healthPercent = (float)playerP4.health / (float)playerP4.maxHealth;
    float currentBarWidth = barWidth * healthPercent;
    
    if (healthPercent > 0.6f) {
        glColor3f(0.0f, 0.8f, 0.0f); // Verde
    } else if (healthPercent > 0.3f) {
        glColor3f(0.8f, 0.8f, 0.0f); // Amarelo
    } else {
        glColor3f(0.8f, 0.0f, 0.0f); // Vermelho
    }
    
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + currentBarWidth, barY);
    glVertex2f(barX + currentBarWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Borda da barra
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    glLineWidth(1.0f);
    
    // Texto de vida em preto
    glColor3f(0.0f, 0.0f, 0.0f);
    std::ostringstream oss;
    oss << "Vida: " << playerP4.health << "/" << playerP4.maxHealth;
    std::string healthText = oss.str();
    glRasterPos2f(barX + 5.0f, barY + 8.0f);
    for (char c : healthText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // Texto de tiros abaixo da barra de vida
    glColor3f(1.0f, 1.0f, 1.0f); // branco
    std::ostringstream oss2;
    oss2 << "Tiros: " << shotsRemainingP4;
    std::string shotsText = oss2.str();
    glRasterPos2f(barX + 5.0f, barY - 15.0f);
    for (char c : shotsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void drawBossHealthBarP4() {
    float barWidth = 200.0f;
    float barHeight = 25.0f;
    float barX = windowWidthP4 - barWidth - 10.0f; // canto superior direito
    float barY = windowHeightP4 - 40.0f;
    
    // Fundo da barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Calcular "vida" do boss baseado em hits (6 hits para derrotar)
    float bossHealthPercent = 1.0f - ((float)bossP4.hitCount / (float)hitsNeededP4);
    float currentBarWidth = barWidth * bossHealthPercent;
    
    if (bossHealthPercent > 0.6f) {
        glColor3f(0.8f, 0.0f, 0.0f); // Vermelho (boss é inimigo)
    } else if (bossHealthPercent > 0.3f) {
        glColor3f(0.9f, 0.4f, 0.0f); // Laranja
    } else {
        glColor3f(0.9f, 0.7f, 0.0f); // Amarelo (quase derrotado)
    }
    
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + currentBarWidth, barY);
    glVertex2f(barX + currentBarWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Borda da barra
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    glLineWidth(1.0f);
}

void drawUI() {
    // UI vazia - informações movidas para as barras de vida
}

void drawCountdownP4() {
    if (!showCountdownP4 || countdownValueP4 <= 0) return;
    
    // Fundo semi-transparente escuro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 700);
    glVertex2f(0, 700);
    glEnd();
    glDisable(GL_BLEND);
    
    // Número da contagem
    std::string countText = std::to_string(countdownValueP4);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Posição centralizada para janela 800x700
    float textX = 400.0f - 20.0f;
    float textY = 350.0f;
    
    // Fonte grande
    glRasterPos2f(textX, textY);
    for (char c : countText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void drawPhase4() {
    drawParallaxBackground();
    drawBossP4();
    drawAsteroidsP4();
    drawBossProjectilesP4();
    // Draw the player's ship using custom top-down small ship
    drawPlayerP4();
    drawLaserShotsP4();
    drawHealthBarP4();
    drawBossHealthBarP4();
    drawUI();
    
    // Desenhar tela de pausa se ESC foi pressionado (ANTES da contagem)
    if (getPaused()) {
        drawPauseScreen();
        return;
    }
    
    // Desenhar contagem regressiva se ativa
    if (showCountdownP4) {
        drawCountdownP4();
        return;
    }

    if (bossDefeatedP4) {
        glColor3f(0.8f, 1.0f, 0.3f);
        std::string s = "BOSS DERROTADO!";
        glRasterPos2f(windowWidthP4/2.0f - 80.0f, windowHeightP4/2.0f);
        for (char c : s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    // Desenhar tela de game over se necessário
    drawGameOver();
}

void spawnAsteroidP4() {
    AsteroidP4 a;
    a.radius = randf(12.0f, 28.0f);
    a.x = randf(a.radius, windowWidthP4 - a.radius);
    a.y = windowHeightP4 + a.radius + 10.0f; // começa fora da tela no topo
    a.vx = randf(-20.0f, 20.0f);
    a.vy = randf(-40.0f, -100.0f); // desce
    a.active = true;
    asteroidsP4.push_back(a);
}

void spawnBossProjectileP4() {
    ProjectileP4 p;
    p.radius = 6.0f;
    // Spawn from the rear of the boss ship (top part since boss points down)
    p.x = bossP4.x + bossP4.width * randf(0.3f, 0.7f); // random position along width
    p.y = bossP4.y + bossP4.height; // rear of ship (top edge)
    
    // Fire straight down in a line (não segue o jogador)
    p.vx = 0.0f; // no horizontal movement
    p.vy = -120.0f * bossP4.baseSpeed / 60.0f; // move straight down (slower than before)
    p.active = true;
    bossProjectilesP4.push_back(p);
}

void fireLaserP4(float targetX, float targetY) {
    if (shotsRemainingP4 <= 0) return;
    LaserShotP4 s;
    // Posição inicial: centro do topo da nave do jogador
    s.x = playerP4.x + playerP4.width*0.5f;
    s.y = playerP4.y + playerP4.height;
    
    // Calcular direção do tiro baseado no clique
    float dx = targetX - s.x;
    float dy = targetY - s.y;
    float len = sqrt(dx*dx + dy*dy);
    if (len < 1.0f) len = 1.0f;
    
    // Velocidade do projétil (rápido mas não instantâneo)
    float speed = 8.0f; // pixels por frame
    s.vx = (dx / len) * speed;
    s.vy = (dy / len) * speed;
    s.radius = 5.0f;
    s.active = true;
    laserShotsP4.push_back(s);
    shotsRemainingP4--;
    // tocar som de tiro
    Audio::getInstance().play(Audio::SOUND_LASER);
}

void updatePhase4() {
    if (getGameOver() || getPaused()) return; // parar se gameover ou pausado

    // Atualizar contagem regressiva
    if (showCountdownP4) {
        countdownTimerP4++;
        if (countdownTimerP4 >= 60) { // 1 segundo a 60 FPS
            countdownTimerP4 = 0;
            countdownValueP4--;
            if (countdownValueP4 <= 0) {
                showCountdownP4 = false;
            }
        }
        return; // Não atualizar o jogo enquanto está contando
    }

    gameFrameCounterP4++;

    // Handle continuous player movement based on key state
    if (isMovingLeftP4) {
        playerP4.x -= playerP4.speed;
        if (playerP4.x < 0) playerP4.x = 0;
    }
    if (isMovingRightP4) {
        playerP4.x += playerP4.speed;
        if (playerP4.x + playerP4.width > windowWidthP4) playerP4.x = windowWidthP4 - playerP4.width;
    }
    if (isMovingUpP4) {
        playerP4.y += playerP4.speed;
        if (playerP4.y + playerP4.height > windowHeightP4 * 0.9f) playerP4.y = windowHeightP4 * 0.9f - playerP4.height; // don't go too high
    }
    if (isMovingDownP4) {
        playerP4.y -= playerP4.speed;
        if (playerP4.y < 10.0f) playerP4.y = 10.0f; // keep above bottom edge
    }

    // mover parallax baseado em jogador x (quem mexe é o fundo)
    float moveX = (playerP4.x + playerP4.width*0.5f) - (windowWidthP4/2.0f);
    for (auto &l : parallaxLayersP4) {
        l.offsetX += -moveX * (l.speedFactor) * 0.0015f; // sensibilidade
    }

    // atualizar boss movimento lateral com mudança aleatória de direção
    if (bossP4.active) {
        bossP4.changeDirTimer--;
        if (bossP4.changeDirTimer <= 0) {
            bossP4.changeDirTimer = rand() % 60 + 30; // muda direção com mais frequência (era 100+60)
            float dir = (rand() % 3) - 1; // -1,0,1
            bossP4.vx = dir * 100.0f * bossP4.baseSpeed / 60.0f; // velocidade aumentada (era 60.0f)
        }
        bossP4.x += bossP4.vx;
        // manter dentro da tela
        if (bossP4.x < 0) bossP4.x = 0, bossP4.vx = -bossP4.vx;
        if (bossP4.x + bossP4.width > windowWidthP4) bossP4.x = windowWidthP4 - bossP4.width, bossP4.vx = -bossP4.vx;

        // Boss projéteis (atira mais devagar)
        bossProjectileCooldownP4--;
        if (bossProjectileCooldownP4 <= 0) {
            spawnBossProjectileP4();
            // cooldown maior para atirar mais devagar (120 frames = 2 segundos a 60fps)
            bossProjectileCooldownP4 = std::max(60, (int)(120 * (1.0f / bossP4.baseSpeed)));
        }
    }

    // atualizar projectiles do boss
    for (auto &p : bossProjectilesP4) {
        if (!p.active) continue;
        p.x += p.vx;
        p.y += p.vy;
        // fora da tela
        if (p.y > windowHeightP4 + 50 || p.y < -50 || p.x < -50 || p.x > windowWidthP4 + 50) p.active = false;
        // colisão com jogador
        if (rectCircleCollision(playerP4.x, playerP4.y, playerP4.width, playerP4.height, p.x, p.y, p.radius)) {
            p.active = false;
            playerP4.health -= 15;
            playerP4.isHit = true;
            playerP4.hitTimer = 24;
            printf("Player health: %d\n", playerP4.health);
            if (playerP4.health <= 0) {
                playerP4.health = 0;
                printf("GAME OVER TRIGGERED!\n");
                // game over
                setGameOver(true);
            }
            // tocar som de dano
            Audio::getInstance().play(Audio::SOUND_DAMAGE);
        }
    }

    // atualizar asteroides
    asteroidSpawnCooldownP4--;
    if (asteroidSpawnCooldownP4 <= 0) {
        spawnAsteroidP4();
        asteroidSpawnCooldownP4 = rand() % 60 + 40; // spawn aleatório
    }
    for (auto &a : asteroidsP4) {
        if (!a.active) continue;
        a.x += a.vx / 60.0f;
        a.y += a.vy / 60.0f;
        if (a.y < -100.0f) a.active = false;
        if (rectCircleCollision(playerP4.x, playerP4.y, playerP4.width, playerP4.height, a.x, a.y, a.radius)) {
            a.active = false;
            playerP4.health -= 20;
            playerP4.isHit = true;
            playerP4.hitTimer = 24;
            printf("Player health (asteroid): %d\n", playerP4.health);
            if (playerP4.health <= 0) {
                playerP4.health = 0;
                printf("GAME OVER TRIGGERED (asteroid)!\n");
                setGameOver(true);
            }
            // tocar som de dano por asteroid
            Audio::getInstance().play(Audio::SOUND_DAMAGE);
        }
    }

    // atualizar tiros do jogador
    for (auto &s : laserShotsP4) {
        if (!s.active) continue;
        // Mover o projétil
        s.x += s.vx;
        s.y += s.vy;
        
        // Desativar se sair da tela
        if (s.y > windowHeightP4 + 50 || s.y < -50 || s.x < -50 || s.x > windowWidthP4 + 50) {
            s.active = false;
            continue;
        }
        
        // checar colisão com boss (usando colisão círculo-retângulo)
        if (bossP4.active) {
            if (rectCircleCollision(bossP4.x, bossP4.y, bossP4.width, bossP4.height, s.x, s.y, s.radius)) {
                s.active = false;
                bossP4.hitCount++;
                bossP4.isHit = true;
                bossP4.hitTimer = 18;
                // tocar som de explosão/hit no boss
                Audio::getInstance().play(Audio::SOUND_EXPLOSION);
                // Aumentar velocidade gradualmente a cada acerto (incremento menor e mais frequente)
                bossP4.baseSpeed += 0.15f; // aumenta 15% a cada hit (antes era 20% a cada 2 hits)
                // reduzir cooldown para mais ataques gradualmente
                bossProjectileCooldownP4 = std::max(30, (int)(bossProjectileCooldownP4 * 0.92f));
                if (bossP4.hitCount >= hitsNeededP4) {
                    bossP4.active = false;
                    bossDefeatedP4 = true;
                    // Parar música da fase 4
                    Audio::getInstance().stopMusic();
                    // Completou a Fase 4! Mostrar história da Fase 5 (3D)
                    showStoryForPhase(5);
                    return;
                }
            }
        }
    }

    // decrement hit timers
    if (playerP4.isHit) {
        playerP4.hitTimer--;
        playerP4.shakeOffsetX = (rand()%10 - 5) * 1.5f;
        playerP4.shakeOffsetY = (rand()%10 - 5) * 1.0f;
        if (playerP4.hitTimer <= 0) {
            playerP4.isHit = false;
            playerP4.shakeOffsetX = playerP4.shakeOffsetY = 0.0f;
        }
    }
    if (bossP4.isHit) {
        bossP4.hitTimer--;
        if (bossP4.hitTimer <= 0) bossP4.isHit = false;
    }
    
    // Verificar se os tiros acabaram e o boss ainda está vivo
    if (shotsRemainingP4 <= 0 && bossP4.active) {
        // Verificar se ainda há tiros ativos na tela
        bool hasActiveShotsP4 = false;
        for (const auto &s : laserShotsP4) {
            if (s.active) {
                hasActiveShotsP4 = true;
                break;
            }
        }
        // Se não há tiros restantes e nenhum tiro ativo, game over
        if (!hasActiveShotsP4) {
            setGameOver(true);
        }
    }
}

void handlePhase4Keyboard(unsigned char key) {
    // ESC para pausar
    if (key == 27) {
        if (!getGameOver() && !getPaused()) {
            setPaused(true, 4);
        } else if (getPaused()) {
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
    
    // Allow moving the player in all directions with WASD or arrow keys
    if (key == 'a' || key == 'A') {
        isMovingLeftP4 = true;
    } else if (key == 'd' || key == 'D') {
        isMovingRightP4 = true;
    } else if (key == 'w' || key == 'W') {
        isMovingUpP4 = true;
    } else if (key == 's' || key == 'S') {
        isMovingDownP4 = true;
    }
}

void handlePhase4KeyboardUp(unsigned char key) {
    // Stop movement when key is released
    if (key == 'a' || key == 'A') {
        isMovingLeftP4 = false;
    } else if (key == 'd' || key == 'D') {
        isMovingRightP4 = false;
    } else if (key == 'w' || key == 'W') {
        isMovingUpP4 = false;
    } else if (key == 's' || key == 'S') {
        isMovingDownP4 = false;
    }
}

void handlePhase4SpecialKey(int key, int x, int y) {
    (void)x; (void)y;
    // Arrow keys for movement
    if (key == GLUT_KEY_LEFT) {
        isMovingLeftP4 = true;
    } else if (key == GLUT_KEY_RIGHT) {
        isMovingRightP4 = true;
    } else if (key == GLUT_KEY_UP) {
        isMovingUpP4 = true;
    } else if (key == GLUT_KEY_DOWN) {
        isMovingDownP4 = true;
    }
}

void handlePhase4SpecialKeyUp(int key, int x, int y) {
    (void)x; (void)y;
    // Stop movement when arrow key is released
    if (key == GLUT_KEY_LEFT) {
        isMovingLeftP4 = false;
    } else if (key == GLUT_KEY_RIGHT) {
        isMovingRightP4 = false;
    } else if (key == GLUT_KEY_UP) {
        isMovingUpP4 = false;
    } else if (key == GLUT_KEY_DOWN) {
        isMovingDownP4 = false;
    }
}

void handlePhase4MouseMove(int x, int y) {
    mouseXP4 = x;
    mouseYP4 = y;
}

void handlePhase4MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float targetX = (float)x;
        float targetY = windowHeightP4 - (float)y; // coordenada Y invertida
        fireLaserP4(targetX, targetY);
    }
}

void restartPhase4() {
    initPhase4();
}

void returnToMenuFromPhase4() {
    // Chamado pelo gameover/menu - apenas voltar ao menu principal
    setGameOver(false);
    setVictory(false);
    setPaused(false, 0);
    changeState(MAIN_MENU);
    // Restore menu music
    Audio::getInstance().playMusic("assets/music/menu.wav");
    glutPostRedisplay();
}
