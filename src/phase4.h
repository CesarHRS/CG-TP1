#ifndef PHASE4_H
#define PHASE4_H

#include <vector>
#include <string>

// Estruturas para a fase 4 (boss fight com parallax)
struct PlayerPhase4 {
    float x, y, width, height;
    int health;
    int maxHealth;
    bool isHit;
    int hitTimer;
    float shakeOffsetX;
    float shakeOffsetY;
};

struct BossPhase4 {
    float x, y, width, height;
    float vx;            // velocidade lateral
    float baseSpeed;     // velocidade base
    int hitCount;        // quantos acertos recebeu
    bool active;
    int changeDirTimer;  // contador para mudar direção
    bool isHit;          // efeito quando recebe tiro
    int hitTimer;
    float colorR, colorG, colorB;
};

struct ProjectileP4 {
    float x, y;
    float vx, vy;
    float radius;
    bool active;
};

struct AsteroidP4 {
    float x, y;
    float vx, vy;
    float radius;
    bool active;
};

struct LaserShotP4 {
    float x, y;      // posição atual do projétil
    float vx, vy;    // velocidade do projétil
    float radius;    // tamanho do projétil
    bool active;
};

struct ParallaxLayer {
    float speedFactor; // quanto se move relativo ao jogador
    float offsetX;
    float offsetY;
    // Dados visuais simplificados (gerados em init)
};

// Variáveis globais da fase 4
extern PlayerPhase4 playerP4;
extern BossPhase4 bossP4;
extern std::vector<ProjectileP4> bossProjectilesP4;
extern std::vector<AsteroidP4> asteroidsP4;
extern std::vector<LaserShotP4> laserShotsP4;
extern std::vector<ParallaxLayer> parallaxLayersP4;

extern int windowWidthP4;
extern int windowHeightP4;
extern int mouseXP4;
extern int mouseYP4;

extern int shotsRemainingP4; // 10 tiros
extern int hitsNeededP4;    // 6 acertos para derrotar
extern bool bossDefeatedP4;

// Funções
void initPhase4();
void drawPhase4();
void updatePhase4();

void handlePhase4Keyboard(unsigned char key);
void handlePhase4KeyboardUp(unsigned char key);
void handlePhase4SpecialKey(int key, int x, int y);
void handlePhase4SpecialKeyUp(int key, int x, int y);
void handlePhase4MouseMove(int x, int y);
void handlePhase4MouseClick(int button, int state, int x, int y);

// Callbacks
void restartPhase4();
void returnToMenuFromPhase4();

#endif // PHASE4_H
