#include <GL/glut.h>
#include "game.h"
#include "menu.h"
#include "phase2.h"
#include "gameover.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player player;
std::vector<Enemy> enemies;
std::vector<Explosion> explosions; // Lista de explosões ativas
LaserShot laserShot = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false}; // Tiro laser
int windowWidth_game = 800;  
int windowHeight_game = 600; 

bool isMovingLeft = false;
bool isMovingRight = false;
int spawnCooldown = 0; // Contador de frames entre spawns
int gameFrameCounter = 0; // Contador global de frames para animações
int mouseX = 400; // Posição inicial do mouse (centro)
int mouseY = 300;
// Init MathQuestion: active=false, num1=0, num2=0, correctAnswer=0, userAnswer="", asteroidIndex=-1, op='+', showError=false, errorTimer=0
MathQuestion currentQuestion = {false, 0, 0, 0, "", -1, '+', false, 0};
int correctAnswersCount = 0;
int correctAnswersTarget = 10; // Vencer após 10 acertos
bool phase1Completed = false; // Se completou a fase 1

void drawPlayer() {
    // Nave vista de cima, apenas a parte frontal (15% inferior da tela)
    // Para-brisa ENORME ocupando ~70% da tela horizontalmente
    
    // Aplicar tremor se foi atingido
    float shakeX = player.isHit ? player.shakeOffsetX : 0.0f;
    float shakeY = player.isHit ? player.shakeOffsetY : 0.0f;
    
    // Calcular intensidade do efeito vermelho (0.0 a 1.0)
    float redIntensity = 0.0f;
    if (player.isHit) {
        // Piscar vermelho nos primeiros 15 frames
        redIntensity = (player.hitTimer % 10 < 5) ? 0.6f : 0.3f;
    }
    
    float baseY = windowHeight_game * 0.15f; // 15% da altura
    float parabrisa_width = windowWidth_game * 0.35f; // 70% total da tela (35% cada lado)
    
    // Asa esquerda (muito pequena)
    glColor3f(0.2f + redIntensity, 0.2f, 0.6f - redIntensity * 0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(0 + shakeX, 0 + shakeY);
    glVertex2f(0 + shakeX, baseY * 0.3f + shakeY);
    glVertex2f(player.x - parabrisa_width - 50 + shakeX, baseY * 0.2f + shakeY);
    glVertex2f(player.x - parabrisa_width - 40 + shakeX, 0 + shakeY);
    glEnd();
    
    // Asa direita (muito pequena)
    glBegin(GL_POLYGON);
    glVertex2f(windowWidth_game + shakeX, 0 + shakeY);
    glVertex2f(windowWidth_game + shakeX, baseY * 0.3f + shakeY);
    glVertex2f(player.x + parabrisa_width + 50 + shakeX, baseY * 0.2f + shakeY);
    glVertex2f(player.x + parabrisa_width + 40 + shakeX, 0 + shakeY);
    glEnd();
    
    // Corpo lateral esquerdo (parte da fuselagem)
    glColor3f(0.15f + redIntensity, 0.15f, 0.5f - redIntensity * 0.5f); // Azul escuro
    glBegin(GL_POLYGON);
    glVertex2f(player.x - parabrisa_width - 40 + shakeX, 0 + shakeY);
    glVertex2f(player.x - parabrisa_width - 50 + shakeX, baseY * 0.2f + shakeY);
    glVertex2f(player.x - parabrisa_width + shakeX, baseY * 0.8f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.9f + shakeX, 0 + shakeY);
    glEnd();
    
    // Corpo lateral direito (parte da fuselagem)
    glBegin(GL_POLYGON);
    glVertex2f(player.x + parabrisa_width + 40 + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width + 50 + shakeX, baseY * 0.2f + shakeY);
    glVertex2f(player.x + parabrisa_width + shakeX, baseY * 0.8f + shakeY);
    glVertex2f(player.x + parabrisa_width * 0.9f + shakeX, 0 + shakeY);
    glEnd();
    
    // Nariz central (ponta da nave apontando para cima)
    glColor3f(0.25f + redIntensity, 0.25f, 0.7f - redIntensity * 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(player.x + shakeX, baseY + shakeY); // Ponta superior
    glVertex2f(player.x - parabrisa_width * 0.9f + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width * 0.9f + shakeX, 0 + shakeY);
    glEnd();
    
    // Para-brisa/Cockpit ENORME (vidro transparente/azul claro) - 70% da tela
    glColor3f(0.25f + redIntensity, 0.6f - redIntensity * 0.3f, 0.9f - redIntensity * 0.7f); // Azul claro brilhante
    glBegin(GL_POLYGON);
    glVertex2f(player.x + shakeX, baseY * 0.98f + shakeY); // Ponta quase no topo
    glVertex2f(player.x - parabrisa_width + shakeX, baseY * 0.6f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.85f + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width * 0.85f + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width + shakeX, baseY * 0.6f + shakeY);
    glEnd();
    
    // Reflexo no vidro (detalhe maior)
    glColor3f(0.8f + redIntensity * 0.2f, 0.92f - redIntensity * 0.5f, 1.0f - redIntensity);
    glBegin(GL_POLYGON);
    glVertex2f(player.x - parabrisa_width * 0.1f + shakeX, baseY * 0.85f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.6f + shakeX, baseY * 0.55f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.5f + shakeX, baseY * 0.3f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.15f + shakeX, baseY * 0.5f + shakeY);
    glEnd();
    
    // Moldura do para-brisa (cinza escuro - mais grossa)
    glColor3f(0.25f + redIntensity, 0.25f, 0.35f);
    glLineWidth(6.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(player.x + shakeX, baseY * 0.98f + shakeY);
    glVertex2f(player.x - parabrisa_width + shakeX, baseY * 0.6f + shakeY);
    glVertex2f(player.x - parabrisa_width * 0.85f + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width * 0.85f + shakeX, 0 + shakeY);
    glVertex2f(player.x + parabrisa_width + shakeX, baseY * 0.6f + shakeY);
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

// Desenhar mira (crosshair) na posição do mouse
void drawCrosshair() {
    // Converter coordenadas do mouse (Y invertido)
    float crosshairX = (float)mouseX;
    float crosshairY = windowHeight_game - (float)mouseY;
    
    float size = 15.0f; // Tamanho da mira
    float gap = 5.0f;   // Espaço no centro
    float thickness = 2.5f;
    
    // Cor da mira (verde brilhante com contorno)
    glLineWidth(thickness);
    
    // Contorno preto para melhor visibilidade
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(thickness + 1.5f);
    
    // Linha horizontal (esquerda)
    glBegin(GL_LINES);
    glVertex2f(crosshairX - size, crosshairY);
    glVertex2f(crosshairX - gap, crosshairY);
    glEnd();
    
    // Linha horizontal (direita)
    glBegin(GL_LINES);
    glVertex2f(crosshairX + gap, crosshairY);
    glVertex2f(crosshairX + size, crosshairY);
    glEnd();
    
    // Linha vertical (cima)
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY + gap);
    glVertex2f(crosshairX, crosshairY + size);
    glEnd();
    
    // Linha vertical (baixo)
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY - size);
    glVertex2f(crosshairX, crosshairY - gap);
    glEnd();
    
    // Mira verde brilhante
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(thickness);
    
    // Linha horizontal (esquerda)
    glBegin(GL_LINES);
    glVertex2f(crosshairX - size, crosshairY);
    glVertex2f(crosshairX - gap, crosshairY);
    glEnd();
    
    // Linha horizontal (direita)
    glBegin(GL_LINES);
    glVertex2f(crosshairX + gap, crosshairY);
    glVertex2f(crosshairX + size, crosshairY);
    glEnd();
    
    // Linha vertical (cima)
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY + gap);
    glVertex2f(crosshairX, crosshairY + size);
    glEnd();
    
    // Linha vertical (baixo)
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY - size);
    glVertex2f(crosshairX, crosshairY - gap);
    glEnd();
    
    // Círculo central pequeno
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(crosshairX, crosshairY);
    glEnd();
    
    glLineWidth(1.0f);
    glPointSize(1.0f);
}

