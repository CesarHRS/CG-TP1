#include <GL/glut.h>
#include "game.h"
#include "gameover.h"
#include <sstream>

Player player;
std::vector<Enemy> enemies;
int windowWidth_game = 800;  
int windowHeight_game = 600; 

bool isMovingLeft = false;
bool isMovingRight = false;
int spawnCooldown = 0; // Contador de frames entre spawns
int mouseX = 400; // Posição inicial do mouse (centro)
int mouseY = 300;
MathQuestion currentQuestion = {false, 0, 0, 0, "", -1, false, 0};

void drawPlayer() {
    // Nave vista de cima, apenas a parte frontal (15% inferior da tela)
    // Para-brisa ENORME ocupando ~70% da tela horizontalmente
    
    float baseY = windowHeight_game * 0.15f; // 15% da altura
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
    ss << currentQuestion.num1 << " + " << currentQuestion.num2 << " = " << currentQuestion.userAnswer;
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

    enemies.clear();
    spawnCooldown = 0; // Resetar cooldown para spawnar imediatamente
    setGameOver(false); // Resetar estado do game over
    
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
    drawHealthBar();
    drawMathQuestion(); // Desenhar questão matemática (se ativa)
    drawCrosshair(); // Desenhar mira customizada
    
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
}

void handleGameKeyboard(unsigned char key) {
    // Se há uma questão ativa, processar input numérico
    if (currentQuestion.active) {
        if (key >= '0' && key <= '9') {
            // Adicionar dígito à resposta (apenas se não estiver mostrando erro)
            if (!currentQuestion.showError) {
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
                // Resposta correta! Remover o asteroide
                if (currentQuestion.asteroidIndex >= 0 && 
                    currentQuestion.asteroidIndex < (int)enemies.size()) {
                    enemies.erase(enemies.begin() + currentQuestion.asteroidIndex);
                }
                
                // Limpar questão
                currentQuestion.active = false;
                currentQuestion.userAnswer = "";
                currentQuestion.asteroidIndex = -1;
                currentQuestion.showError = false;
                currentQuestion.errorTimer = 0;
                
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
            
            // Criar questão matemática
            currentQuestion.active = true;
            currentQuestion.num1 = (rand() % 20) + 1; // 1 a 20
            currentQuestion.num2 = (rand() % 20) + 1; // 1 a 20
            currentQuestion.correctAnswer = currentQuestion.num1 + currentQuestion.num2;
            currentQuestion.userAnswer = "";
            currentQuestion.asteroidIndex = i;
            
            // Marcar asteroide como tendo questão
            enemy.hasQuestion = true;
            enemy.questionNum1 = currentQuestion.num1;
            enemy.questionNum2 = currentQuestion.num2;
            
            break; // Só uma questão por vez
        }
    }
    
    glutPostRedisplay();
}
