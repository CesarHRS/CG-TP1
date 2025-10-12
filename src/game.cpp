#include <GL/glut.h>
#include "game.h"
#include "gameover.h"

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

void drawHealthBar() {
    // Barra de vida no canto superior esquerdo
    float barWidth = 200.0f;
    float barHeight = 25.0f;
    float barX = 20.0f;
    float barY = windowHeight_game - 40.0f;
    
    // Fundo da barra (vermelho escuro)
    glColor3f(0.3f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Barra de vida (verde para saudável, amarelo para médio, vermelho para baixo)
    float healthPercentage = (float)player.health / (float)player.maxHealth;
    float currentBarWidth = barWidth * healthPercentage;
    
    // Cor baseada na porcentagem de vida
    if (healthPercentage > 0.6f) {
        glColor3f(0.0f, 0.8f, 0.0f); // Verde
    } else if (healthPercentage > 0.3f) {
        glColor3f(0.9f, 0.9f, 0.0f); // Amarelo
    } else {
        glColor3f(0.9f, 0.0f, 0.0f); // Vermelho
    }
    
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + currentBarWidth, barY);
    glVertex2f(barX + currentBarWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Borda da barra (branco)
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
    player.health = 100;    // Vida inicial
    player.maxHealth = 100; // Vida máxima

    enemies.clear();
    spawnCooldown = 0; // Resetar cooldown para spawnar imediatamente
    setGameOver(false); // Resetar estado do game over
    
    // Registrar callbacks do Game Over
    initGameOver(windowWidth_game, windowHeight_game);
    registerRestartCallback(restartCurrentPhase);
    registerMenuCallback(returnToMenu);
}

void drawGame() {
    drawPlayer();
    drawEnemies();
    drawHealthBar();
    
    // Desenhar tela de Game Over se necessário
    drawGameOver();
}

bool checkCollision(const Enemy& enemy) {
    // A nave ocupa 10% da parte inferior da tela (0 até windowHeight_game * 0.1)
    float naveBaseY = windowHeight_game * 0.1f;
    
    // Verificar se o asteroide está na região da nave (10% inferior)
    if (enemy.y > naveBaseY) {
        return false; // Asteroide ainda não chegou na nave
    }
    
    // Asteroide está na altura da nave
    // A nave ocupa TODA a largura da tela (asas vão de 0 até windowWidth_game)
    float enemyCenterY = enemy.y + enemy.height / 2.0f;
    
    // Colisão ocorre se o asteroide está na altura da nave (10% inferior)
    // Como a nave ocupa toda a largura, qualquer asteroide nessa altura colide
    if (enemyCenterY <= naveBaseY) {
        return true;
    }
    
    return false;
}

void updateGame() {
    // Se o jogo acabou, não atualizar
    if (getGameOver()) {
        return;
    }
    
    // Nave agora é fixa - sem movimento lateral
    // Removido: isMovingLeft e isMovingRight

    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].y -= enemies[i].speed;
    }

    for (auto it = enemies.begin(); it != enemies.end(); ) {
        // Verificar colisão com a nave
        if (checkCollision(*it)) {
            player.health -= 20; // Perde 20 de vida por colisão
            if (player.health <= 0) {
                player.health = 0; // Não deixa ficar negativo
                setGameOver(true); // Ativar tela de Game Over
            }
            it = enemies.erase(it); // Remove o asteroide após colisão
        } else if (it->y < -it->height) {
            it = enemies.erase(it); // Remove se saiu da tela
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

// Callbacks para o sistema de Game Over
void restartCurrentPhase() {
    // Reinicia apenas a fase atual (Fase 1)
    player.health = 100;
    player.maxHealth = 100;
    enemies.clear();
    spawnCooldown = 0;
    setGameOver(false);
}

void returnToMenu() {
    // Volta ao menu principal
    extern void changeState(int newState);
    changeState(0); // 0 = MAIN_MENU
}
