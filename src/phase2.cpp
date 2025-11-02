#include <GL/glut.h>
#include "phase2.h"
#include "phase3.h"
#include "game.h"
#include "menu.h"
#include "gameover.h"
#include "audio.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PlayerPhase2 playerP2;
std::vector<AsteroidPhase2> asteroidsP2;
std::vector<ExplosionPhase2> explosionsP2;
std::vector<LaserShotPhase2> laserShotsP2;
EquationPhase2 currentEquationP2;
int windowWidthP2 = 800;
int windowHeightP2 = 600;
int mouseXP2 = 400;
int mouseYP2 = 300;
int shotsRemainingP2 = 5;
int gameTimerP2 = 0;
int maxGameTimeP2 = 1200; // 20 segundos a 60 FPS
int spawnCooldownP2 = 0;
int gameFrameCounterP2 = 0;
int asteroidsSpawnedP2 = 0;
int correctAsteroidIndexP2 = 0;
int correctAnswersCountP2 = 0;
int correctAnswersTargetP2 = 10;
bool allAsteroidsSpawnedP2 = false;

void drawPlayerP2() {
    // Nave na lateral esquerda (visão lateral)
    float shakeX = playerP2.isHit ? playerP2.shakeOffsetX : 0.0f;
    float shakeY = playerP2.isHit ? playerP2.shakeOffsetY : 0.0f;
    
    float redIntensity = 0.0f;
    if (playerP2.isHit) {
        redIntensity = (playerP2.hitTimer % 10 < 5) ? 0.6f : 0.3f;
    }
    
    float naveX = windowWidthP2 * 0.1f; // 10% da largura (lateral esquerda)
    float naveY = playerP2.y;
    float naveWidth = 80.0f;
    float naveHeight = 60.0f;
    
    // Corpo principal da nave (visão lateral)
    glColor3f(0.2f + redIntensity, 0.2f, 0.6f - redIntensity * 0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(naveX + shakeX, naveY + shakeY); // Ponta frontal
    glVertex2f(naveX - naveWidth * 0.3f + shakeX, naveY + naveHeight * 0.5f + shakeY);
    glVertex2f(naveX - naveWidth * 0.5f + shakeX, naveY + naveHeight * 0.4f + shakeY);
    glVertex2f(naveX - naveWidth * 0.5f + shakeX, naveY - naveHeight * 0.4f + shakeY);
    glVertex2f(naveX - naveWidth * 0.3f + shakeX, naveY - naveHeight * 0.5f + shakeY);
    glEnd();
    
    // Asa superior
    glColor3f(0.15f + redIntensity, 0.15f, 0.5f - redIntensity * 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(naveX - naveWidth * 0.2f + shakeX, naveY + naveHeight * 0.3f + shakeY);
    glVertex2f(naveX - naveWidth * 0.4f + shakeX, naveY + naveHeight * 0.3f + shakeY);
    glVertex2f(naveX - naveWidth * 0.3f + shakeX, naveY + naveHeight * 0.7f + shakeY);
    glEnd();
    
    // Asa inferior
    glBegin(GL_TRIANGLES);
    glVertex2f(naveX - naveWidth * 0.2f + shakeX, naveY - naveHeight * 0.3f + shakeY);
    glVertex2f(naveX - naveWidth * 0.4f + shakeX, naveY - naveHeight * 0.3f + shakeY);
    glVertex2f(naveX - naveWidth * 0.3f + shakeX, naveY - naveHeight * 0.7f + shakeY);
    glEnd();
    
    // Cockpit/janela
    glColor3f(0.3f + redIntensity, 0.7f - redIntensity * 0.3f, 0.9f - redIntensity * 0.7f);
    glBegin(GL_POLYGON);
    glVertex2f(naveX - naveWidth * 0.1f + shakeX, naveY + shakeY);
    glVertex2f(naveX - naveWidth * 0.25f + shakeX, naveY + naveHeight * 0.15f + shakeY);
    glVertex2f(naveX - naveWidth * 0.35f + shakeX, naveY + naveHeight * 0.1f + shakeY);
    glVertex2f(naveX - naveWidth * 0.35f + shakeX, naveY - naveHeight * 0.1f + shakeY);
    glVertex2f(naveX - naveWidth * 0.25f + shakeX, naveY - naveHeight * 0.15f + shakeY);
    glEnd();
}

void drawAsteroidsP2() {
    // Animação mais lenta (muda a cada 20 frames) - parar no game over
    int slowAnimationSeed = getGameOver() ? 0 : gameFrameCounterP2 / 20;
    
    for (size_t idx = 0; idx < asteroidsP2.size(); ++idx) {
        const auto& asteroid = asteroidsP2[idx];
        if (asteroid.isDestroyed) continue;
        
        srand(static_cast<unsigned int>(idx * 1000 + slowAnimationSeed));
        
        // Desenhar asteroide
        glColor3f(0.5f + 0.2f * (rand() % 3), 0.4f, 0.3f + 0.1f * (rand() % 2));
        
        glBegin(GL_POLYGON);
        int numVertices = 8 + rand() % 4;
        float cx = asteroid.x + asteroid.width / 2.0f;
        float cy = asteroid.y + asteroid.height / 2.0f;
        float radius = asteroid.width / 2.0f;
        for (int i = 0; i < numVertices; ++i) {
            float angle = 2.0f * M_PI * i / numVertices;
            float r = radius * (0.7f + 0.3f * ((rand() % 100) / 100.0f));
            float x = cx + r * cos(angle);
            float y = cy + r * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
        
        // Desenhar número no asteroide
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(cx - 10.0f, cy - 5.0f);
        
        std::string numStr = std::to_string(asteroid.displayNumber);
        for (char c : numStr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
    
    srand(static_cast<unsigned int>(time(0)));
}

void drawHealthBarP2() {
    float barWidth = 200.0f;
    float barHeight = 25.0f;
    float barX = 10.0f;
    float barY = windowHeightP2 - 40.0f;
    
    // Fundo da barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Barra de vida
    float healthPercent = (float)playerP2.health / (float)playerP2.maxHealth;
    float currentBarWidth = barWidth * healthPercent;
    
    if (healthPercent > 0.6f) {
        glColor3f(0.0f, 0.8f, 0.0f);
    } else if (healthPercent > 0.3f) {
        glColor3f(0.8f, 0.8f, 0.0f);
    } else {
        glColor3f(0.8f, 0.0f, 0.0f);
    }
    
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + currentBarWidth, barY);
    glVertex2f(barX + currentBarWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    
    // Borda
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

void drawCrosshairP2() {
    float crosshairX = (float)mouseXP2;
    float crosshairY = windowHeightP2 - (float)mouseYP2;
    
    float size = 15.0f;
    float gap = 5.0f;
    float thickness = 2.5f;
    
    // Contorno preto
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(thickness + 1.5f);
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX - size, crosshairY);
    glVertex2f(crosshairX - gap, crosshairY);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX + gap, crosshairY);
    glVertex2f(crosshairX + size, crosshairY);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY + gap);
    glVertex2f(crosshairX, crosshairY + size);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY - size);
    glVertex2f(crosshairX, crosshairY - gap);
    glEnd();
    
    // Mira verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(thickness);
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX - size, crosshairY);
    glVertex2f(crosshairX - gap, crosshairY);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX + gap, crosshairY);
    glVertex2f(crosshairX + size, crosshairY);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY + gap);
    glVertex2f(crosshairX, crosshairY + size);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(crosshairX, crosshairY - size);
    glVertex2f(crosshairX, crosshairY - gap);
    glEnd();
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(crosshairX, crosshairY);
    glEnd();
    
    glLineWidth(1.0f);
    glPointSize(1.0f);
}

