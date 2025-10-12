#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath> 

struct Player {
    float x, y, width, height;
    float speed;
    int health;      // Vida do jogador
    int maxHealth;   // Vida máxima
};

struct Enemy {
    float x, y, width, height;
    float speed;
    bool hasQuestion;  // Se este asteroide tem uma questão ativa
    int questionNum1;  // Primeiro número da soma
    int questionNum2;  // Segundo número da soma
};

// Estrutura para questão de matemática
struct MathQuestion {
    bool active;           // Se há uma questão ativa
    int num1;              // Primeiro número
    int num2;              // Segundo número
    int correctAnswer;     // Resposta correta
    std::string userAnswer; // Resposta digitada pelo usuário
    int asteroidIndex;     // Índice do asteroide associado
    bool showError;        // Mostrar erro (fundo vermelho)
    int errorTimer;        // Contador de frames para erro (30 frames = 0.5s a 60 FPS)
};

// Variáveis globais
extern Player player;
extern std::vector<Enemy> enemies;
extern int windowWidth_game;
extern int windowHeight_game;
extern bool isMovingLeft;
extern bool isMovingRight;
extern int spawnCooldown; // Cooldown entre spawns de asteroides
extern int mouseX; // Posição X do mouse
extern int mouseY; // Posição Y do mouse
extern MathQuestion currentQuestion; // Questão matemática atual

// Funções de desenho
void drawPlayer();
void drawEnemies();
void drawHealthBar();
void drawCrosshair(); // Desenhar mira
void drawMathQuestion(); // Desenhar questão matemática

void initGame();
void drawGame();
void updateGame();
void handleGameKeyboard(unsigned char key);
void handleGameKeyboardUp(unsigned char key);
void handleGameMouseMove(int x, int y); // Movimentação do mouse
void handleGameMouseClick(int button, int state, int x, int y); // Click do mouse
void spawnEnemy();

// Callbacks para o sistema de Game Over
void restartCurrentPhase();
void returnToMenu();

#endif // GAME_H
