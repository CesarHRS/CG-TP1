#ifndef PHASE7_H
#define PHASE7_H

#include <GL/glut.h>

// --- ESTRUTURAS ---

typedef struct {
    float x, y, z;
    float angle;
    float pitch;
} PlayerP7;

typedef enum {
    P7_SHAPE_CUBE,
    P7_SHAPE_SPHERE,
    P7_SHAPE_PYRAMID,
    P7_SHAPE_CYLINDER
} ShapeTypeP7;

typedef struct {
    float x, y, z;
    ShapeTypeP7 type;
    bool collected;
    bool held;
} CollectibleP7;

typedef struct {
    float x, y, z;
    ShapeTypeP7 requiredType;
    bool filled;
} DepositZoneP7;

typedef struct {
    float x, y, z;
    float startX, startZ;
    int hp;
    bool active;
    float moveDir;
    int moveAxis;
    int shootTimer;
} EnemyP7;

typedef struct {
    float x, y, z;
    float dirX, dirZ;
    bool active;
} ProjectileP7;

// --- EXTERN ---

extern PlayerP7 playerP7;
extern CollectibleP7 objectsP7[];
extern DepositZoneP7 depositsP7[];
extern int numObjectsP7;
extern int numDepositsP7;
extern float timeRemainingP7;
extern bool gameOverP7;

extern int phase7_municao;
extern int phase7_vida;
extern bool phase7_won;
extern bool phase7_gameOver;

extern EnemyP7 enemiesP7[];
extern int numEnemiesP7;
extern ProjectileP7 enemyProjectiles[];
extern int maxProjectilesP7;

// NOVAS VARIÁVEIS PARA ANIMAÇÃO DO TIRO
extern int laserAnimTimer;
extern float laserEndX, laserEndY, laserEndZ;

// Declarações de Funções
void initPhase7();
void drawPhase7(int windowWidth, int windowHeight);
void drawPhase7();
void updatePhase7(int value);

// Inputs
void handlePhase7Keyboard(unsigned char key, int x, int y);
void handlePhase7KeyboardUp(unsigned char key, int x, int y);
void handlePhase7Mouse(int button, int state, int x, int y);
void handlePhase7PassiveMotion(int x, int y);
void handlePhase7MouseClick(int button, int state, int x, int y);
void handlePhase7MouseMove(int x, int y);
void handlePhase7SpecialKey(int key, int x, int y);
void handlePhase7SpecialKeyUp(int key, int x, int y);

#endif // PHASE7_H