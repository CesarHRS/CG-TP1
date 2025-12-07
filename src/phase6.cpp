#include <GL/glut.h>
#include "phase6.h"
#include "gameover.h"
#include "audio.h"
#include "menu.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===================================================================
// TEXTURAS GLOBAIS FASE 6
// ===================================================================

GLuint textureShipP6 = 0;
GLuint textureAsteroidP6 = 0;
GLuint textureFieldP6 = 0;
bool texturesLoadedP6 = false;

// ===================================================================
// FUNÇÕES DE TEXTURAS
// ===================================================================

GLuint createProceduralTextureP6(int width, int height, int type) {
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            switch(type) {
                case 0: // Nave (metal azulado)
                    {
                        int noise = rand() % 40;
                        data[index] = 100 + noise;
                        data[index + 1] = 140 + noise;
                        data[index + 2] = 200 + noise;
                    }
                    break;
                case 1: // Campo magnético (branco com variação sutil)
                    {
                        int noise = rand() % 30;
                        data[index] = 230 + noise;
                        data[index + 1] = 230 + noise;
                        data[index + 2] = 230 + noise;
                    }
                    break;
                case 2: // Asteroide (rocha cinza)
                    {
                        int noise = rand() % 50;
                        int base = 60 + ((x * y) % 40);
                        data[index] = base + noise;
                        data[index + 1] = base + noise;
                        data[index + 2] = base + noise + 10;
                    }
                    break;
            }
        }
    }
    
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    free(data);
    return texID;
}

void loadTexturesP6() {
    if (!texturesLoadedP6) {
        textureShipP6 = createProceduralTextureP6(128, 128, 0);
        textureFieldP6 = createProceduralTextureP6(128, 128, 1);
        textureAsteroidP6 = createProceduralTextureP6(128, 128, 2);
        texturesLoadedP6 = true;
        printf("Texturas procedurais da Fase 6 criadas\n");
    }
}

// ===================================================================
// ESTRUTURAS E VARIÁVEIS GLOBAIS
// ===================================================================

struct MagneticField {
    float x, y, z;        // Posição
    int answerValue;      // Valor mostrado no campo
    bool isCorrect;       // Se é a resposta certa
    bool active;
};

struct Question {
    int radius;
    int correctAnswer;
    std::string text;
};

// Player (nave)
float shipX = 0.0f;
float shipY = 0.0f;
float shipZ = -5.0f;
float shipSpeed = 0.3f;

// Movimento
bool keyStatesP6[256] = {false};
bool specialKeysP6[256] = {false};

// Game state
int livesP6 = 3;
int scoreP6 = 0;
int questionsAnsweredP6 = 0;
const int totalQuestionsP6 = 10;
bool gameOverP6 = false;
bool victoryP6 = false;

// Efeito visual de dano
int damageFlashP6 = 0;

// Campos magnéticos
std::vector<MagneticField> magneticFields;
Question currentQuestionP6;
float fieldSpawnZ = 80.0f;
float fieldSpeed = 0.15f;  // Velocidade média - equilibrada
int fieldSetCount = 0;
bool fieldSetPassed = false;  // Flag para detectar se passou pelo conjunto de campos

// Estrelas de fundo
struct Star {
    float x, y, z;
    float size;
};
std::vector<Star> stars;

// Timer para próxima pergunta
int questionCooldown = 0;

// Countdown variables
bool showCountdownP6 = false;
int countdownTimerP6 = 0;
int countdownValueP6 = 3;

// Histórico de raios já usados
std::vector<int> usedRadii;

// Declarações de funções
void restartPhase6();
void returnToMenuFromPhase6();

// ===================================================================
// FUNÇÕES AUXILIARES
// ===================================================================

