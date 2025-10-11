#ifndef GAME_H
#define GAME_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath> 

struct Player {
    float x, y, width, height;
    float speed;
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

// Funções de desenho
void drawPlayer();
void drawEnemies();

void initGame();
void drawGame();
void updateGame();
void handleGameKeyboard(unsigned char key);
void handleGameKeyboardUp(unsigned char key);
void spawnEnemy();

#endif // GAME_H
