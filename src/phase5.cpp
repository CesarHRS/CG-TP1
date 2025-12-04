#include "phase5.h"
#include "audio.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// ===================================================================
// Global variables for Phase 5
// ===================================================================

Player3D playerP5;
Collectible objectsP5[10];
DepositZone depositsP5[4];
int numObjectsP5;
int numDepositsP5;
int scoreP5;
bool phase5_won = false;
int heldObjectIndex = -1; // Índice do objeto sendo segurado (-1 = nenhum)

// Keyboard state for smooth movement
bool keyStateP5[256];

// Mouse look variables
int lastMouseXP5;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// ===================================================================
// Utility Functions
// ===================================================================

void drawPyramidP5() {
    glBegin(GL_TRIANGLES);
        // Face 1
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 0.0f, 1.f, 0.0f);
        glVertex3f(-1.f, -1.f, 1.f);
        glVertex3f(1.f, -1.f, 1.f);
        // Face 2
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.f, 0.0f);
        glVertex3f(1.f, -1.f, 1.f);
        glVertex3f(1.f, -1.f, -1.f);
        // Face 3
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 1.f, 0.0f);
        glVertex3f(1.f, -1.f, -1.f);
        glVertex3f(-1.f, -1.f, -1.f);
        // Face 4
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f( 0.0f, 1.f, 0.0f);
        glVertex3f(-1.f,-1.f,-1.f);
        glVertex3f(-1.f,-1.f, 1.f);
    glEnd();
}

void drawCylinderP5(float radius, float height, int sides) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * i / sides;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glNormal3f(x/radius, 0.0, z/radius);
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * i / sides;
        glVertex3f(radius * cos(angle), height, radius * sin(angle));
    }
    glEnd();
}

