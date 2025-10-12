#ifndef GAME_H
#define GAME_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath> 

// Estados internos do jogo (jogando ou game over)
enum InGameState {
    PLAYING,
    GAME_OVER
};

struct Player {
    float x, y, width, height;
    float speed;
    int health;      // Vida do jogador
    int maxHealth;   // Vida máxima
};

struct Enemy {
    float x, y, width, height;
    float speed;
};

// Variáveis globais
extern Player player;
extern std::vector<Enemy> enemies;
extern int windowWidth_game;
extern int windowHeight_game;
extern bool isMovingLeft;
extern bool isMovingRight;
extern int spawnCooldown; // Cooldown entre spawns de asteroides
extern InGameState currentInGameState;

// Funções de desenho
void drawPlayer();
void drawEnemies();
void drawHealthBar();
void drawGameOver();

void initGame();
void drawGame();
void updateGame();
void handleGameKeyboard(unsigned char key);
void handleGameKeyboardUp(unsigned char key);
void handleGameOverKeyboard(unsigned char key);
void spawnEnemy();

#endif // GAME_H