// Desenhar explosões
void drawExplosions() {
    for (const auto& explosion : explosions) {
        if (!explosion.active) continue;
        
        // Desenhar cada partícula da explosão
        for (const auto& particle : explosion.particles) {
            if (particle.life <= 0.0f) continue;
            
            // Calcular opacidade baseada na vida da partícula
            float alpha = particle.life;
            
            // Desenhar partícula como círculo
            glColor4f(particle.r, particle.g, particle.b, alpha);
            
            // Desenhar círculo preenchido para a partícula
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(particle.x, particle.y);
            
            int segments = 8;
            for (int i = 0; i <= segments; i++) {
                float angle = (float)i / (float)segments * 2.0f * M_PI;
                float px = particle.x + cos(angle) * particle.size;
                float py = particle.y + sin(angle) * particle.size;
                glVertex2f(px, py);
            }
            glEnd();
        }
    }
}

// Desenhar tiro laser
void drawLaserShot() {
    if (!laserShot.active) return;
    
    // Calcular posição atual do laser baseado no progresso
    float currentX = laserShot.startX + (laserShot.endX - laserShot.startX) * laserShot.currentProgress;
    float currentY = laserShot.startY + (laserShot.endY - laserShot.startY) * laserShot.currentProgress;
    
    // Desenhar linha do laser (verde brilhante)
    glLineWidth(3.0f);
    glColor3f(0.0f, 1.0f, 0.0f); // Verde
    glBegin(GL_LINES);
    glVertex2f(laserShot.startX, laserShot.startY);
    glVertex2f(currentX, currentY);
    glEnd();
    
    // Efeito de brilho (linha mais fina e clara)
    glLineWidth(1.5f);
    glColor3f(0.5f, 1.0f, 0.5f); // Verde claro
    glBegin(GL_LINES);
    glVertex2f(laserShot.startX, laserShot.startY);
    glVertex2f(currentX, currentY);
    glEnd();
    
    // Ponto de impacto brilhante
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glVertex2f(currentX, currentY);
    glEnd();
    
    glLineWidth(1.0f);
    glPointSize(1.0f);
}