void drawTextP5(float x, float y, const char *string) {
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Desenhar texto 3D no mundo
void drawText3DP5(float x, float y, float z, const char *string, void* font) {
    glRasterPos3f(x, y, z);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Desenhar árvore 3D
void drawTreeP5(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale, scale);
    
    // Tronco (cilindro vertical)
    glColor3f(0.4f, 0.25f, 0.1f); // Marrom
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    drawCylinderP5(0.2f, 2.0f, 10);
    glPopMatrix();
    
    // Copa da árvore (esfera verde)
    glColor3f(0.1f, 0.6f, 0.1f); // Verde escuro
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, 0.0f);
    glutSolidSphere(1.0f, 12, 12);
    glPopMatrix();
    
    glColor3f(0.2f, 0.7f, 0.2f); // Verde mais claro
    glPushMatrix();
    glTranslatef(0.0f, 3.2f, 0.0f);
    glutSolidSphere(0.7f, 12, 12);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar arbusto
void drawBushP5(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale, scale);
    
    // Arbusto (esferas verdes)
    glColor3f(0.2f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glutSolidSphere(0.4f, 10, 10);
    glPopMatrix();
    
    glColor3f(0.15f, 0.55f, 0.15f);
    glPushMatrix();
    glTranslatef(0.2f, 0.25f, 0.1f);
    glutSolidSphere(0.35f, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.15f, 0.2f, -0.1f);
    glutSolidSphere(0.3f, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar flores
void drawFlowerP5(float x, float z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    
    // Caule vertical
    glColor3f(0.1f, 0.5f, 0.1f);
    glPushMatrix();
    drawCylinderP5(0.02f, 0.3f, 6);
    glPopMatrix();
    
    // Pétalas (pequenos círculos)
    glColor3f(r, g, b);
    for (int i = 0; i < 5; i++) {
        float angle = i * 2.0f * M_PI / 5.0f;
        glPushMatrix();
        glTranslatef(cos(angle) * 0.08f, 0.3f, sin(angle) * 0.08f);
        glutSolidSphere(0.05f, 8, 8);
        glPopMatrix();
    }
    
    // Centro da flor
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glutSolidSphere(0.04f, 8, 8);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar rocha
void drawRockP5(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale * 0.6f, scale * 0.8f);
    
    // Rocha (esfera achatada cinza)
    glColor3f(0.5f, 0.5f, 0.52f);
    glutSolidSphere(0.5f, 8, 6);
    glPopMatrix();
}

// Desenhar painel central (base quadrada simples)
void drawCentralPanelP5() {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Centro do cenário
    
    // Base quadrada cinza elevada
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(1.5f, 1.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar mãos do traje espacial (primeira pessoa)
void drawSpaceGlovesP5() {
    glDisable(GL_LIGHTING);
    
    // Mão esquerda
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f); // Branco/cinza claro do traje
    glTranslatef(-0.4f, -0.5f, -1.0f);
    glRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
    
    // Palma
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.15f, 0.0f, 0.0f);
    glVertex3f(0.15f, 0.25f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glEnd();
    
    // Dedos (simplificados)
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glVertex3f(0.04f, 0.25f, 0.0f);
    glVertex3f(0.04f, 0.35f, 0.0f);
    glVertex3f(0.0f, 0.35f, 0.0f);
    
    glVertex3f(0.055f, 0.25f, 0.0f);
    glVertex3f(0.095f, 0.25f, 0.0f);
    glVertex3f(0.095f, 0.35f, 0.0f);
    glVertex3f(0.055f, 0.35f, 0.0f);
    
    glVertex3f(0.11f, 0.25f, 0.0f);
    glVertex3f(0.15f, 0.25f, 0.0f);
    glVertex3f(0.15f, 0.33f, 0.0f);
    glVertex3f(0.11f, 0.33f, 0.0f);
    glEnd();
    
    // Detalhes do traje (azul)
    glColor3f(0.2f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, -0.05f, 0.0f);
    glVertex3f(0.15f, -0.05f, 0.0f);
    glVertex3f(0.15f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    
    // Mão direita
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glTranslatef(0.4f, -0.5f, -1.0f);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    
    // Palma
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.15f, 0.0f, 0.0f);
    glVertex3f(-0.15f, 0.25f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glEnd();
    
    // Dedos
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glVertex3f(-0.04f, 0.25f, 0.0f);
    glVertex3f(-0.04f, 0.35f, 0.0f);
    glVertex3f(0.0f, 0.35f, 0.0f);
    
    glVertex3f(-0.055f, 0.25f, 0.0f);
    glVertex3f(-0.095f, 0.25f, 0.0f);
    glVertex3f(-0.095f, 0.35f, 0.0f);
    glVertex3f(-0.055f, 0.35f, 0.0f);
    
    glVertex3f(-0.11f, 0.25f, 0.0f);
    glVertex3f(-0.15f, 0.25f, 0.0f);
    glVertex3f(-0.15f, 0.33f, 0.0f);
    glVertex3f(-0.11f, 0.33f, 0.0f);
    glEnd();
    
    // Detalhes do traje
    glColor3f(0.2f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, -0.05f, 0.0f);
    glVertex3f(-0.15f, -0.05f, 0.0f);
    glVertex3f(-0.15f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}


// ===================================================================
// Core Phase 5 Functions
// ===================================================================

void initPhase5() {
    printf("Initializing Phase 5 (3D Geometric Planet)...\n");
    srand((unsigned int)time(NULL));

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat light_pos[] = {2.0, 8.0, 2.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_DEPTH_TEST);

    playerP5.x = 0.0f;
    playerP5.y = 0.6f; // Altura dos olhos do astronauta
    playerP5.z = 8.0f;
    playerP5.angle = 0.0f;
    playerP5.pitch = 0.0f;
    
    lastMouseXP5 = -1;
    for(int i = 0; i < 256; i++) keyStateP5[i] = false;

    // Objetos menores em posições aleatórias evitando obstáculos
    numObjectsP5 = 4;
    
    // Gerar posições aleatórias evitando centro, rochas e vegetação
    float posX[4], posZ[4];
    for (int i = 0; i < 4; i++) {
        bool validPos = false;
        int attempts = 0;
        do {
            posX[i] = ((rand() % 180) - 90) / 10.0f; // -9 a 9
            posZ[i] = ((rand() % 180) - 90) / 10.0f;
            
            // Verifica se não está no centro (torre)
            if (fabs(posX[i]) < 4.5f && fabs(posZ[i]) < 4.5f) continue;
            
            // Verifica se não está nas paredes de rochas principais
            if ((fabs(posX[i] - 4.0f) < 2.0f || fabs(posX[i] + 4.0f) < 2.0f) && 
                (posZ[i] < -5.5f || posZ[i] > 5.5f)) continue;
            if ((fabs(posZ[i] - 4.0f) < 2.0f || fabs(posZ[i] + 4.0f) < 2.0f) && 
                (posX[i] < -5.5f || posX[i] > 5.5f)) continue;
            
            // Verifica se não está nas paredes internas
            if ((fabs(posX[i] - 8.0f) < 1.5f || fabs(posX[i] + 8.0f) < 1.5f) && 
                fabs(posZ[i]) > 6.5f) continue;
            if ((fabs(posZ[i] - 8.0f) < 1.5f || fabs(posZ[i] + 8.0f) < 1.5f) && 
                fabs(posX[i]) > 6.5f) continue;
            
            validPos = true;
            attempts++;
        } while (!validPos && attempts < 100);
    }
    
    objectsP5[0] = (Collectible){posX[0], 0.2f, posZ[0], SHAPE_CUBE, false, false};
    objectsP5[1] = (Collectible){posX[1], 0.2f, posZ[1], SHAPE_SPHERE, false, false};
    objectsP5[2] = (Collectible){posX[2], 0.2f, posZ[2], SHAPE_PYRAMID, false, false};
    objectsP5[3] = (Collectible){posX[3], 0.2f, posZ[3], SHAPE_CYLINDER, false, false};
    
    // Zonas de depósito ao redor do painel central
    numDepositsP5 = 4;
    depositsP5[0] = (DepositZone){-2.5f, 0.0f, 0.0f, SHAPE_CUBE, false}; // Esquerda
    depositsP5[1] = (DepositZone){2.5f, 0.0f, 0.0f, SHAPE_SPHERE, false}; // Direita
    depositsP5[2] = (DepositZone){0.0f, 0.0f, -2.5f, SHAPE_PYRAMID, false}; // Frente
    depositsP5[3] = (DepositZone){0.0f, 0.0f, 2.5f, SHAPE_CYLINDER, false}; // Trás
    
    scoreP5 = 0;
    phase5_won = false;
    heldObjectIndex = -1;
}

void drawPhase5(int windowWidth, int windowHeight) {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Céu azul claro
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / (double)windowHeight, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float lookX = playerP5.x + sin(playerP5.angle);
    float lookZ = playerP5.z - cos(playerP5.angle);
    gluLookAt(playerP5.x, playerP5.y, playerP5.z, lookX, playerP5.y, lookZ, 0.0f, 1.0f, 0.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // Chão de terra e grama
    glBegin(GL_QUADS);
    for (int x = -15; x < 15; x++) {
        for (int z = -15; z < 15; z++) {
            // Alterna entre tons de verde e marrom para parecer natural
            if ((x + z) % 3 == 0) {
                glColor3f(0.34f, 0.52f, 0.19f); // Verde escuro
            } else if ((x + z) % 3 == 1) {
                glColor3f(0.42f, 0.61f, 0.24f); // Verde médio
            } else {
                glColor3f(0.55f, 0.42f, 0.23f); // Marrom (terra)
            }
            
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f((float)x, 0.0f, (float)z);
            glVertex3f((float)x, 0.0f, (float)(z + 1));
            glVertex3f((float)(x + 1), 0.0f, (float)(z + 1));
            glVertex3f((float)(x + 1), 0.0f, (float)z);
        }
    }
    glEnd();
    
    // Desenhar árvores espalhadas
    drawTreeP5(-10.0f, 8.0f, 1.0f);
    drawTreeP5(-8.0f, -6.0f, 0.9f);
    drawTreeP5(9.0f, -9.0f, 1.1f);
    drawTreeP5(11.0f, 7.0f, 0.95f);
    drawTreeP5(-6.0f, 10.0f, 1.05f);
    drawTreeP5(6.0f, -12.0f, 0.85f);
    drawTreeP5(-12.0f, -10.0f, 1.0f);
    drawTreeP5(8.0f, 10.0f, 0.9f);
    
    // Desenhar arbustos
    drawBushP5(-7.0f, 3.0f, 1.0f);
    drawBushP5(8.5f, -5.0f, 0.9f);
    drawBushP5(-4.0f, -8.0f, 1.1f);
    drawBushP5(10.0f, 9.0f, 0.95f);
    drawBushP5(-9.0f, -3.0f, 1.05f);
    drawBushP5(5.0f, 6.0f, 0.85f);
    drawBushP5(-11.0f, 5.0f, 1.0f);
    drawBushP5(4.0f, -10.0f, 0.9f);
    drawBushP5(-3.0f, 11.0f, 1.0f);
    drawBushP5(11.0f, -7.0f, 0.95f);
    
    // Desenhar flores coloridas
    drawFlowerP5(-5.5f, -9.5f, 1.0f, 0.2f, 0.8f); // Roxas perto da pirâmide
    drawFlowerP5(-4.8f, -8.8f, 0.9f, 0.2f, 0.9f);
    drawFlowerP5(-5.2f, -9.0f, 1.0f, 0.3f, 0.7f);
    
    drawFlowerP5(7.5f, -7.5f, 1.0f, 1.0f, 0.3f); // Amarelas
    drawFlowerP5(9.5f, -6.8f, 0.8f, 1.0f, 0.5f);
    drawFlowerP5(8.8f, -7.2f, 1.0f, 0.9f, 0.4f);
    
    drawFlowerP5(-7.5f, 5.5f, 1.0f, 1.0f, 0.1f); // Vermelhas
    drawFlowerP5(-8.2f, 6.2f, 0.9f, 0.9f, 0.2f);
    drawFlowerP5(-8.5f, 5.8f, 1.0f, 1.0f, 0.15f);
    
    drawFlowerP5(6.5f, 7.8f, 0.9f, 1.0f, 0.8f); // Rosas
    drawFlowerP5(7.2f, 8.2f, 1.0f, 1.0f, 0.6f);
    drawFlowerP5(7.5f, 7.5f, 0.95f, 0.9f, 0.7f);
    
    // Desenhar labirinto de rochas
    // Paredes horizontais
    for (float x = -12.0f; x <= -6.0f; x += 1.2f) {
        drawRockP5(x, -4.0f, 1.5f);
        drawRockP5(x, 4.0f, 1.4f);
    }
    for (float x = 6.0f; x <= 12.0f; x += 1.2f) {
        drawRockP5(x, -4.0f, 1.5f);
        drawRockP5(x, 4.0f, 1.4f);
    }
    
    // Paredes verticais
    for (float z = -12.0f; z <= -6.0f; z += 1.2f) {
        drawRockP5(-4.0f, z, 1.4f);
        drawRockP5(4.0f, z, 1.5f);
    }
    for (float z = 6.0f; z <= 12.0f; z += 1.2f) {
        drawRockP5(-4.0f, z, 1.4f);
        drawRockP5(4.0f, z, 1.5f);
    }
    
    // Paredes internas formando caminhos
    for (float x = -10.0f; x <= -7.0f; x += 1.2f) {
        drawRockP5(x, -8.0f, 1.3f);
        drawRockP5(x, 8.0f, 1.3f);
    }
    for (float x = 7.0f; x <= 10.0f; x += 1.2f) {
        drawRockP5(x, -8.0f, 1.3f);
        drawRockP5(x, 8.0f, 1.3f);
    }
    
    for (float z = -10.0f; z <= -7.0f; z += 1.2f) {
        drawRockP5(-8.0f, z, 1.3f);
        drawRockP5(8.0f, z, 1.3f);
    }
    for (float z = 7.0f; z <= 10.0f; z += 1.2f) {
        drawRockP5(-8.0f, z, 1.3f);
        drawRockP5(8.0f, z, 1.3f);
    }
    
    // Algumas rochas decorativas no meio
    drawRockP5(-6.5f, -6.5f, 1.2f);
    drawRockP5(6.5f, -6.5f, 1.2f);
    drawRockP5(-6.5f, 6.5f, 1.2f);
    drawRockP5(6.5f, 6.5f, 1.2f);

    // Desenhar painel central (torre com livro)
    drawCentralPanelP5();
    
    // Desenhar zonas de depósito (plataformas simples ao redor da torre)
    glDisable(GL_LIGHTING);
    for (int i = 0; i < numDepositsP5; i++) {
        glPushMatrix();
        glTranslatef(depositsP5[i].x, 0.01f, depositsP5[i].z);
        
        if (depositsP5[i].filled) {
            glColor3f(0.2f, 0.8f, 0.2f); // Verde quando preenchida
        } else {
            // Cor baseada no tipo requerido
            switch(depositsP5[i].requiredType) {
                case SHAPE_CUBE: glColor3f(0.8f, 0.2f, 0.2f); break;
                case SHAPE_SPHERE: glColor3f(0.2f, 0.8f, 0.2f); break;
                case SHAPE_PYRAMID: glColor3f(0.2f, 0.2f, 0.8f); break;
                case SHAPE_CYLINDER: glColor3f(0.8f, 0.8f, 0.2f); break;
            }
        }
        
        // Plataforma circular no chão
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f);
        for (int j = 0; j <= 20; j++) {
            float angle = j * 2.0f * M_PI / 20.0f;
            glVertex3f(cos(angle) * 0.8f, 0.0f, sin(angle) * 0.8f);
        }
        glEnd();
        
        // Texto indicando qual forma colocar (só se não estiver preenchida)
        if (!depositsP5[i].filled) {
            glColor3f(1.0f, 1.0f, 1.0f);
            const char* labelText = "";
            switch(depositsP5[i].requiredType) {
                case SHAPE_CUBE: labelText = "CUBO"; break;
                case SHAPE_SPHERE: labelText = "ESFERA"; break;
                case SHAPE_PYRAMID: labelText = "PIRAMIDE"; break;
                case SHAPE_CYLINDER: labelText = "CILINDRO"; break;
            }
            drawText3DP5(-0.3f, 0.05f, 0.0f, labelText, GLUT_BITMAP_HELVETICA_12);
        }
        
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    
    // Desenhar objetos depositados nas plataformas
    for (int i = 0; i < numObjectsP5; ++i) {
        if (objectsP5[i].collected && !objectsP5[i].held) {
            glPushMatrix();
            glTranslatef(objectsP5[i].x, 0.13f, objectsP5[i].z); // Altura fixa no chão
            glScalef(0.25f, 0.25f, 0.25f);
            
            switch (objectsP5[i].type) {
                case SHAPE_CUBE: glColor3f(1.0, 0.2, 0.2); glutSolidCube(1.0); break;
                case SHAPE_SPHERE: glColor3f(0.2, 1.0, 0.2); glutSolidSphere(0.6, 20, 20); break;
                case SHAPE_PYRAMID: glColor3f(0.2, 0.8, 1.0); drawPyramidP5(); break;
                case SHAPE_CYLINDER: 
                    glColor3f(1.0, 1.0, 0.2); 
                    glTranslatef(0.0f, -0.5f, 0.0f);
                    drawCylinderP5(0.5, 1.0, 20); 
                    break;
            }
            glPopMatrix();
        }
    }

    // Desenhar objetos coletáveis (menores, parcialmente escondidos)
    for (int i = 0; i < numObjectsP5; ++i) {
        if (!objectsP5[i].collected && !objectsP5[i].held) {
            glPushMatrix();
            glTranslatef(objectsP5[i].x, 0.13f, objectsP5[i].z); // Altura fixa no chão
            
            glScalef(0.25f, 0.25f, 0.25f); // Objetos bem menores
            
            switch (objectsP5[i].type) {
                case SHAPE_CUBE: glColor3f(1.0, 0.2, 0.2); glutSolidCube(1.0); break;
                case SHAPE_SPHERE: glColor3f(0.2, 1.0, 0.2); glutSolidSphere(0.6, 20, 20); break;
                case SHAPE_PYRAMID: glColor3f(0.2, 0.2, 1.0); drawPyramidP5(); break;
                case SHAPE_CYLINDER: 
                    glColor3f(1.0, 1.0, 0.2); 
                    glTranslatef(0.0f, -0.5f, 0.0f);
                    drawCylinderP5(0.5, 1.0, 20); 
                    break;
            }
            glPopMatrix();
        }
    }
    
    // Se estiver segurando um objeto, desenhar na frente
    if (heldObjectIndex >= 0) {
        glPushMatrix();
        float holdX = playerP5.x + sin(playerP5.angle) * 0.8f;
        float holdZ = playerP5.z - cos(playerP5.angle) * 0.8f;
        glTranslatef(holdX, playerP5.y - 0.1f, holdZ);
        glScalef(0.25f, 0.25f, 0.25f);
        
        switch (objectsP5[heldObjectIndex].type) {
            case SHAPE_CUBE: glColor3f(1.0, 0.2, 0.2); glutSolidCube(1.0); break;
            case SHAPE_SPHERE: glColor3f(0.2, 1.0, 0.2); glutSolidSphere(0.6, 20, 20); break;
            case SHAPE_PYRAMID: glColor3f(0.2, 0.8, 1.0); drawPyramidP5(); break;
            case SHAPE_CYLINDER: 
                glColor3f(1.0, 1.0, 0.2); 
                glTranslatef(0.0f, -0.5f, 0.0f);
                drawCylinderP5(0.5, 1.0, 20); 
                break;
        }
        glPopMatrix();
    }

    // Desenhar mãos do traje espacial (em primeiro plano, sobre tudo)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / (double)windowHeight, 0.01, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawSpaceGlovesP5();
    
    // Voltar para HUD 2D
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    char text[100];
    glColor3f(0.0, 1.0, 1.0);

    sprintf(text, "Objetos depositados: %d/4", scoreP5);
    drawTextP5(10, windowHeight - 20, text);

    if (phase5_won) {
        glColor3f(0.2, 1.0, 0.2);
        drawTextP5(10, windowHeight - 45, "MISSAO COMPLETA! Pressione ESC para sair.");
    } else if (heldObjectIndex >= 0) {
        glColor3f(1.0, 1.0, 0.3);
        const char* heldName = "";
        switch(objectsP5[heldObjectIndex].type) {
            case SHAPE_CUBE: heldName = "Segurando: CUBO - Leve ao pedestal VERMELHO"; break;
            case SHAPE_SPHERE: heldName = "Segurando: ESFERA - Leve ao pedestal VERDE"; break;
            case SHAPE_PYRAMID: heldName = "Segurando: PIRAMIDE - Leve ao pedestal AZUL"; break;
            case SHAPE_CYLINDER: heldName = "Segurando: CILINDRO - Leve ao pedestal AMARELO"; break;
        }
        drawTextP5(10, windowHeight - 45, heldName);
    } else {
        glColor3f(1.0, 1.0, 1.0);
        drawTextP5(10, windowHeight - 45, "E para pegar | Procure formas escondidas | Leve ao painel central");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Verificar colisão com obstáculos
bool checkCollisionP5(float x, float z) {
    // Colisão com plataforma central (quadrado 1.5x1.5)
    if (fabs(x) < 1.2f && fabs(z) < 1.2f) return true;
    
    // Colisão com árvores (posições fixas)
    float trees[][2] = {
        {-10.0f, 8.0f}, {-8.0f, -6.0f}, {9.0f, -9.0f}, {11.0f, 7.0f},
        {-6.0f, 10.0f}, {6.0f, -12.0f}, {-12.0f, -10.0f}, {8.0f, 10.0f}
    };
    for (int i = 0; i < 8; i++) {
        float dx = x - trees[i][0];
        float dz = z - trees[i][1];
        if (sqrt(dx*dx + dz*dz) < 0.8f) return true;
    }
    
    // Colisão com arbustos
    float bushes[][2] = {
        {-7.0f, 3.0f}, {8.5f, -5.0f}, {-4.0f, -8.0f}, {10.0f, 9.0f},
        {-9.0f, -3.0f}, {5.0f, 6.0f}, {-11.0f, 5.0f}, {4.0f, -10.0f},
        {-3.0f, 11.0f}, {11.0f, -7.0f}
    };
    for (int i = 0; i < 10; i++) {
        float dx = x - bushes[i][0];
        float dz = z - bushes[i][1];
        if (sqrt(dx*dx + dz*dz) < 0.6f) return true;
    }
    
    // Colisão com paredes de rochas horizontais
    for (float rx = -12.0f; rx <= -6.0f; rx += 1.2f) {
        if (sqrt(pow(x - rx, 2) + pow(z + 4.0f, 2)) < 0.9f) return true;
        if (sqrt(pow(x - rx, 2) + pow(z - 4.0f, 2)) < 0.9f) return true;
    }
    for (float rx = 6.0f; rx <= 12.0f; rx += 1.2f) {
        if (sqrt(pow(x - rx, 2) + pow(z + 4.0f, 2)) < 0.9f) return true;
        if (sqrt(pow(x - rx, 2) + pow(z - 4.0f, 2)) < 0.9f) return true;
    }
    
    // Colisão com paredes de rochas verticais
    for (float rz = -12.0f; rz <= -6.0f; rz += 1.2f) {
        if (sqrt(pow(x + 4.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
        if (sqrt(pow(x - 4.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
    }
    for (float rz = 6.0f; rz <= 12.0f; rz += 1.2f) {
        if (sqrt(pow(x + 4.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
        if (sqrt(pow(x - 4.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
    }
    
    // Colisão com paredes internas
    for (float rx = -10.0f; rx <= -7.0f; rx += 1.2f) {
        if (sqrt(pow(x - rx, 2) + pow(z + 8.0f, 2)) < 0.9f) return true;
        if (sqrt(pow(x - rx, 2) + pow(z - 8.0f, 2)) < 0.9f) return true;
    }
    for (float rx = 7.0f; rx <= 10.0f; rx += 1.2f) {
        if (sqrt(pow(x - rx, 2) + pow(z + 8.0f, 2)) < 0.9f) return true;
        if (sqrt(pow(x - rx, 2) + pow(z - 8.0f, 2)) < 0.9f) return true;
    }
    for (float rz = -10.0f; rz <= -7.0f; rz += 1.2f) {
        if (sqrt(pow(x + 8.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
        if (sqrt(pow(x - 8.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
    }
    for (float rz = 7.0f; rz <= 10.0f; rz += 1.2f) {
        if (sqrt(pow(x + 8.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
        if (sqrt(pow(x - 8.0f, 2) + pow(z - rz, 2)) < 0.9f) return true;
    }
    
    // Rochas decorativas
    if (sqrt(pow(x + 6.5f, 2) + pow(z + 6.5f, 2)) < 0.8f) return true;
    if (sqrt(pow(x - 6.5f, 2) + pow(z + 6.5f, 2)) < 0.8f) return true;
    if (sqrt(pow(x + 6.5f, 2) + pow(z - 6.5f, 2)) < 0.8f) return true;
    if (sqrt(pow(x - 6.5f, 2) + pow(z - 6.5f, 2)) < 0.8f) return true;
    
    return false;
}

void updatePhase5(int value) {
    (void)value;
    float speed = 0.08f;
    float lookX = sin(playerP5.angle);
    float lookZ = -cos(playerP5.angle);

    // Movimento com verificação de colisão
    float newX = playerP5.x;
    float newZ = playerP5.z;
    
    if(keyStateP5['w'] || keyStateP5['W']) { newX += lookX * speed; newZ += lookZ * speed; }
    if(keyStateP5['s'] || keyStateP5['S']) { newX -= lookX * speed; newZ -= lookZ * speed; }
    if(keyStateP5['a'] || keyStateP5['A']) { newX += lookZ * speed; newZ -= lookX * speed; }
    if(keyStateP5['d'] || keyStateP5['D']) { newX -= lookZ * speed; newZ += lookX * speed; }
    
    // Aplicar movimento apenas se não houver colisão
    if (!checkCollisionP5(newX, newZ)) {
        playerP5.x = newX;
        playerP5.z = newZ;
    }

    if (!phase5_won) {
        // Se estiver segurando um objeto, verificar se está perto de uma zona de depósito
        if (heldObjectIndex >= 0) {
            for (int i = 0; i < numDepositsP5; i++) {
                if (!depositsP5[i].filled) {
                    float dist = sqrt(pow(playerP5.x - depositsP5[i].x, 2) + pow(playerP5.z - depositsP5[i].z, 2));
                    if (dist < 1.5f) {
                        // Está perto de uma zona de depósito
                        if (keyStateP5['e'] || keyStateP5['E']) {
                            // Tentar depositar
                            if (objectsP5[heldObjectIndex].type == depositsP5[i].requiredType) {
                                // Correto! Posicionar objeto na plataforma
                                printf("Objeto depositado corretamente! Score: %d\n", scoreP5 + 1);
                                depositsP5[i].filled = true;
                                objectsP5[heldObjectIndex].x = depositsP5[i].x;
                                objectsP5[heldObjectIndex].y = 0.13f; // Altura no chão
                                objectsP5[heldObjectIndex].z = depositsP5[i].z;
                                objectsP5[heldObjectIndex].collected = true;
                                objectsP5[heldObjectIndex].held = false;
                                scoreP5++;
                                Audio::getInstance().play(Audio::SOUND_VICTORY);
                                heldObjectIndex = -1;
                                
                                // Verificar vitória
                                if (scoreP5 >= 4) {
                                    phase5_won = true;
                                    printf("Fase 5 completa!\n");
                                }
                                keyStateP5['e'] = false;
                                keyStateP5['E'] = false;
                            } else {
                                // Errado - tipo incorreto
                                printf("Objeto errado para esta plataforma!\n");
                                Audio::getInstance().play(Audio::SOUND_ERROR);
                                keyStateP5['e'] = false;
                                keyStateP5['E'] = false;
                            }
                        }
                    }
                }
            }
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, updatePhase5, 0);
}

void handlePhase5Keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keyStateP5[key] = true;
    
    // Pegar/soltar objeto com 'E'
    if (key == 'e' || key == 'E') {
        if (heldObjectIndex >= 0) {
            // Verificar se está perto de uma plataforma de depósito primeiro
            bool deposited = false;
            for (int i = 0; i < numDepositsP5; i++) {
                if (!depositsP5[i].filled) {
                    float dist = sqrt(pow(playerP5.x - depositsP5[i].x, 2) + pow(playerP5.z - depositsP5[i].z, 2));
                    if (dist < 1.5f) {
                        // Está perto de uma zona de depósito
                        if (objectsP5[heldObjectIndex].type == depositsP5[i].requiredType) {
                            // Correto! Posicionar objeto na plataforma
                            printf("Objeto depositado corretamente! Score: %d\n", scoreP5 + 1);
                            depositsP5[i].filled = true;
                            objectsP5[heldObjectIndex].x = depositsP5[i].x;
                            objectsP5[heldObjectIndex].y = 0.13f;
                            objectsP5[heldObjectIndex].z = depositsP5[i].z;
                            objectsP5[heldObjectIndex].collected = true;
                            objectsP5[heldObjectIndex].held = false;
                            scoreP5++;
                            Audio::getInstance().play(Audio::SOUND_VICTORY);
                            heldObjectIndex = -1;
                            deposited = true;
                            
                            // Verificar vitória
                            if (scoreP5 >= 4) {
                                phase5_won = true;
                                printf("Fase 5 completa!\n");
                            }
                        } else {
                            // Errado - tipo incorreto
                            printf("Objeto errado para esta plataforma!\n");
                            Audio::getInstance().play(Audio::SOUND_ERROR);
                        }
                        break;
                    }
                }
            }
            
            // Se não depositou, soltar no chão
            if (!deposited) {
                objectsP5[heldObjectIndex].x = playerP5.x + sin(playerP5.angle) * 1.0f;
                objectsP5[heldObjectIndex].y = 0.13f;
                objectsP5[heldObjectIndex].z = playerP5.z - cos(playerP5.angle) * 1.0f;
                objectsP5[heldObjectIndex].held = false;
                heldObjectIndex = -1;
            }
        } else {
            // Tentar pegar objeto próximo
            for (int i = 0; i < numObjectsP5; i++) {
                if (!objectsP5[i].collected && !objectsP5[i].held) {
                    float dist = sqrt(pow(playerP5.x - objectsP5[i].x, 2) + pow(playerP5.z - objectsP5[i].z, 2));
                    if (dist < 1.5f) {
                        heldObjectIndex = i;
                        objectsP5[i].held = true;
                        Audio::getInstance().play(Audio::SOUND_VICTORY);
                        break;
                    }
                }
            }
        }
    }
}

void handlePhase5KeyboardUp(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keyStateP5[key] = false;
}

void handlePhase5Mouse(int button, int state, int x, int y) {
    (void)button;
    (void)state;
    (void)x;
    (void)y;
    // Empty for now
}

void handlePhase5PassiveMotion(int x, int y) {
    (void)y;
    if (lastMouseXP5 == -1) {
        lastMouseXP5 = x;
        return;
    }
    int dx = x - lastMouseXP5;
    lastMouseXP5 = x;
    
    float sensitivity = 0.01f;
    playerP5.angle += dx * sensitivity;
}
