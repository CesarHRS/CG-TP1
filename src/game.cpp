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
    glColor3f(0.2f, 0.2f, 0.8f); // Azul escuro
    glBegin(GL_POLYGON);
    glVertex2f(player.x, player.y + player.height); // topo
    glVertex2f(player.x - player.width * 0.25f, player.y + player.height * 0.5f);
    glVertex2f(player.x - player.width * 0.25f, player.y);
    glVertex2f(player.x + player.width * 0.25f, player.y);
    glVertex2f(player.x + player.width * 0.25f, player.y + player.height * 0.5f);
    glEnd();

    // Cockpit
    glColor3f(0.6f, 0.9f, 1.0f); // Azul claro
    glBegin(GL_POLYGON);
    glVertex2f(player.x, player.y + player.height * 0.85f);
    glVertex2f(player.x - player.width * 0.10f, player.y + player.height * 0.55f);
    glVertex2f(player.x + player.width * 0.10f, player.y + player.height * 0.55f);
    glEnd();

    // Asa esquerda
    glColor3f(0.7f, 0.7f, 0.7f); // Cinza
    glBegin(GL_TRIANGLES);
    glVertex2f(player.x - player.width * 0.25f, player.y + player.height * 0.5f);
    glVertex2f(player.x - player.width * 0.5f, player.y + player.height * 0.2f);
    glVertex2f(player.x - player.width * 0.25f, player.y);
    glEnd();

    // Asa direita
    glBegin(GL_TRIANGLES);
    glVertex2f(player.x + player.width * 0.25f, player.y + player.height * 0.5f);
    glVertex2f(player.x + player.width * 0.5f, player.y + player.height * 0.2f);
    glVertex2f(player.x + player.width * 0.25f, player.y);
    glEnd();

    // Detalhe na ponta
    glColor3f(1.0f, 0.3f, 0.3f); // Vermelho
    glBegin(GL_TRIANGLES);
    glVertex2f(player.x, player.y + player.height);
    glVertex2f(player.x - player.width * 0.07f, player.y + player.height * 0.92f);
    glVertex2f(player.x + player.width * 0.07f, player.y + player.height * 0.92f);
    glEnd();
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
    newEnemy.width = 30;
    newEnemy.height = 30;
    newEnemy.x = rand() % (windowWidth_game - (int)newEnemy.width);
    newEnemy.y = windowHeight_game;
    newEnemy.speed = 0.4f; // Velocidade constante
    
    enemies.push_back(newEnemy);
}



void initGame() {
    srand(time(0));

    player.width = 50.0f;
    player.height = 50.0f;
    player.x = windowWidth_game / 2.0f;
    player.y = 50.0f;
    player.speed = 15.0f;

    enemies.clear();
}

void drawGame() {
    drawPlayer();
    drawEnemies();
}

void updateGame() {
    
    if (isMovingLeft) {
        player.x -= player.speed * 0.5f; 
    }
    if (isMovingRight) {
        player.x += player.speed * 0.5f;
    }
    if (player.x < player.width / 2) {
        player.x = player.width / 2;
    }
    if (player.x > windowWidth_game - player.width / 2) {
        player.x = windowWidth_game - player.width / 2;
    }

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
