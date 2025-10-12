#include <GL/glut.h>
#include "game.h"


Player player;
std::vector<Enemy> enemies;
int windowWidth_game = 800;  
int windowHeight_game = 600; 

bool isMovingLeft = false;
bool isMovingRight = false;
int spawnCooldown = 0; // Contador de frames entre spawns


void drawPlayer() {
    // Nave vista de cima, apenas a parte frontal (10% inferior da tela)
    // Para-brisa ENORME ocupando ~70% da tela horizontalmente
    
    float baseY = windowHeight_game * 0.1f; // 10% da altura
    float parabrisa_width = windowWidth_game * 0.35f; // 70% total da tela (35% cada lado)
    
    // Asa esquerda (muito pequena)
    glColor3f(0.2f, 0.2f, 0.6f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(0, baseY * 0.3f);
    glVertex2f(player.x - parabrisa_width - 50, baseY * 0.2f);
    glVertex2f(player.x - parabrisa_width - 40, 0);
    glEnd();
    
    // Asa direita (muito pequena)
    glBegin(GL_POLYGON);
    glVertex2f(windowWidth_game, 0);
    glVertex2f(windowWidth_game, baseY * 0.3f);
    glVertex2f(player.x + parabrisa_width + 50, baseY * 0.2f);
    glVertex2f(player.x + parabrisa_width + 40, 0);
    glEnd();
    
    // Corpo lateral esquerdo (parte da fuselagem)
    glColor3f(0.15f, 0.15f, 0.5f); // Azul escuro
    glBegin(GL_POLYGON);
    glVertex2f(player.x - parabrisa_width - 40, 0);
    glVertex2f(player.x - parabrisa_width - 50, baseY * 0.2f);
    glVertex2f(player.x - parabrisa_width, baseY * 0.8f);
    glVertex2f(player.x - parabrisa_width * 0.9f, 0);
    glEnd();
    
    // Corpo lateral direito (parte da fuselagem)
    glBegin(GL_POLYGON);
    glVertex2f(player.x + parabrisa_width + 40, 0);
    glVertex2f(player.x + parabrisa_width + 50, baseY * 0.2f);
    glVertex2f(player.x + parabrisa_width, baseY * 0.8f);
    glVertex2f(player.x + parabrisa_width * 0.9f, 0);
    glEnd();
    
    // Nariz central (ponta da nave apontando para cima)
    glColor3f(0.25f, 0.25f, 0.7f);
    glBegin(GL_TRIANGLES);
    glVertex2f(player.x, baseY); // Ponta superior
    glVertex2f(player.x - parabrisa_width * 0.9f, 0);
    glVertex2f(player.x + parabrisa_width * 0.9f, 0);
    glEnd();
    
    // Para-brisa/Cockpit ENORME (vidro transparente/azul claro) - 70% da tela
    glColor3f(0.25f, 0.6f, 0.9f); // Azul claro brilhante
    glBegin(GL_POLYGON);
    glVertex2f(player.x, baseY * 0.98f); // Ponta quase no topo
    glVertex2f(player.x - parabrisa_width, baseY * 0.6f);
    glVertex2f(player.x - parabrisa_width * 0.85f, 0);
    glVertex2f(player.x + parabrisa_width * 0.85f, 0);
    glVertex2f(player.x + parabrisa_width, baseY * 0.6f);
    glEnd();
    
    // Reflexo no vidro (detalhe maior)
    glColor3f(0.8f, 0.92f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(player.x - parabrisa_width * 0.1f, baseY * 0.85f);
    glVertex2f(player.x - parabrisa_width * 0.6f, baseY * 0.55f);
    glVertex2f(player.x - parabrisa_width * 0.5f, baseY * 0.3f);
    glVertex2f(player.x - parabrisa_width * 0.15f, baseY * 0.5f);
    glEnd();
    
    // Moldura do para-brisa (cinza escuro - mais grossa)
    glColor3f(0.25f, 0.25f, 0.35f);
    glLineWidth(6.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(player.x, baseY * 0.98f);
    glVertex2f(player.x - parabrisa_width, baseY * 0.6f);
    glVertex2f(player.x - parabrisa_width * 0.85f, 0);
    glVertex2f(player.x + parabrisa_width * 0.85f, 0);
    glVertex2f(player.x + parabrisa_width, baseY * 0.6f);
    glEnd();
    glLineWidth(1.0f);
}

void drawEnemies() {
    for (const auto& enemy : enemies) {
        // Cores de asteroide
        glColor3f(0.5f + 0.2f * (rand() % 3), 0.4f, 0.3f + 0.1f * (rand() % 2));
        
        glBegin(GL_POLYGON);
        int numVertices = 8 + rand() % 4; // 8 a 11 vértices
        float cx = enemy.x + enemy.width / 2.0f;
        float cy = enemy.y + enemy.height / 2.0f;
        float radius = enemy.width / 2.0f;
        for (int i = 0; i < numVertices; ++i) {
            float angle = 2.0f * 3.1415926f * i / numVertices;
            float r = radius * (0.7f + 0.3f * ((rand() % 100) / 100.0f)); // Irregularidade
            float x = cx + r * cos(angle);
            float y = cy + r * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
        
        // Detalhes (crateras)
        glColor3f(0.3f, 0.25f, 0.2f);
        
        glBegin(GL_POINTS);
        for (int i = 0; i < 5; ++i) {
            float angle = 2.0f * 3.1415926f * (rand() % 100) / 100.0f;
            float r = radius * 0.5f * (0.5f + 0.5f * ((rand() % 100) / 100.0f));
            float x = cx + r * cos(angle);
            float y = cy + r * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }
}


void spawnEnemy() {
    Enemy newEnemy;
    newEnemy.width = 40;  // Aumentado de 30 para 40 (+33%)
    newEnemy.height = 40; // Aumentado de 30 para 40 (+33%)
    newEnemy.x = rand() % (windowWidth_game - (int)newEnemy.width);
    newEnemy.y = windowHeight_game;
    newEnemy.speed = 0.4f; // Velocidade constante
    
    enemies.push_back(newEnemy);
}



void initGame() {
    srand(time(0));

    player.width = 200.0f;  // Largura para controlar proporções da nave
    player.height = 60.0f;  // 10% da altura (60px de 600px)
    player.x = windowWidth_game / 2.0f;
    player.y = 0.0f; // Nave na parte inferior
    player.speed = 15.0f;

    enemies.clear();
}

void drawGame() {
    drawPlayer();
    drawEnemies();
}

void updateGame() {
    
    // Nave agora é fixa - sem movimento lateral
    // Removido: isMovingLeft e isMovingRight

    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].y -= enemies[i].speed;
    }

    for (auto it = enemies.begin(); it != enemies.end(); ) {
        if (it->y < -it->height) {
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }

    // Decrementar cooldown a cada frame
    if (spawnCooldown > 0) {
        spawnCooldown--;
    }

    // Spawnar novo inimigo apenas se houver menos de 10 na tela E cooldown zerado
    if (enemies.size() < 10 && spawnCooldown == 0) {
        spawnEnemy();
        spawnCooldown = 1200; // 1200 frames de espera (~20 segundos a 60 FPS)
    }
}

void handleGameKeyboard(unsigned char key) {
    switch (key) {
        case 'a':
        case 'A':
            isMovingLeft = true;
            break;
        case 'd':
        case 'D':
            isMovingRight = true;
            break;
    }
}

void handleGameKeyboardUp(unsigned char key) {
    switch (key) {
        case 'a':
        case 'A':
            isMovingLeft = false;
            break;
        case 'd':
        case 'D':
            isMovingRight = false;
            break;
    }
}