// Desenhar barra de progresso vertical no canto direito
void drawProgressBar() {
    // Tamanho e posição
    float barWidth = 30.0f;
    float barHeight = windowHeight_game * 0.6f; // 60% da altura
    float barX = windowWidth_game - 40.0f; // 40px da borda direita
    float barY = (windowHeight_game - barHeight) / 2.0f; // centralizada verticalmente

    // Fundo da barra (cinza escuro)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();

    // Altura preenchida proporcional aos acertos
    float fillPercentage = 0.0f;
    if (correctAnswersTarget > 0) {
        fillPercentage = (float)correctAnswersCount / (float)correctAnswersTarget;
        if (fillPercentage > 1.0f) fillPercentage = 1.0f;
    }
    float filledHeight = barHeight * fillPercentage;

    // Cores do preenchimento (de vermelho para verde conforme avança)
    float r = 1.0f - fillPercentage;
    float g = 0.2f + 0.8f * fillPercentage;
    glColor3f(r, g, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + filledHeight);
    glVertex2f(barX, barY + filledHeight);
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

    // Desenhar texto com contagem (ex: 3/10) acima da barra
    std::stringstream ss;
    ss << correctAnswersCount << "/" << correctAnswersTarget;
    std::string txt = ss.str();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(barX - 40.0f, barY + barHeight + 20.0f);
    for (const char c : txt) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

// Desenhar questão matemática no para-brisa
void drawMathQuestion() {
    if (!currentQuestion.active) {
        return;
    }
    
    // Área do para-brisa (centro da tela, parte inferior)
    float questionY = windowHeight_game * 0.08f; // 8% da altura (no para-brisa)
    float questionX = windowWidth_game / 2.0f;
    
    // Fundo semi-transparente para a questão (vermelho se erro, preto normal)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (currentQuestion.showError) {
        glColor4f(0.8f, 0.0f, 0.0f, 0.8f); // Vermelho semi-transparente
    } else {
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Preto semi-transparente
    }
    
    glBegin(GL_QUADS);
    glVertex2f(questionX - 150, questionY - 20);
    glVertex2f(questionX + 150, questionY - 20);
    glVertex2f(questionX + 150, questionY + 40);
    glVertex2f(questionX - 150, questionY + 40);
    glEnd();
    glDisable(GL_BLEND);
    
    // Borda da caixa de questão (vermelha se erro, ciano normal)
    if (currentQuestion.showError) {
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho brilhante
    } else {
        glColor3f(0.0f, 1.0f, 1.0f); // Ciano brilhante
    }
    
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(questionX - 150, questionY - 20);
    glVertex2f(questionX + 150, questionY - 20);
    glVertex2f(questionX + 150, questionY + 40);
    glVertex2f(questionX - 150, questionY + 40);
    glEnd();
    glLineWidth(1.0f);
    
    // Montar o texto da questão
    std::stringstream ss;
    // Mostrar o operador associado à questão
    ss << currentQuestion.num1 << " " << currentQuestion.op << " " << currentQuestion.num2 << " = " << currentQuestion.userAnswer;
    std::string questionText = ss.str();
    
    // Desenhar o texto
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    glRasterPos2f(questionX - 70, questionY + 10);
    for (const char c : questionText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    // Cursor piscante (indicador de digitação)
    static int blinkCounter = 0;
    blinkCounter++;
    if ((blinkCounter / 30) % 2 == 0) { // Pisca a cada 30 frames
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
        float cursorX = questionX - 70 + questionText.length() * 10;
        glBegin(GL_LINES);
        glVertex2f(cursorX, questionY);
        glVertex2f(cursorX, questionY + 20);
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
    newEnemy.hasQuestion = false;
    newEnemy.questionNum1 = 0;
    newEnemy.questionNum2 = 0;
    newEnemy.questionOp = '+';
    
    enemies.push_back(newEnemy);
}



void initGame() {
    srand(time(0));

    player.width = 200.0f;  // Largura para controlar proporções da nave
    player.height = 90.0f;  // 15% da altura (90px de 600px)
    player.x = windowWidth_game / 2.0f;
    player.y = 0.0f; // Nave na parte inferior
    player.speed = 15.0f;
    player.health = 100;    // Vida inicial
    player.maxHealth = 100; // Vida máxima
    player.isHit = false;   // Não foi atingido
    player.hitTimer = 0;    // Timer zerado
    player.shakeOffsetX = 0.0f; // Sem tremor
    player.shakeOffsetY = 0.0f; // Sem tremor

    enemies.clear();
    spawnCooldown = 0; // Resetar cooldown para spawnar imediatamente
    setGameOver(false); // Resetar estado do game over
    // Resetar progresso de acertos
    correctAnswersCount = 0;
    phase1Completed = false; // Resetar flag de conclusão
    
    // Registrar callbacks do Game Over
    initGameOver(windowWidth_game, windowHeight_game);
    registerRestartCallback(restartCurrentPhase);
    registerMenuCallback(returnToMenu);
    
    // Esconder cursor padrão do sistema
    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawGame() {
    drawPlayer();
    drawEnemies();
    drawExplosions(); // Desenhar explosões
    drawLaserShot(); // Desenhar tiro laser
    drawHealthBar();
    drawMathQuestion(); // Desenhar questão matemática (se ativa)
    drawCrosshair(); // Desenhar mira customizada
    drawProgressBar(); // Desenhar barra de progresso vertical no canto direito
    
    // Desenhar tela de Game Over se necessário
    drawGameOver();
}

bool checkCollision(const Enemy& enemy) {
    // A nave ocupa 15% da parte inferior da tela (0 até windowHeight_game * 0.15)
    float naveBaseY = windowHeight_game * 0.15f;
    
    // Verificar se o asteroide está na região da nave (15% inferior)
    if (enemy.y > naveBaseY) {
        return false; // Asteroide ainda não chegou na nave
    }
    
    // Asteroide está na altura da nave
    // A nave ocupa TODA a largura da tela (asas vão de 0 até windowWidth_game)
    float enemyCenterY = enemy.y + enemy.height / 2.0f;
    
    // Colisão ocorre se o asteroide está na altura da nave (15% inferior)
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
    
    // Atualizar explosões
    updateExplosions();
    
    // Atualizar tiro laser
    updateLaserShot();
    
    // Atualizar efeito de dano (tremor)
    if (player.isHit && player.hitTimer > 0) {
        player.hitTimer--;
        
        // Calcular tremor (mais intenso no início)
        float shakeIntensity = (float)player.hitTimer / 30.0f * 8.0f; // Máximo de 8 pixels
        player.shakeOffsetX = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity; // -shakeIntensity a +shakeIntensity
        player.shakeOffsetY = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
        
        if (player.hitTimer <= 0) {
            player.isHit = false;
            player.shakeOffsetX = 0.0f;
            player.shakeOffsetY = 0.0f;
        }
    }
    
    // Decrementar timer de erro
    if (currentQuestion.showError && currentQuestion.errorTimer > 0) {
        currentQuestion.errorTimer--;
        if (currentQuestion.errorTimer <= 0) {
            currentQuestion.showError = false;
            currentQuestion.userAnswer = ""; // Limpar resposta após erro
        }
    }
    
    // Nave agora é fixa - sem movimento lateral
    // Removido: isMovingLeft e isMovingRight

    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].y -= enemies[i].speed;
    }

    for (size_t i = 0; i < enemies.size(); ) {
        // Verificar colisão com a nave
        if (checkCollision(enemies[i])) {
            // Se este asteroide tem a questão ativa, cancelar a questão
            if (currentQuestion.active && currentQuestion.asteroidIndex == (int)i) {
                currentQuestion.active = false;
                currentQuestion.userAnswer = "";
                currentQuestion.asteroidIndex = -1;
            }
            
            player.health -= 20; // Perde 20 de vida por colisão
            
            // Ativar efeito de dano
            player.isHit = true;
            player.hitTimer = 30; // 30 frames = 0.5 segundos de efeito
            
            if (player.health <= 0) {
                player.health = 0; // Não deixa ficar negativo
                setGameOver(true); // Ativar tela de Game Over
            }
            
            enemies.erase(enemies.begin() + i); // Remove o asteroide após colisão
            
            // Atualizar índice da questão se necessário
            if (currentQuestion.active && currentQuestion.asteroidIndex > (int)i) {
                currentQuestion.asteroidIndex--;
            }
        } else if (enemies[i].y < -enemies[i].height) {
            // Se este asteroide saiu da tela e tem a questão ativa, cancelar
            if (currentQuestion.active && currentQuestion.asteroidIndex == (int)i) {
                currentQuestion.active = false;
                currentQuestion.userAnswer = "";
                currentQuestion.asteroidIndex = -1;
            }
            
            enemies.erase(enemies.begin() + i); // Remove se saiu da tela
            
            // Atualizar índice da questão se necessário
            if (currentQuestion.active && currentQuestion.asteroidIndex > (int)i) {
                currentQuestion.asteroidIndex--;
            }
        } else {
            ++i;
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
    
    // Incrementar contador global de frames
    gameFrameCounter++;
}

// Atualizar explosões
void updateExplosions() {
    for (auto& explosion : explosions) {
        if (!explosion.active) continue;
        
        // Atualizar cada partícula
        for (auto& particle : explosion.particles) {
            // Atualizar posição com velocidade
            particle.x += particle.vx;
            particle.y += particle.vy;
            
            // Reduzir vida da partícula
            particle.life -= 0.05f; // Diminuir vida gradualmente
            
            // Desacelerar partículas
            particle.vx *= 0.95f;
            particle.vy *= 0.95f;
        }
        
        // Incrementar timer
        explosion.timer++;
        
        // Desativar explosão se o timer passou de 30 frames (~0.5s)
        if (explosion.timer > 30) {
            explosion.active = false;
        }
    }
    
    // Remover explosões inativas
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
            [](const Explosion& e) { return !e.active; }),
        explosions.end()
    );
}

// Atualizar tiro laser
void updateLaserShot() {
    if (!laserShot.active) return;
    
    // Incrementar progresso (velocidade rápida)
    laserShot.currentProgress += 0.15f; // 15% por frame = ~7 frames para chegar
    
    // Desativar quando completar
    if (laserShot.currentProgress >= 1.0f) {
        laserShot.active = false;
    }
}

void handleGameKeyboard(unsigned char key) {
    // Se há uma questão ativa, processar input numérico
    if (currentQuestion.active) {
        if (key >= '0' && key <= '9') {
            // Adicionar dígito à resposta (apenas se não estiver mostrando erro E tiver menos de 3 dígitos)
            if (!currentQuestion.showError && currentQuestion.userAnswer.length() < 3) {
                currentQuestion.userAnswer += key;
                glutPostRedisplay();
            }
        } else if (key == 8 || key == 127) { // Backspace ou Delete
            // Remover último dígito (apenas se não estiver mostrando erro)
            if (!currentQuestion.showError && !currentQuestion.userAnswer.empty()) {
                currentQuestion.userAnswer.pop_back();
                glutPostRedisplay();
            }
        } else if (key == 13 || key == '\r') { // Enter
            // Não processar Enter se já estiver mostrando erro
            if (currentQuestion.showError) {
                return;
            }
            
            // Verificar resposta
            int userAnswerInt = 0;
            if (!currentQuestion.userAnswer.empty()) {
                userAnswerInt = std::stoi(currentQuestion.userAnswer);
            }
            
            if (userAnswerInt == currentQuestion.correctAnswer) {
                // Resposta correta! Criar tiro laser, explosão e remover o asteroide
                if (currentQuestion.asteroidIndex >= 0 && 
                    currentQuestion.asteroidIndex < (int)enemies.size()) {
                    
                    // Posição do asteroide alvo
                    float asteroidCenterX = enemies[currentQuestion.asteroidIndex].x + enemies[currentQuestion.asteroidIndex].width / 2.0f;
                    float asteroidCenterY = enemies[currentQuestion.asteroidIndex].y + enemies[currentQuestion.asteroidIndex].height / 2.0f;
                    
                    // Criar tiro laser do centro da nave até o asteroide
                    laserShot.startX = player.x;
                    laserShot.startY = windowHeight_game * 0.15f; // Topo da nave
                    laserShot.endX = asteroidCenterX;
                    laserShot.endY = asteroidCenterY;
                    laserShot.currentProgress = 0.0f;
                    laserShot.active = true;
                    
                    // Criar explosão na posição do asteroide
                    Explosion explosion;
                    explosion.x = asteroidCenterX;
                    explosion.y = asteroidCenterY;
                    explosion.timer = 0;
                    explosion.active = true;
                    
                    // Criar 12 partículas em diferentes direções
                    int numParticles = 12;
                    for (int i = 0; i < numParticles; i++) {
                        Particle p;
                        float angle = (float)i / (float)numParticles * 2.0f * M_PI;
                        float speed = 2.0f + (float)(rand() % 100) / 50.0f; // Velocidade entre 2.0 e 4.0
                        
                        p.x = explosion.x;
                        p.y = explosion.y;
                        p.vx = cos(angle) * speed;
                        p.vy = sin(angle) * speed;
                        p.life = 1.0f;
                        p.size = 3.0f + (float)(rand() % 100) / 50.0f; // Tamanho entre 3.0 e 5.0
                        
                        // Cores amarelo-laranja-vermelho para efeito de fogo
                        float colorVariation = (float)(rand() % 100) / 100.0f;
                        p.r = 1.0f;
                        p.g = 0.5f + colorVariation * 0.5f; // 0.5 a 1.0
                        p.b = 0.0f;
                        
                        explosion.particles.push_back(p);
                    }
                    
                    explosions.push_back(explosion);
                    
                    // Remover asteroide
                    enemies.erase(enemies.begin() + currentQuestion.asteroidIndex);
                }
                
                // Limpar questão
                currentQuestion.active = false;
                currentQuestion.userAnswer = "";
                currentQuestion.asteroidIndex = -1;
                currentQuestion.showError = false;
                currentQuestion.errorTimer = 0;
                
                // Incrementar contador de acertos e verificar vitória
                correctAnswersCount++;
                if (correctAnswersCount >= correctAnswersTarget) {
                    // Completou a Fase 1! Ir para Fase 2
                    phase1Completed = true;
                    currentState = PHASE2_SCREEN;
                    initPhase2();
                    glutPassiveMotionFunc(handlePhase2MouseMove);
                    glutMotionFunc(handlePhase2MouseMove);
                    return;
                }

                // Adiantar próximo asteroide (resetar cooldown)
                spawnCooldown = 0;
            } else {
                // Resposta errada - mostrar erro
                currentQuestion.showError = true;
                currentQuestion.errorTimer = 30; // 30 frames = 0.5 segundos a 60 FPS
                
                // Asteroide continua
                if (currentQuestion.asteroidIndex >= 0 && 
                    currentQuestion.asteroidIndex < (int)enemies.size()) {
                    enemies[currentQuestion.asteroidIndex].hasQuestion = false;
                }
            }
            glutPostRedisplay();
        } else {
            // Tecla inválida (não é número, backspace ou enter)
            // Mostrar erro por 0.5 segundos
            currentQuestion.showError = true;
            currentQuestion.errorTimer = 30; // 30 frames = 0.5 segundos a 60 FPS
            glutPostRedisplay();
        }
        return; // Não processar outras teclas quando questão ativa
    }
    
    // Input normal do jogo
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
    correctAnswersCount = 0;
}

void returnToMenu() {
    // Volta ao menu principal
    extern void changeState(int newState);
    changeState(0); // 0 = MAIN_MENU
    
    // Restaurar cursor normal no menu
    glutSetCursor(GLUT_CURSOR_INHERIT);
}

// Função para capturar movimento do mouse
void handleGameMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
    glutPostRedisplay(); // Atualizar tela
}

