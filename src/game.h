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
    bool isHit;      // Se foi atingido recentemente
    int hitTimer;    // Contador de frames do efeito de dano
    float shakeOffsetX; // Deslocamento horizontal do tremor
    float shakeOffsetY; // Deslocamento vertical do tremor
};

struct Enemy {
    float x, y, width, height;
    float speed;
    bool hasQuestion;  // Se este asteroide tem uma questão ativa
    int questionNum1;  // Primeiro número da soma
    int questionNum2;  // Segundo número da soma
    char questionOp;   // Operador da questão associada ('+', '-', '*')
};

// Estrutura para partículas de explosão
struct Particle {
    float x, y;           // Posição
    float vx, vy;         // Velocidade
    float life;           // Tempo de vida (0.0 a 1.0)
    float size;           // Tamanho da partícula
    float r, g, b;        // Cor
};

// Estrutura para explosão
struct Explosion {
    float x, y;                    // Posição central
    std::vector<Particle> particles; // Partículas
    int timer;                     // Tempo de vida da explosão
    bool active;                   // Se está ativa
};

// Estrutura para tiro laser
struct LaserShot {
    float startX, startY;  // Posição inicial (nave)
    float endX, endY;      // Posição final (asteroide)
    float currentProgress; // Progresso da animação (0.0 a 1.0)
    bool active;           // Se está ativo
};

// Estrutura para questão de matemática
struct MathQuestion {
    bool active;           // Se há uma questão ativa
    int num1;              // Primeiro número
    int num2;              // Segundo número
    int correctAnswer;     // Resposta correta
    std::string userAnswer; // Resposta digitada pelo usuário
    int asteroidIndex;     // Índice do asteroide associado
    char op;               // Operador da questão ('+', '-', '*')
    bool showError;        // Mostrar erro (fundo vermelho)
    int errorTimer;        // Contador de frames para erro (30 frames = 0.5s a 60 FPS)
};

// Variáveis globais
extern Player player;
extern std::vector<Enemy> enemies;
extern std::vector<Explosion> explosions; // Lista de explosões ativas
extern LaserShot laserShot; // Tiro laser
extern int windowWidth_game;
extern int windowHeight_game;
extern bool isMovingLeft;
extern bool isMovingRight;
extern int spawnCooldown; // Cooldown entre spawns de asteroides
extern int mouseX; // Posição X do mouse
extern int mouseY; // Posição Y do mouse
extern MathQuestion currentQuestion; // Questão matemática atual
// Progresso de acertos
extern int correctAnswersCount; // Quantos acertos o jogador tem
extern int correctAnswersTarget; // Quantos acertos para vencer
extern bool phase1Completed; // Se a fase 1 foi completada com sucesso

// Desenho da barra de progresso
void drawProgressBar();

// Funções de desenho
void drawPlayer();
void drawEnemies();
void drawHealthBar();
void drawCrosshair(); // Desenhar mira
void drawMathQuestion(); // Desenhar questão matemática
void drawExplosions(); // Desenhar explosões
void drawLaserShot(); // Desenhar tiro laser

void initGame();
void drawGame();
void updateGame();
void updateExplosions(); // Atualizar explosões
void updateLaserShot(); // Atualizar tiro laser
void handleGameKeyboard(unsigned char key);
void handleGameKeyboardUp(unsigned char key);
void handleGameMouseMove(int x, int y); // Movimentação do mouse
void handleGameMouseClick(int button, int state, int x, int y); // Click do mouse
void spawnEnemy();

// Callbacks para o sistema de Game Over
void restartCurrentPhase();
void returnToMenu();

#endif // GAME_H