void drawEquationP2() {
    if (!currentEquationP2.active) return;
    
    // Desenhar equação abaixo da nave
    float equationX = windowWidthP2 * 0.1f;
    float equationY = 100.0f;
    
    // Fundo semi-transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(equationX - 60.0f, equationY - 20.0f);
    glVertex2f(equationX + 60.0f, equationY - 20.0f);
    glVertex2f(equationX + 60.0f, equationY + 20.0f);
    glVertex2f(equationX - 60.0f, equationY + 20.0f);
    glEnd();
    glDisable(GL_BLEND);
    
    // Bordas
    glColor3f(0.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(equationX - 60.0f, equationY - 20.0f);
    glVertex2f(equationX + 60.0f, equationY - 20.0f);
    glVertex2f(equationX + 60.0f, equationY + 20.0f);
    glVertex2f(equationX - 60.0f, equationY + 20.0f);
    glEnd();
    glLineWidth(1.0f);
    
    // Texto da equação
    std::ostringstream oss;
    oss << currentEquationP2.num1 << " " << currentEquationP2.operation << " " << currentEquationP2.num2 << " = ?";
    std::string equationText = oss.str();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(equationX - 40.0f, equationY - 5.0f);
    for (char c : equationText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawExplosionsP2() {
    for (const auto& explosion : explosionsP2) {
        if (!explosion.active) continue;
        
        for (const auto& particle : explosion.particles) {
            if (particle.life <= 0.0f) continue;
            
            glColor4f(particle.r, particle.g, particle.b, particle.life);
            
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

void drawLaserShotsP2() {
    for (const auto& laser : laserShotsP2) {
        if (!laser.active) continue;
        
        float currentX = laser.startX + (laser.endX - laser.startX) * laser.currentProgress;
        float currentY = laser.startY + (laser.endY - laser.startY) * laser.currentProgress;
        
        // Linha principal
        glLineWidth(3.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(laser.startX, laser.startY);
        glVertex2f(currentX, currentY);
        glEnd();
        
        // Brilho
        glLineWidth(1.5f);
        glColor3f(0.5f, 1.0f, 0.5f);
        glBegin(GL_LINES);
        glVertex2f(laser.startX, laser.startY);
        glVertex2f(currentX, currentY);
        glEnd();
        
        // Ponto de impacto
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(8.0f);
        glBegin(GL_POINTS);
        glVertex2f(currentX, currentY);
        glEnd();
        
        glLineWidth(1.0f);
        glPointSize(1.0f);
    }
}

void drawShotsRemainingP2() {
    float x = windowWidthP2 - 120.0f;
    float y = windowHeightP2 - 40.0f;
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    
    std::string shotsText = "Tiros: " + std::to_string(shotsRemainingP2);
    for (char c : shotsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawGameTimerP2() {
    float timeRemaining = (float)(maxGameTimeP2 - gameTimerP2) / 60.0f;
    
    float x = windowWidthP2 / 2.0f - 50.0f;
    float y = windowHeightP2 - 40.0f;
    
    if (timeRemaining < 5.0f) {
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho quando pouco tempo
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    
    glRasterPos2f(x, y);
    
    std::ostringstream oss;
    oss.precision(1);
    oss << std::fixed << "Tempo: " << timeRemaining << "s";
    std::string timeText = oss.str();
    
    for (char c : timeText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawCorrectAnswersCountP2() {
    float x = 20.0f;
    float y = windowHeightP2 - 70.0f; // Abaixo da barra de vida
    
    glColor3f(0.0f, 1.0f, 0.0f); // Verde para acertos
    glRasterPos2f(x, y);
    
    std::string countText = "Acertos: " + std::to_string(correctAnswersCountP2) + "/" + std::to_string(correctAnswersTargetP2);
    for (char c : countText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void initPhase2() {
    srand(time(0));
    
    playerP2.x = windowWidthP2 * 0.1f;
    playerP2.y = windowHeightP2 / 2.0f;
    playerP2.width = 80.0f;
    playerP2.height = 60.0f;
    playerP2.health = 100;
    playerP2.maxHealth = 100;
    playerP2.isHit = false;
    playerP2.hitTimer = 0;
    playerP2.shakeOffsetX = 0.0f;
    playerP2.shakeOffsetY = 0.0f;
    
    asteroidsP2.clear();
    explosionsP2.clear();
    laserShotsP2.clear();
    
    shotsRemainingP2 = 5;
    gameTimerP2 = 0;
    spawnCooldownP2 = 0;
    asteroidsSpawnedP2 = 0;
    correctAnswersCountP2 = 0;
    allAsteroidsSpawnedP2 = false;
    
    // Iniciar contagem regressiva
    showCountdown = true;
    countdownTimer = 0;
    countdownValue = 3;
    
    createNewEquationP2();
    setGameOver(false);
    
    // Registrar callbacks do Game Over
    initGameOver(windowWidthP2, windowHeightP2);
    registerRestartCallback(restartPhase2);
    registerMenuCallback(returnToMenuFromPhase2);
    
    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawPhase2() {
    drawPlayerP2();
    drawAsteroidsP2();
    drawExplosionsP2();
    drawLaserShotsP2();
    drawHealthBarP2();
    drawEquationP2();
    drawShotsRemainingP2();
    drawGameTimerP2();
    drawCorrectAnswersCountP2();
    drawCrosshairP2();
    drawCountdown(); // Contagem regressiva
    drawPauseMenu(); // Menu de pausa
    
    drawGameOver();
}

void createNewEquationP2() {
    // Decidir operação (50% multiplicação, 50% divisão)
    bool isMultiplication = (rand() % 2) == 0;
    
    if (isMultiplication) {
        // Multiplicação: números de 2 a 12
        currentEquationP2.num1 = 2 + rand() % 11;
        currentEquationP2.num2 = 2 + rand() % 11;
        currentEquationP2.operation = '*';
        currentEquationP2.correctAnswer = currentEquationP2.num1 * currentEquationP2.num2;
    } else {
        // Divisão: garantir que seja exata
        currentEquationP2.num2 = 2 + rand() % 11; // Divisor de 2 a 12
        int quotient = 2 + rand() % 11; // Resultado de 2 a 12
        currentEquationP2.num1 = currentEquationP2.num2 * quotient; // Dividendo
        currentEquationP2.operation = '/';
        currentEquationP2.correctAnswer = quotient;
    }
    
    // Definir qual asteroide terá a resposta correta (0-9)
    correctAsteroidIndexP2 = rand() % 10;
    
    currentEquationP2.active = true;
}

void spawnAsteroidP2() {
    if (asteroidsSpawnedP2 >= 10) {
        allAsteroidsSpawnedP2 = true;
        return;
    }
    
    AsteroidPhase2 asteroid;
    asteroid.width = 40.0f;
    asteroid.height = 40.0f;
    
    // Posição inicial: topo da tela, lado direito
    asteroid.y = windowHeightP2 + asteroid.height;
    
    // Distribuir horizontalmente na lateral direita (70% a 95% da largura)
    float rightSideStart = windowWidthP2 * 0.70f;
    float rightSideWidth = windowWidthP2 * 0.25f;
    float spacing = rightSideWidth / 11.0f;
    asteroid.x = rightSideStart + spacing * (asteroidsSpawnedP2 + 1);
    
    asteroid.speed = 0.8f;
    asteroid.isDestroyed = false;
    
    // Verificar se este é o asteroide com a resposta correta
    if (asteroidsSpawnedP2 == correctAsteroidIndexP2) {
        asteroid.isCorrectAnswer = true;
        asteroid.displayNumber = currentEquationP2.correctAnswer;
    } else {
        asteroid.isCorrectAnswer = false;
        // Gerar número diferente da resposta correta
        do {
            asteroid.displayNumber = (rand() % 100) + 1;
        } while (asteroid.displayNumber == currentEquationP2.correctAnswer);
    }
    
    asteroidsP2.push_back(asteroid);
    asteroidsSpawnedP2++;
}

void updateExplosionsP2() {
    for (auto& explosion : explosionsP2) {
        if (!explosion.active) continue;
        
        for (auto& particle : explosion.particles) {
            particle.x += particle.vx;
            particle.y += particle.vy;
            particle.life -= 0.05f;
            particle.vx *= 0.95f;
            particle.vy *= 0.95f;
        }
        
        explosion.timer++;
        
        if (explosion.timer > 30) {
            explosion.active = false;
        }
    }
    
    explosionsP2.erase(
        std::remove_if(explosionsP2.begin(), explosionsP2.end(),
            [](const ExplosionPhase2& e) { return !e.active; }),
        explosionsP2.end()
    );
}

void updateLaserShotsP2() {
    for (auto& laser : laserShotsP2) {
        if (!laser.active) continue;
        
        laser.currentProgress += 0.15f;
        
        if (laser.currentProgress >= 1.0f) {
            laser.active = false;
        }
    }
    
    laserShotsP2.erase(
        std::remove_if(laserShotsP2.begin(), laserShotsP2.end(),
            [](const LaserShotPhase2& l) { return !l.active; }),
        laserShotsP2.end()
    );
}

void checkTimeoutDamageP2() {
    // Contar asteroides restantes não destruídos
    int remainingAsteroids = 0;
    for (const auto& asteroid : asteroidsP2) {
        if (!asteroid.isDestroyed) {
            remainingAsteroids++;
        }
    }
    
    if (remainingAsteroids > 0) {
        // Explodir todos os asteroides restantes
        for (auto& asteroid : asteroidsP2) {
            if (!asteroid.isDestroyed) {
                // Criar explosão
                ExplosionPhase2 explosion;
                explosion.x = asteroid.x + asteroid.width / 2.0f;
                explosion.y = asteroid.y + asteroid.height / 2.0f;
                explosion.timer = 0;
                explosion.active = true;
                
                int numParticles = 12;
                for (int i = 0; i < numParticles; i++) {
                    ParticlePhase2 p;
                    float angle = (float)i / (float)numParticles * 2.0f * M_PI;
                    float speed = 2.0f + (float)(rand() % 100) / 50.0f;
                    
                    p.x = explosion.x;
                    p.y = explosion.y;
                    p.vx = cos(angle) * speed;
                    p.vy = sin(angle) * speed;
                    p.life = 1.0f;
                    p.size = 3.0f + (float)(rand() % 100) / 50.0f;
                    
                    float colorVariation = (float)(rand() % 100) / 100.0f;
                    p.r = 1.0f;
                    p.g = 0.5f + colorVariation * 0.5f;
                    p.b = 0.0f;
                    
                    explosion.particles.push_back(p);
                }
                
                explosionsP2.push_back(explosion);
                asteroid.isDestroyed = true;
            }
        }
        
    // Aplicar dano (10 de dano por asteroide restante)
    int damage = remainingAsteroids * 10;
    playerP2.health -= damage;

    // tocar som de dano
    Audio::getInstance().play(Audio::SOUND_DAMAGE);
        
        if (playerP2.health <= 0) {
            playerP2.health = 0;
            setGameOver(true);
        } else {
            playerP2.isHit = true;
            playerP2.hitTimer = 30;
        }
    }
}

void updatePhase2() {
    if (getGameOver() || isPaused) {
        return;
    }
    
    // Atualizar contagem regressiva
    if (showCountdown) {
        countdownTimer++;
        if (countdownTimer >= 60) {
            countdownTimer = 0;
            countdownValue--;
            if (countdownValue <= 0) {
                showCountdown = false;
            }
        }
        return; // Não atualizar o jogo enquanto está contando
    }
    
    updateExplosionsP2();
    updateLaserShotsP2();
    
    // Atualizar efeito de dano - parar no game over
    if (!getGameOver() && playerP2.isHit && playerP2.hitTimer > 0) {
        playerP2.hitTimer--;
        
        float shakeIntensity = (float)playerP2.hitTimer / 30.0f * 8.0f;
        playerP2.shakeOffsetX = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
        playerP2.shakeOffsetY = ((rand() % 100) / 50.0f - 1.0f) * shakeIntensity;
        
        if (playerP2.hitTimer <= 0) {
            playerP2.isHit = false;
            playerP2.shakeOffsetX = 0.0f;
            playerP2.shakeOffsetY = 0.0f;
        }
    }
    
    // Mover asteroides (caindo de cima para baixo)
    for (auto& asteroid : asteroidsP2) {
        if (!asteroid.isDestroyed) {
            asteroid.y -= asteroid.speed;
        }
    }
    
    // Remover asteroides que saíram da tela pela parte de baixo
    asteroidsP2.erase(
        std::remove_if(asteroidsP2.begin(), asteroidsP2.end(),
            [](const AsteroidPhase2& a) { 
                return a.y + a.height < 0; 
            }),
        asteroidsP2.end()
    );
    
    // Spawnar asteroides com delay
    if (!allAsteroidsSpawnedP2) {
        if (spawnCooldownP2 <= 0) {
            spawnAsteroidP2();
            spawnCooldownP2 = 60; // 1 segundo entre spawns
        } else {
            spawnCooldownP2--;
        }
    }
    
    // Incrementar timer do jogo
    gameTimerP2++;
    
    // Verificar timeout
    if (gameTimerP2 >= maxGameTimeP2) {
        checkTimeoutDamageP2();
        
        // Reset para próxima rodada se não morreu
        if (!getGameOver()) {
            asteroidsP2.clear();
            asteroidsSpawnedP2 = 0;
            allAsteroidsSpawnedP2 = false;
            gameTimerP2 = 0;
            shotsRemainingP2 = 5;
            createNewEquationP2();
        }
    }
    
    gameFrameCounterP2++;
}

void handlePhase2MouseMove(int x, int y) {
    mouseXP2 = x;
    mouseYP2 = y;
    glutPostRedisplay();
}

void handlePhase2MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !getGameOver()) {
        if (shotsRemainingP2 <= 0) return;
        
        float clickX = (float)x;
        float clickY = windowHeightP2 - (float)y;
        
        // Verificar se clicou em algum asteroide
        bool hitAsteroid = false;
        for (size_t i = 0; i < asteroidsP2.size(); i++) {
            if (asteroidsP2[i].isDestroyed) continue;
            
            float ax = asteroidsP2[i].x;
            float ay = asteroidsP2[i].y;
            float aw = asteroidsP2[i].width;
            float ah = asteroidsP2[i].height;
            
            if (clickX >= ax && clickX <= ax + aw &&
                clickY >= ay && clickY <= ay + ah) {
                
                hitAsteroid = true;
                
                // Criar laser
                LaserShotPhase2 laser;
                laser.startX = windowWidthP2 * 0.1f;
                laser.startY = playerP2.y;
                laser.endX = ax + aw / 2.0f;
                laser.endY = ay + ah / 2.0f;
                laser.currentProgress = 0.0f;
                laser.active = true;
                laserShotsP2.push_back(laser);
                // tocar som de tiro
                Audio::getInstance().play(Audio::SOUND_LASER);
                
                // Criar explosão
                ExplosionPhase2 explosion;
                explosion.x = ax + aw / 2.0f;
                explosion.y = ay + ah / 2.0f;
                explosion.timer = 0;
                explosion.active = true;
                
                int numParticles = 12;
                for (int j = 0; j < numParticles; j++) {
                    ParticlePhase2 p;
                    float angle = (float)j / (float)numParticles * 2.0f * M_PI;
                    float speed = 2.0f + (float)(rand() % 100) / 50.0f;
                    
                    p.x = explosion.x;
                    p.y = explosion.y;
                    p.vx = cos(angle) * speed;
                    p.vy = sin(angle) * speed;
                    p.life = 1.0f;
                    p.size = 3.0f + (float)(rand() % 100) / 50.0f;
                    
                    float colorVariation = (float)(rand() % 100) / 100.0f;
                    p.r = 1.0f;
                    p.g = 0.5f + colorVariation * 0.5f;
                    p.b = 0.0f;
                    
                    explosion.particles.push_back(p);
                }
                
                explosionsP2.push_back(explosion);
                // tocar som de explosão
                Audio::getInstance().play(Audio::SOUND_EXPLOSION);
                
                // Decrementar tiros
                shotsRemainingP2--;
                
                // Verificar se acertou o correto
                if (asteroidsP2[i].isCorrectAnswer) {
                    // Acertou! Incrementar contador
                    correctAnswersCountP2++;
                    
                    // Verificar se completou os 10 acertos
                    if (correctAnswersCountP2 >= correctAnswersTargetP2) {
                        // Completou a Fase 2! Mostrar história da Fase 3
                        showStoryForPhase(3);
                        return;
                    } else {
                        // Resetar rodada para próxima pergunta
                        asteroidsP2.clear();
                        asteroidsSpawnedP2 = 0;
                        allAsteroidsSpawnedP2 = false;
                        gameTimerP2 = 0;
                        shotsRemainingP2 = 5;
                        createNewEquationP2();
                    }
                } else {
                    // Errou, apenas marca como destruído
                    asteroidsP2[i].isDestroyed = true;
                    
                    // Se os tiros acabaram, verificar se ainda há asteroides não destruídos
                    if (shotsRemainingP2 <= 0) {
                        int remainingAsteroids = 0;
                        for (const auto& asteroid : asteroidsP2) {
                            if (!asteroid.isDestroyed) {
                                remainingAsteroids++;
                            }
                        }
                        
                        if (remainingAsteroids > 0) {
                            // Explodir todos os asteroides restantes e causar dano
                            checkTimeoutDamageP2();
                            
                            // Reset para próxima rodada se não morreu
                            if (!getGameOver()) {
                                asteroidsP2.clear();
                                asteroidsSpawnedP2 = 0;
                                allAsteroidsSpawnedP2 = false;
                                gameTimerP2 = 0;
                                shotsRemainingP2 = 5;
                                createNewEquationP2();
                            }
                        }
                    }
                }
                
                break;
            }
        }
        
        // Se não acertou nenhum asteroide, ainda perde o tiro
        if (!hitAsteroid) {
            shotsRemainingP2--;
        }
        
        glutPostRedisplay();
    }
}

void handlePhase2Keyboard(unsigned char key) {
    // ESC para pausar/despausar
    if (key == 27) { // ESC
        if (!getGameOver()) {
            isPaused = !isPaused;
            pauseSelectedOption = 0;
            glutPostRedisplay();
        }
        return;
    }
    
    // Se está pausado, tratar navegação do menu
    if (isPaused) {
        if (key == 13 || key == '\r') { // Enter
            if (pauseSelectedOption == 0) {
                isPaused = false;
            } else if (pauseSelectedOption == 1) {
                isPaused = false;
                currentState = MAIN_MENU;
                glutSetCursor(GLUT_CURSOR_INHERIT);
                glutPassiveMotionFunc(handleMouseHover);
            }
            glutPostRedisplay();
        }
        return;
    }
}

void handlePhase2KeyboardUp(unsigned char key) {
    (void)key;
    // Pode adicionar controles de teclado se necessário
}

void handlePhase2SpecialKey(int key, int x, int y) {
    (void)x;
    (void)y;
    
    if (isPaused) {
        if (key == GLUT_KEY_UP) {
            pauseSelectedOption = 0;
            glutPostRedisplay();
        } else if (key == GLUT_KEY_DOWN) {
            pauseSelectedOption = 1;
            glutPostRedisplay();
        }
    }
}

void restartPhase2() {
    initPhase2();
}

void returnToMenuFromPhase2() {
    // Retornar ao menu
    glutSetCursor(GLUT_CURSOR_INHERIT);
    glutPassiveMotionFunc(handleMouseHover);
    currentState = MAIN_MENU;
}