// Função para capturar clique do mouse
void handleGameMouseClick(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
        return;
    }
    
    // Se já tem uma questão ativa, não fazer nada
    if (currentQuestion.active) {
        return;
    }
    
    // Converter coordenadas do mouse para OpenGL
    float clickX = (float)x;
    float clickY = windowHeight_game - (float)y;
    
    // Verificar se clicou em algum asteroide
    for (size_t i = 0; i < enemies.size(); ++i) {
        Enemy& enemy = enemies[i];
        
        // Verificar se o clique está dentro do asteroide
        if (clickX >= enemy.x && clickX <= enemy.x + enemy.width &&
            clickY >= enemy.y && clickY <= enemy.y + enemy.height) {
            
            // Criar questão matemática com operação aleatória (+, -)
            currentQuestion.active = true;
            int opType = rand() % 3; // 0:+, 1:-
            currentQuestion.num1 = (rand() % 20) + 1; // 1 a 20
            currentQuestion.num2 = (rand() % 20) + 1; // 1 a 20
            if (opType == 0) {
                currentQuestion.op = '+';
                currentQuestion.correctAnswer = currentQuestion.num1 + currentQuestion.num2;
            } else{
                currentQuestion.op = '-';
                // garantir resultado não-negativo para facilitar
                if (currentQuestion.num1 < currentQuestion.num2) std::swap(currentQuestion.num1, currentQuestion.num2);
                currentQuestion.correctAnswer = currentQuestion.num1 - currentQuestion.num2;
            }
            currentQuestion.userAnswer = "";
            currentQuestion.asteroidIndex = i;

            // Marcar asteroide como tendo questão
            enemy.hasQuestion = true;
            enemy.questionNum1 = currentQuestion.num1;
            enemy.questionNum2 = currentQuestion.num2;
            enemy.questionOp = currentQuestion.op;
            
            break; // Só uma questão por vez
        }
    }
    
    glutPostRedisplay();
}