void drawText3DP6(float x, float y, float z, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos3f(x, y, z);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

void generateQuestion() {
    // Raios entre 1 e 20, sem repetir
    int radius;
    int attempts = 0;
    do {
        radius = 1 + (rand() % 20);
        attempts++;
        // Se já tentou muito, limpa o histórico para permitir repetição
        if (attempts > 50) {
            usedRadii.clear();
            attempts = 0;
        }
    } while (std::find(usedRadii.begin(), usedRadii.end(), radius) != usedRadii.end());
    
    currentQuestionP6.radius = radius;
    usedRadii.push_back(radius);
    
    // Área = π * r² (usando π ≈ 3)
    currentQuestionP6.correctAnswer = 3 * currentQuestionP6.radius * currentQuestionP6.radius;
    
    char buffer[128];
    sprintf(buffer, "Campo com raio %d se aproxima! Qual sua area? (Use PI = 3)", currentQuestionP6.radius);
    currentQuestionP6.text = std::string(buffer);
    
    printf("Nova pergunta: raio=%d, resposta=%d\n", currentQuestionP6.radius, currentQuestionP6.correctAnswer);
}

void spawnFieldSet() {
    magneticFields.clear();
    
    // Gerar 3 campos: 1 correto e 2 incorretos
    std::vector<int> answers;
    answers.push_back(currentQuestionP6.correctAnswer);
    
    // Respostas incorretas próximas
    int wrongAnswer1 = currentQuestionP6.correctAnswer + (rand() % 10 - 5) * 3;
    int wrongAnswer2 = currentQuestionP6.correctAnswer + (rand() % 10 - 5) * 3;
    if (wrongAnswer1 <= 0) wrongAnswer1 = currentQuestionP6.correctAnswer + 10;
    if (wrongAnswer2 <= 0) wrongAnswer2 = currentQuestionP6.correctAnswer + 15;
    if (wrongAnswer1 == currentQuestionP6.correctAnswer) wrongAnswer1 += 6;
    if (wrongAnswer2 == currentQuestionP6.correctAnswer) wrongAnswer2 -= 6;
    
    answers.push_back(wrongAnswer1);
    answers.push_back(wrongAnswer2);
    
    // Embaralhar posições
    for (int i = 0; i < 3; i++) {
        MagneticField field;
        
        // Posições: linha reta horizontal com maior espaçamento
        float xPositions[] = {-25.0f, 0.0f, 25.0f};
        field.x = xPositions[i];
        field.y = 0.0f; // Mesma altura - linha reta
        field.z = fieldSpawnZ;
        
        // Embaralhar respostas
        int answerIndex = rand() % answers.size();
        field.answerValue = answers[answerIndex];
        field.isCorrect = (field.answerValue == currentQuestionP6.correctAnswer);
        field.active = true;
        
        answers.erase(answers.begin() + answerIndex);
        magneticFields.push_back(field);
    }
    
    fieldSetCount++;
}

void initStars() {
    stars.clear();
    for (int i = 0; i < 300; i++) {
        Star s;
        // Espalhar estrelas em área muito maior para cobrir toda a tela
        s.x = (rand() % 1000 - 500) * 1.5f;
        s.y = (rand() % 1000 - 500) * 1.5f;
        s.z = (rand() % 800);  // De Z=0 até Z=800
        s.size = 1.0f + (rand() % 4);
        stars.push_back(s);
    }
}

// ===================================================================
// FUNÇÕES DE DESENHO
// ===================================================================

void drawStarField() {
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    for (size_t i = 0; i < stars.size(); i++) {
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
        glEnd();
    }
}

void drawCockpitFrame() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Mudar para projeção ortogonal 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Moldura superior (5% da tela = 30 pixels)
    glColor3f(0.15f, 0.15f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 570);
    glVertex2f(800, 570);
    glVertex2f(800, 600);
    glVertex2f(0, 600);
    glEnd();
    
    // Moldura esquerda
    glBegin(GL_QUADS);
    glVertex2f(0, 180);
    glVertex2f(40, 180);
    glVertex2f(40, 570);
    glVertex2f(0, 570);
    glEnd();
    
    // Moldura direita
    glBegin(GL_QUADS);
    glVertex2f(760, 180);
    glVertex2f(800, 180);
    glVertex2f(800, 570);
    glVertex2f(760, 570);
    glEnd();
    
    // Detalhes metálicos
    glColor3f(0.3f, 0.3f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(40, 570); glVertex2f(760, 570);
    glVertex2f(40, 180); glVertex2f(40, 570);
    glVertex2f(760, 180); glVertex2f(760, 570);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
}

void drawControlPanel() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Painel inferior (30% = 180 pixels)
    glColor3f(0.1f, 0.1f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 180);
    glVertex2f(0, 180);
    glEnd();
    
    // Borda superior do painel
    glColor3f(0.3f, 0.3f, 0.35f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, 180);
    glVertex2f(800, 180);
    glEnd();
    
    // Painel da pergunta (centro-superior do painel)
    glColor3f(0.05f, 0.15f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(150, 120);
    glVertex2f(650, 120);
    glVertex2f(650, 170);
    glVertex2f(150, 170);
    glEnd();
    
    // Borda do painel da pergunta
    glColor3f(0.2f, 0.4f, 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(150, 120);
    glVertex2f(650, 120);
    glVertex2f(650, 170);
    glVertex2f(150, 170);
    glEnd();
    
    // Desenhar pergunta
    if (!victoryP6 && !gameOverP6) {
        glColor3f(0.3f, 0.8f, 1.0f);
        glRasterPos2f(160, 145);
        for (char c : currentQuestionP6.text) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
    
    // Volante 2D (centro do painel) que gira com movimento
    float steeringAngle = (shipX / 25.0f) * 45.0f; // Rotaciona até 45 graus
    float cx = 400.0f, cy = 60.0f, radius = 50.0f;
    
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glRotatef(steeringAngle, 0, 0, 1);
    glTranslatef(-cx, -cy, 0);
    
    // Aro do volante
    glColor3f(0.4f, 0.4f, 0.45f);
    glLineWidth(10.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
    
    // Centro do volante
    glColor3f(0.3f, 0.3f, 0.35f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(cx + cos(angle) * 15.0f, cy + sin(angle) * 15.0f);
    }
    glEnd();
    
    // Raios do volante
    glLineWidth(6.0f);
    glColor3f(0.45f, 0.45f, 0.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < 4; i++) {
        float angle = i * M_PI / 2.0f;
        glVertex2f(cx, cy);
        glVertex2f(cx + cos(angle) * radius * 0.85f, cy + sin(angle) * radius * 0.85f);
    }
    glEnd();
    
    glPopMatrix();
    
    // Botões decorativos (direita)
    float buttonY = 50.0f;
    for (int i = 0; i < 6; i++) {
        float bx = 680.0f + (i % 3) * 35.0f;
        float by = buttonY + (i / 3) * 40.0f;
        
        // Botão
        if (i % 2 == 0) {
            glColor3f(0.2f, 0.6f, 0.2f); // Verde
        } else {
            glColor3f(0.6f, 0.2f, 0.2f); // Vermelho
        }
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(bx, by);
        for (int j = 0; j <= 360; j += 30) {
            float angle = j * M_PI / 180.0f;
            glVertex2f(bx + cos(angle) * 12.0f, by + sin(angle) * 12.0f);
        }
        glEnd();
        
        // Borda
        glColor3f(0.3f, 0.3f, 0.3f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j <= 360; j += 30) {
            float angle = j * M_PI / 180.0f;
            glVertex2f(bx + cos(angle) * 12.0f, by + sin(angle) * 12.0f);
        }
        glEnd();
    }
    
    // Barra de vida (esquerda)
    glColor3f(0.2f, 0.2f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(20, 90);
    glVertex2f(220, 90);
    glVertex2f(220, 110);
    glVertex2f(20, 110);
    glEnd();
    
    // Borda da barra
    glColor3f(0.4f, 0.4f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, 90);
    glVertex2f(220, 90);
    glVertex2f(220, 110);
    glVertex2f(20, 110);
    glEnd();
    
    // Vida atual (verde para vermelho)
    float lifePercent = (float)livesP6 / 3.0f;
    float barWidth = 196.0f * lifePercent;
    
    if (lifePercent > 0.6f) {
        glColor3f(0.2f, 0.8f, 0.3f); // Verde
    } else if (lifePercent > 0.3f) {
        glColor3f(0.9f, 0.8f, 0.2f); // Amarelo
    } else {
        glColor3f(0.9f, 0.2f, 0.2f); // Vermelho
    }
    
    glBegin(GL_QUADS);
    glVertex2f(22, 92);
    glVertex2f(22 + barWidth, 92);
    glVertex2f(22 + barWidth, 108);
    glVertex2f(22, 108);
    glEnd();
    
    // Texto da vida
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[64];
    sprintf(buffer, "Vida: %d/3", livesP6);
    glRasterPos2f(90, 97);
    for (char* p = buffer; *p; p++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
    }
    
    // HUD Info - Contador de acertos
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buffer, "Campos: %d/%d", questionsAnsweredP6, totalQuestionsP6);
    glRasterPos2f(20, 70);
    for (char* p = buffer; *p; p++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }
    
    // Fórmula no canto direito superior
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* formula1 = "Formula:";
    const char* formula2 = "A = pi * r ^2";
    
    glRasterPos2f(690, 155);
    for (const char* p = formula1; *p; p++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
    }
    
    glRasterPos2f(680, 140);
    for (const char* p = formula2; *p; p++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
}

void drawMagneticField(const MagneticField& field) {
    glPushMatrix();
    glTranslatef(field.x, field.y, field.z);
    
    // Pulsação sutil
    float pulse = 0.95f + 0.05f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.003f);
    
    // Habilitar iluminação e textura para dar profundidade 3D
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    
    // Configurar luz para dar volume
    GLfloat lightPos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    GLfloat lightAmb[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat lightDif[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpec[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
    
    // Material brilhante
    GLfloat matAmb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat matDif[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat matSpec[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShin[] = {100.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShin);
    
    // Aplicar textura
    glBindTexture(GL_TEXTURE_2D, textureFieldP6);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Desenhar esfera com textura e iluminação
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, 5.0 * pulse, 32, 32);
    gluDeleteQuadric(quad);
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    
    glPopMatrix();
    
    // Desenhar número no centro (billboard)
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glTranslatef(field.x, field.y, field.z);
    
    // Billboard (sempre virado para câmera)
    glRotatef(180, 0, 1, 0);
    
    // Texto branco, tamanho ajustado para números grandes
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[16];
    sprintf(buffer, "%d", field.answerValue);
    
    // Usar fonte menor para números grandes
    void* font;
    if (field.answerValue > 999) {
        font = GLUT_BITMAP_HELVETICA_12;
    } else if (field.answerValue > 99) {
        font = GLUT_BITMAP_HELVETICA_18;
    } else {
        font = GLUT_BITMAP_TIMES_ROMAN_24;
    }
    
    int textWidth = glutBitmapLength(font, (unsigned char*)buffer);
    
    // Melhor centralização horizontal e vertical
    float xOffset = -textWidth * 0.006f;
    float yOffset = -0.5f;
    
    glRasterPos3f(xOffset, yOffset, 0.1f);
    
    for (char* p = buffer; *p; p++) {
        glutBitmapCharacter(font, *p);
    }
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ===================================================================
// FUNÇÕES PRINCIPAIS
// ===================================================================

void initPhase6() {
    printf("Inicializando Fase 6 (Cinturão de Asteroides Magnéticos)...\n");
    srand((unsigned int)time(NULL));
    
    // Carregar texturas
    loadTexturesP6();
    
    shipX = 0.0f;
    shipY = 0.0f;
    shipZ = -5.0f;
    
    // Reset global game over state FIRST
    setGameOver(false);
    setVictory(false);
    
    livesP6 = 3;
    scoreP6 = 0;
    questionsAnsweredP6 = 0;
    gameOverP6 = false;
    victoryP6 = false;
    fieldSetCount = 0;
    questionCooldown = 0;
    damageFlashP6 = 0;
    
    // Initialize countdown
    showCountdownP6 = true;
    countdownTimerP6 = 0;
    countdownValueP6 = 3;
    
    magneticFields.clear();
    usedRadii.clear();
    fieldSetPassed = false;
    initStars();
    
    // Resetar pergunta atual antes de gerar nova
    currentQuestionP6.radius = 0;
    currentQuestionP6.correctAnswer = 0;
    currentQuestionP6.text = "";
    
    generateQuestion();
    spawnFieldSet();
    initGameOver(800, 700);
    registerRestartCallback(restartPhase6);
    registerMenuCallback(returnToMenuFromPhase6);
}

void drawPhase6(int windowWidth, int windowHeight) {
    // Limpar tela primeiro com azul escuro
    if (damageFlashP6 > 0) {
        float intensity = (float)damageFlashP6 / 20.0f;
        glClearColor(0.3f * intensity, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(0.0f, 0.05f, 0.15f, 1.0f);  // Azul escuro
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Configurar perspectiva
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Câmera segue a nave (cockpit se move com WASD)
    gluLookAt(shipX, shipY, shipZ - 8.0f,  // Posição da câmera segue a nave
              shipX, shipY, shipZ + 50.0f,  // Olhando para frente na direção da nave
              0.0f, 1.0f, 0.0f);             // Up vector
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    
    // Desenhar estrelas de fundo (no mundo, atrás de tudo)
    drawStarField();
    
    // Desenhar campos magnéticos (sem iluminação)
    for (size_t i = 0; i < magneticFields.size(); i++) {
        if (magneticFields[i].active) {
            // Debug: verificar se isCorrect está consistente
            bool shouldBeCorrect = (magneticFields[i].answerValue == currentQuestionP6.correctAnswer);
            if (magneticFields[i].isCorrect != shouldBeCorrect) {
                printf("ERRO: Campo %zu tem isCorrect=%d mas deveria ser %d (valor=%d, resposta=%d)\n",
                       i, magneticFields[i].isCorrect, shouldBeCorrect, 
                       magneticFields[i].answerValue, currentQuestionP6.correctAnswer);
                // Corrigir em tempo real
                magneticFields[i].isCorrect = shouldBeCorrect;
            }
            drawMagneticField(magneticFields[i]);
        }
    }
    
    // Desenhar cockpit e painel
    drawCockpitFrame();
    drawControlPanel();
    
    // Configurar para overlay 2D
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    
    // Draw countdown if active
    if (showCountdownP6 && countdownValueP6 > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(windowWidth, 0);
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0, windowHeight);
        glEnd();
        glDisable(GL_BLEND);
        
        char countText[10];
        sprintf(countText, "%d", countdownValueP6);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(windowWidth / 2.0f - 20.0f, windowHeight / 2.0f);
        for (char* p = countText; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
        }
    }
    
    // Desenhar tela de pausa se ESC foi pressionado
    if (getPaused()) {
        drawPauseScreen();
        return;
    }
    
    // Draw game over screen if needed
    drawGameOver();
}

void updatePhase6(int value) {
    (void)value;
    
    if (getPaused()) {
        glutTimerFunc(16, updatePhase6, 0);
        return;
    }
    
    // Update countdown
    if (showCountdownP6) {
        countdownTimerP6++;
        if (countdownTimerP6 >= 60) {
            countdownTimerP6 = 0;
            countdownValueP6--;
            if (countdownValueP6 <= 0) {
                showCountdownP6 = false;
            }
        }
        glutPostRedisplay();
        glutTimerFunc(16, updatePhase6, 0);
        return;
    }
    
    if (gameOverP6 || victoryP6) {
        glutPostRedisplay();
        glutTimerFunc(16, updatePhase6, 0);
        return;
    }
    
    // Movimento da nave (apenas horizontal)
    if (keyStatesP6['a'] || keyStatesP6['A'] || specialKeysP6[GLUT_KEY_LEFT]) {
        shipX += shipSpeed;
        if (shipX > 25.0f) shipX = 25.0f;
    }
    if (keyStatesP6['d'] || keyStatesP6['D'] || specialKeysP6[GLUT_KEY_RIGHT]) {
        shipX -= shipSpeed;
        if (shipX < -25.0f) shipX = -25.0f;
    }
    
    // Mover campos magnéticos em direção à nave
    bool allFieldsPassed = true;
    bool anyFieldActive = false;
    
    for (size_t i = 0; i < magneticFields.size(); i++) {
        if (magneticFields[i].active) {
            anyFieldActive = true;
            magneticFields[i].z -= fieldSpeed;
            
            // Verificar colisão com nave
            float dx = magneticFields[i].x - shipX;
            float dy = magneticFields[i].y - shipY;
            float dz = magneticFields[i].z - shipZ;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            
            if (dist < 3.5f && magneticFields[i].z < shipZ + 2.0f && magneticFields[i].z > shipZ - 5.0f) {
                // Colidiu com o campo
                fieldSetPassed = true;
                if (magneticFields[i].isCorrect) {
                    Audio::getInstance().play(Audio::SOUND_VICTORY);
                    scoreP6 += 10;
                    questionsAnsweredP6++;
                } else {
                    Audio::getInstance().play(Audio::SOUND_ERROR);
                    livesP6--;
                    damageFlashP6 = 20;
                    
                    if (livesP6 <= 0) {
                        gameOverP6 = true;
                        setGameOver(true);
                        setVictory(false);
                    }
                }
                magneticFields[i].active = false;
            }
            
            // Se o campo passou completamente pela nave sem colisão (detecção imediata após passar)
            if (magneticFields[i].active && magneticFields[i].z < shipZ - 3.0f) {
                magneticFields[i].active = false;
            }
            
            if (magneticFields[i].z > shipZ) {
                allFieldsPassed = false;
            }
        }
    }
    
    // Se todos os campos passaram (não há mais campos ativos) e não colidiu com nenhum
    // Só penalizar se realmente passou pelos campos sem pegar nenhum
    if (!anyFieldActive && magneticFields.size() > 0 && !fieldSetPassed && allFieldsPassed) {
        fieldSetPassed = true;
        Audio::getInstance().play(Audio::SOUND_ERROR);
        livesP6--;
        damageFlashP6 = 20;
        
        if (livesP6 <= 0) {
            gameOverP6 = true;
            setGameOver(true);
            setVictory(false);
        }
    }
    
    // Mover estrelas (efeito parallax) - elas se movem para trás
    for (size_t i = 0; i < stars.size(); i++) {
        stars[i].z -= 0.3f;  // Movimento lento para trás
        
        // Quando uma estrela passa muito para trás, reaparece na frente
        if (stars[i].z < -50.0f) {
            stars[i].z = 800.0f + (rand() % 200);
            stars[i].x = (rand() % 1000 - 500) * 1.5f;
            stars[i].y = (rand() % 1000 - 500) * 1.5f;
        }
    }
    
    // Gerar novo conjunto de campos SOMENTE se passou por algum campo ou já aplicou o dano
    if (allFieldsPassed && questionCooldown <= 0 && !gameOverP6 && !victoryP6 && fieldSetPassed) {
        if (questionsAnsweredP6 >= totalQuestionsP6) {
            printf("Fase 6 completa! Mostrando história da Fase 7...\n");
            Audio::getInstance().play(Audio::SOUND_VICTORY);
            showStoryForPhase(7);
            return; // Sair imediatamente
        } else {
            generateQuestion();
            spawnFieldSet();
            fieldSetPassed = false;  // Resetar flag para o novo conjunto
            questionCooldown = 60; // Cooldown de 1 segundo
        }
    }
    
    if (questionCooldown > 0) {
        questionCooldown--;
    }
    
    // Reduzir efeito de flash de dano
    if (damageFlashP6 > 0) {
        damageFlashP6--;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, updatePhase6, 0);
}

void handlePhase6Keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    
    // ESC para pausar
    if (key == 27) {
        if (!gameOverP6 && !getPaused()) {
            setPaused(true, 6);
            glutPostRedisplay();
        } else if (getPaused()) {
            handlePauseKeyboard(key);
            glutPostRedisplay();
        }
        return;
    }
    
    // Se está pausado, tratar teclas de pausa
    if (getPaused()) {
        handlePauseKeyboard(key);
        glutPostRedisplay();
        return;
    }
    
    if (gameOverP6) {
        handleGameOverKeyboard(key);
        return;
    }
    
    keyStatesP6[key] = true;
}

void handlePhase6KeyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    keyStatesP6[key] = false;
}

void handlePhase6Special(int key, int x, int y) {
    (void)x; (void)y;
    specialKeysP6[key] = true;
}

void handlePhase6SpecialUp(int key, int x, int y) {
    (void)x; (void)y;
    specialKeysP6[key] = false;
}

void restartPhase6() {
    initPhase6();
}

void returnToMenuFromPhase6() {
    // Resetar estados de jogo primeiro
    gameOverP6 = false;
    victoryP6 = false;
    setGameOver(false);
    setVictory(false);
    
    // Resetar variáveis da fase
    damageFlashP6 = 0;
    livesP6 = 3;
    scoreP6 = 0;
    questionsAnsweredP6 = 0;
    magneticFields.clear();
    usedRadii.clear();
    fieldSetPassed = false;
    showCountdownP6 = false;
    
    // Resetar estado de pausa
    setPaused(false, 0);
    
    // Restaurar cor de fundo do menu
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    // Voltar ao menu
    setCurrentPhase(0);
    glutPostRedisplay();
}
