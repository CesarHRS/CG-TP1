#ifndef PHASE2_H
#define PHASE2_H

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct PlayerPhase2 {
    float x, y, width, height;
    int health;
    int maxHealth;
    bool isHit;
    int hitTimer;
    float shakeOffsetX;
    float shakeOffsetY;
};

struct AsteroidPhase2 {
    float x, y, width, height;
    float speed;
    int displayNumber;      // Número mostrado no asteroide
    bool isCorrectAnswer;   // Se este tem a resposta correta
    bool isDestroyed;       // Se foi destruído pelo tiro
};

struct LaserShotPhase2 {
    float startX, startY;
    float endX, endY;
    float currentProgress;
    bool active;
};

struct ParticlePhase2 {
    float x, y;
    float vx, vy;
    float life;
    float size;
    float r, g, b;
};

struct ExplosionPhase2 {
    float x, y;
    std::vector<ParticlePhase2> particles;
    int timer;
    bool active;
};

struct EquationPhase2 {
    int num1;
    int num2;
    char operation;  // '*' ou '/'
    int correctAnswer;
    bool active;
};

// Variáveis globais
extern PlayerPhase2 playerP2;
extern std::vector<AsteroidPhase2> asteroidsP2;
extern std::vector<ExplosionPhase2> explosionsP2;
extern std::vector<LaserShotPhase2> laserShotsP2;
extern EquationPhase2 currentEquationP2;
extern int windowWidthP2;
extern int windowHeightP2;
extern int mouseXP2;
extern int mouseYP2;
extern int shotsRemainingP2;  // Tiros restantes (máximo 5)
extern int gameTimerP2;       // Timer do jogo (em frames)
extern int maxGameTimeP2;     // Tempo máximo (em frames)
extern int spawnCooldownP2;   // Cooldown entre spawns
extern int gameFrameCounterP2;
extern int asteroidsSpawnedP2; // Quantos já foram spawnados
extern int correctAsteroidIndexP2; // Índice do asteroide com resposta correta
extern int correctAnswersCountP2; // Contador de respostas corretas
extern int correctAnswersTargetP2; // Meta de respostas corretas (10)
extern bool allAsteroidsSpawnedP2; // Se todos já foram criados

// Funções de desenho
void drawPlayerP2();
void drawAsteroidsP2();
void drawHealthBarP2();
void drawCrosshairP2();
void drawEquationP2();
void drawExplosionsP2();
void drawLaserShotsP2();
void drawShotsRemainingP2();
void drawGameTimerP2();
void drawCorrectAnswersCountP2();

// Funções principais
void initPhase2();
void drawPhase2();
void updatePhase2();
void updateExplosionsP2();
void updateLaserShotsP2();
void handlePhase2Keyboard(unsigned char key);
void handlePhase2KeyboardUp(unsigned char key);
void handlePhase2MouseMove(int x, int y);
void handlePhase2MouseClick(int button, int state, int x, int y);
void spawnAsteroidP2();
void createNewEquationP2();
void checkTimeoutDamageP2();

// Callbacks para o sistema de Game Over
void restartPhase2();
void returnToMenuFromPhase2();

#endif // PHASE2_H
