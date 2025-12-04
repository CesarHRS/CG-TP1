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

// ===================================================================
// Funções para desenhar o interior da nave
// ===================================================================

// Desenhar parede metálica
void drawMetalWallP5(float x, float z, float width, float height, float depth, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, height/2, z);
    glColor3f(r, g, b);
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Desenhar porta deslizante
void drawDoorP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Frame da porta
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glScalef(1.2f, 2.2f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Porta em si (mais clara)
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glScalef(1.0f, 2.0f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar cama do quarto
void drawBedP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.3f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Base da cama
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glScalef(2.0f, 0.3f, 1.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Colchão
    glColor3f(0.2f, 0.3f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(1.9f, 0.2f, 1.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar bancada da cozinha
void drawKitchenCounterP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Base
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glScalef(2.5f, 1.0f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pia
    glColor3f(0.7f, 0.7f, 0.8f);
    glPushMatrix();
    glTranslatef(-0.5f, 0.4f, 0.0f);
    glScalef(0.6f, 0.2f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar caixa de munição
void drawAmmoBoxP5(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.3f, z);
    
    glColor3f(0.3f, 0.4f, 0.2f); // Verde militar
    glPushMatrix();
    glScalef(0.6f, 0.4f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Detalhes
    glColor3f(0.8f, 0.7f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.21f);
    glScalef(0.3f, 0.05f, 0.01f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar rack de armas
void drawWeaponRackP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glScalef(0.1f, 2.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar console/painel de controle
void drawControlConsoleP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.8f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Base
    glColor3f(0.3f, 0.3f, 0.35f);
    glPushMatrix();
    glScalef(1.5f, 0.8f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Tela
    glColor3f(0.1f, 0.3f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.2f);
    glScalef(1.0f, 0.6f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar cadeira de comando
void drawCommandChairP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Assento
    glColor3f(0.2f, 0.2f, 0.3f);
    glPushMatrix();
    glScalef(0.6f, 0.2f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Encosto
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, -0.2f);
    glScalef(0.6f, 0.8f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar árvore 3D (REMOVIDO - não será mais usado)
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
    printf("Initializing Phase 5 (Inside the Spaceship)...\n");
    srand((unsigned int)time(NULL));

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat light_pos[] = {0.0, 3.0, 0.0, 1.0}; // Luz no centro da nave
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_DEPTH_TEST);

    playerP5.x = 0.0f;
    playerP5.y = 1.6f; // Altura dos olhos
    playerP5.z = 3.0f; // Começa de frente para o painel
    playerP5.angle = 0.0f; // Olhando para frente (norte - direção do painel)
    playerP5.pitch = 0.0f;
    
    lastMouseXP5 = -1;
    for(int i = 0; i < 256; i++) keyStateP5[i] = false;

    // 4 objetos escondidos em diferentes cômodos da nave
    numObjectsP5 = 4;
    
    // CUBO - Cozinha (dentro do quarto lateral direito traseiro)
    objectsP5[0] = (Collectible){8.0f, 0.2f, -7.0f, SHAPE_CUBE, false, false};
    
    // ESFERA - Quarto (dentro do quarto lateral esquerdo traseiro)
    objectsP5[1] = (Collectible){-8.0f, 0.2f, -7.0f, SHAPE_SPHERE, false, false};
    
    // PIRÂMIDE - Sala de Munição (lateral esquerda frontal, no centro do quarto)
    objectsP5[2] = (Collectible){-8.0f, 0.2f, 6.0f, SHAPE_PYRAMID, false, false};
    
    // CILINDRO - Área de console (lateral direita frontal, no centro do quarto)
    objectsP5[3] = (Collectible){8.0f, 0.2f, 6.0f, SHAPE_CYLINDER, false, false};
    
    // Zonas de depósito no painel de controle frontal
    numDepositsP5 = 4;
    depositsP5[0] = (DepositZone){-1.5f, 0.0f, 7.5f, SHAPE_CUBE, false}; // Slot vermelho
    depositsP5[1] = (DepositZone){-0.5f, 0.0f, 7.5f, SHAPE_SPHERE, false}; // Slot verde
    depositsP5[2] = (DepositZone){0.5f, 0.0f, 7.5f, SHAPE_PYRAMID, false}; // Slot azul
    depositsP5[3] = (DepositZone){1.5f, 0.0f, 7.5f, SHAPE_CYLINDER, false}; // Slot amarelo
    
    scoreP5 = 0;
    phase5_won = false;
    heldObjectIndex = -1;
}

void drawPhase5(int windowWidth, int windowHeight) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Escuro - interior da nave
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

    // ===================================================================
    // ESTRUTURA DA NAVE
    // ===================================================================
    
    // Chão metálico da nave (placas com padrão)
    glBegin(GL_QUADS);
    for (int x = -10; x < 10; x++) {
        for (int z = -10; z < 10; z++) {
            // Padrão de placas metálicas
            if ((x + z) % 2 == 0) {
                glColor3f(0.35f, 0.35f, 0.4f); // Cinza metálico claro
            } else {
                glColor3f(0.3f, 0.3f, 0.35f); // Cinza metálico escuro
            }
            
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f((float)x, 0.0f, (float)z);
            glVertex3f((float)x, 0.0f, (float)(z + 1));
            glVertex3f((float)(x + 1), 0.0f, (float)(z + 1));
            glVertex3f((float)(x + 1), 0.0f, (float)z);
        }
    }
    glEnd();
    
    // Teto da nave
    glBegin(GL_QUADS);
    glColor3f(0.25f, 0.25f, 0.28f);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-10.0f, 3.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f);
    glVertex3f(-10.0f, 3.0f, 10.0f);
    glEnd();
    
    // Paredes externas da nave
    glColor3f(0.3f, 0.3f, 0.35f);
    
    // Parede esquerda
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, 10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glEnd();
    
    // Parede direita
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f);
    glVertex3f(10.0f, 3.0f, -10.0f);
    glEnd();
    
    // Parede traseira
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, -10.0f);
    glEnd();
    
    // ===================================================================
    // PAREDE FRONTAL COM PARABRISA E PAINEL DE CONTROLE
    // ===================================================================
    
    // Parede frontal esquerda (lateral do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(-10.0f, 3.0f, 10.0f);
    glVertex3f(-6.5f, 3.0f, 10.0f);
    glVertex3f(-6.5f, 0.0f, 10.0f);
    glEnd();
    
    // Parede frontal direita (lateral do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(6.5f, 0.0f, 10.0f);
    glVertex3f(6.5f, 3.0f, 10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glEnd();
    
    // Parede frontal inferior (abaixo do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-6.5f, 0.0f, 10.0f);
    glVertex3f(-6.5f, 1.0f, 10.0f);
    glVertex3f(6.5f, 1.0f, 10.0f);
    glVertex3f(6.5f, 0.0f, 10.0f);
    glEnd();
    
    // Parabrisa (vidro transparente/azulado mostrando o espaço) - MAIOR
    glColor3f(0.1f, 0.15f, 0.3f); // Azul escuro do espaço
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-6.5f, 1.0f, 10.0f);
    glVertex3f(-6.5f, 3.0f, 10.0f);
    glVertex3f(6.5f, 3.0f, 10.0f);
    glVertex3f(6.5f, 1.0f, 10.0f);
    glEnd();
    
    // Estrelas vistas através do parabrisa (fixas)
    glDisable(GL_LIGHTING);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    // Estrelas em posições fixas (distribuídas pelo parabrisa maior)
    glVertex3f(-6.0f, 2.8f, 9.95f);
    glVertex3f(-5.2f, 1.5f, 9.95f);
    glVertex3f(-4.3f, 2.4f, 9.95f);
    glVertex3f(-3.5f, 2.8f, 9.95f);
    glVertex3f(-2.8f, 2.2f, 9.95f);
    glVertex3f(-1.5f, 2.9f, 9.95f);
    glVertex3f(-0.3f, 2.5f, 9.95f);
    glVertex3f(0.8f, 2.7f, 9.95f);
    glVertex3f(2.2f, 2.3f, 9.95f);
    glVertex3f(3.3f, 2.8f, 9.95f);
    glVertex3f(4.5f, 1.8f, 9.95f);
    glVertex3f(5.5f, 2.5f, 9.95f);
    glVertex3f(6.0f, 1.3f, 9.95f);
    glVertex3f(-5.8f, 1.2f, 9.95f);
    glVertex3f(-3.0f, 1.8f, 9.95f);
    glVertex3f(-1.8f, 1.4f, 9.95f);
    glVertex3f(0.5f, 1.9f, 9.95f);
    glVertex3f(1.9f, 1.7f, 9.95f);
    glVertex3f(3.8f, 1.5f, 9.95f);
    glVertex3f(5.2f, 2.0f, 9.95f);
    glEnd();
    glEnable(GL_LIGHTING);
    
    // Painel de controle principal (console grande na frente)
    drawControlConsoleP5(0.0f, 7.5f, 0.0f);
    drawCommandChairP5(0.0f, 6.0f, 0.0f);
    
    // ===================================================================
    // DIVISÓRIAS INTERNAS (paredes menores - altura 2.0)
    // ===================================================================
    
    glColor3f(0.35f, 0.35f, 0.4f);
    
    // Divisória horizontal esquerda (separa quarto da frente) - PAREDE ÚNICA
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, 0.0f);
    glVertex3f(-5.0f, 0.0f, 0.0f);
    glVertex3f(-5.0f, 2.0f, 0.0f);
    glVertex3f(-10.0f, 2.0f, 0.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 2.0f, 0.0f);
    glVertex3f(-5.0f, 2.0f, 0.0f);
    glVertex3f(-5.0f, 0.0f, 0.0f);
    glEnd();
    
    // Divisória horizontal direita (separa cozinha da frente) - PAREDE ÚNICA
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(5.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 2.0f, 0.0f);
    glVertex3f(5.0f, 2.0f, 0.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(5.0f, 0.0f, 0.0f);
    glVertex3f(5.0f, 2.0f, 0.0f);
    glVertex3f(10.0f, 2.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    glEnd();
    
    // Divisória vertical traseira removida para não aparecer no quarto
    
    // ===================================================================
    // PAREDES LATERAIS DOS QUARTOS (cercam áreas laterais, começam após o parabrisa)
    // ===================================================================
    
    // PAREDE LATERAL ESQUERDA - TRASEIRA (fecha o quarto)
    // De X=-6.5 (fim do parabrisa) até X=-10 (parede externa), em Z=-10 até Z=10
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0); // Face virada para dentro
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 2.0f, -10.0f);
    glVertex3f(-6.5f, 2.0f, -10.0f);
    glVertex3f(-6.5f, 0.0f, -10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0); // Face virada para fora
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-6.5f, 0.0f, -10.0f);
    glVertex3f(-6.5f, 2.0f, -10.0f);
    glVertex3f(-10.0f, 2.0f, -10.0f);
    glEnd();
    
    // PAREDE LATERAL ESQUERDA - FRONTAL (fecha a sala de munição)
    // De X=-6.5 até X=-10, em Z=10 (parede frontal da nave)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(-10.0f, 2.0f, 10.0f);
    glVertex3f(-6.5f, 2.0f, 10.0f);
    glVertex3f(-6.5f, 0.0f, 10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(-6.5f, 0.0f, 10.0f);
    glVertex3f(-6.5f, 2.0f, 10.0f);
    glVertex3f(-10.0f, 2.0f, 10.0f);
    glEnd();
    
    // PAREDE LATERAL ESQUERDA - CONECTORA COM PORTAS (X=-6.5)
    // Seção traseira (quarto): Z=-10 até Z=-2 (antes da porta)
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-6.5f, 0.0f, -10.0f);
    glVertex3f(-6.5f, 2.0f, -10.0f);
    glVertex3f(-6.5f, 2.0f, -2.0f);
    glVertex3f(-6.5f, 0.0f, -2.0f);
    glEnd();
    
    // Seção frontal (sala munição): Z=2 até Z=10 (depois da porta)
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-6.5f, 0.0f, 2.0f);
    glVertex3f(-6.5f, 2.0f, 2.0f);
    glVertex3f(-6.5f, 2.0f, 10.0f);
    glVertex3f(-6.5f, 0.0f, 10.0f);
    glEnd();
    
    // Verga acima da porta esquerda
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-6.5f, 1.8f, -2.0f);
    glVertex3f(-6.5f, 2.0f, -2.0f);
    glVertex3f(-6.5f, 2.0f, 2.0f);
    glVertex3f(-6.5f, 1.8f, 2.0f);
    glEnd();
    
    // PAREDE LATERAL DIREITA - TRASEIRA (fecha a cozinha)
    // De X=6.5 (fim do parabrisa) até X=10 (parede externa), em Z=-10
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(6.5f, 0.0f, -10.0f);
    glVertex3f(6.5f, 2.0f, -10.0f);
    glVertex3f(10.0f, 2.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(6.5f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 2.0f, -10.0f);
    glVertex3f(6.5f, 2.0f, -10.0f);
    glEnd();
    
    // PAREDE LATERAL DIREITA - FRONTAL (fecha a área de console)
    // De X=6.5 até X=10, em Z=10
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(6.5f, 0.0f, 10.0f);
    glVertex3f(6.5f, 2.0f, 10.0f);
    glVertex3f(10.0f, 2.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(6.5f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 2.0f, 10.0f);
    glVertex3f(6.5f, 2.0f, 10.0f);
    glEnd();
    
    // PAREDE LATERAL DIREITA - CONECTORA COM PORTAS (X=6.5)
    // Seção traseira (cozinha): Z=-10 até Z=-2 (antes da porta)
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(6.5f, 0.0f, -10.0f);
    glVertex3f(6.5f, 0.0f, -2.0f);
    glVertex3f(6.5f, 2.0f, -2.0f);
    glVertex3f(6.5f, 2.0f, -10.0f);
    glEnd();
    
    // Seção frontal (área console): Z=2 até Z=10 (depois da porta)
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(6.5f, 0.0f, 2.0f);
    glVertex3f(6.5f, 0.0f, 10.0f);
    glVertex3f(6.5f, 2.0f, 10.0f);
    glVertex3f(6.5f, 2.0f, 2.0f);
    glEnd();
    
    // Verga acima da porta direita
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(6.5f, 1.8f, -2.0f);
    glVertex3f(6.5f, 1.8f, 2.0f);
    glVertex3f(6.5f, 2.0f, 2.0f);
    glVertex3f(6.5f, 2.0f, -2.0f);
    glEnd();
    
    // ===================================================================
    // PAREDES INTERNAS DOS CÔMODOS COM PORTAS
    // ===================================================================
    
    // Parede interna do QUARTO (parede traseira entre corredor e quarto)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, -5.0f);
    glVertex3f(-6.0f, 0.0f, -5.0f); // Antes da porta
    glVertex3f(-6.0f, 2.0f, -5.0f);
    glVertex3f(-10.0f, 2.0f, -5.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-4.0f, 0.0f, -5.0f); // Depois da porta
    glVertex3f(0.0f, 0.0f, -5.0f);
    glVertex3f(0.0f, 2.0f, -5.0f);
    glVertex3f(-4.0f, 2.0f, -5.0f);
    glEnd();
    
    // Verga da porta do quarto (parte superior)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-6.0f, 1.8f, -5.0f);
    glVertex3f(-4.0f, 1.8f, -5.0f);
    glVertex3f(-4.0f, 2.0f, -5.0f);
    glVertex3f(-6.0f, 2.0f, -5.0f);
    glEnd();
    
    // Moldura da porta do quarto
    glColor3f(0.25f, 0.25f, 0.28f);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-6.1f, 0.0f, -5.01f);
    glVertex3f(-5.9f, 0.0f, -5.01f);
    glVertex3f(-5.9f, 1.9f, -5.01f);
    glVertex3f(-6.1f, 1.9f, -5.01f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(-4.1f, 0.0f, -5.01f);
    glVertex3f(-3.9f, 0.0f, -5.01f);
    glVertex3f(-3.9f, 1.9f, -5.01f);
    glVertex3f(-4.1f, 1.9f, -5.01f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(-6.0f, 1.7f, -5.01f);
    glVertex3f(-4.0f, 1.7f, -5.01f);
    glVertex3f(-4.0f, 1.9f, -5.01f);
    glVertex3f(-6.0f, 1.9f, -5.01f);
    glEnd();
    
    glColor3f(0.35f, 0.35f, 0.4f);
    
    // Parede interna da COZINHA (parede traseira entre corredor e cozinha)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(0.0f, 0.0f, -5.0f);
    glVertex3f(4.0f, 0.0f, -5.0f); // Antes da porta
    glVertex3f(4.0f, 2.0f, -5.0f);
    glVertex3f(0.0f, 2.0f, -5.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(6.0f, 0.0f, -5.0f); // Depois da porta
    glVertex3f(10.0f, 0.0f, -5.0f);
    glVertex3f(10.0f, 2.0f, -5.0f);
    glVertex3f(6.0f, 2.0f, -5.0f);
    glEnd();
    
    // Verga da porta da cozinha
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(4.0f, 1.8f, -5.0f);
    glVertex3f(6.0f, 1.8f, -5.0f);
    glVertex3f(6.0f, 2.0f, -5.0f);
    glVertex3f(4.0f, 2.0f, -5.0f);
    glEnd();
    
    // Moldura da porta da cozinha
    glColor3f(0.25f, 0.25f, 0.28f);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(3.9f, 0.0f, -5.01f);
    glVertex3f(4.1f, 0.0f, -5.01f);
    glVertex3f(4.1f, 1.9f, -5.01f);
    glVertex3f(3.9f, 1.9f, -5.01f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(5.9f, 0.0f, -5.01f);
    glVertex3f(6.1f, 0.0f, -5.01f);
    glVertex3f(6.1f, 1.9f, -5.01f);
    glVertex3f(5.9f, 1.9f, -5.01f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(4.0f, 1.7f, -5.01f);
    glVertex3f(6.0f, 1.7f, -5.01f);
    glVertex3f(6.0f, 1.9f, -5.01f);
    glVertex3f(4.0f, 1.9f, -5.01f);
    glEnd();
    
    glColor3f(0.35f, 0.35f, 0.4f);
    
    // ===================================================================
    // QUARTO (Canto traseiro-esquerdo)
    // ===================================================================
    drawBedP5(-7.0f, -7.0f, 0.0f);
    
    // Mesa de cabeceira
    glPushMatrix();
    glTranslatef(-8.5f, 0.3f, -7.0f);
    glColor3f(0.4f, 0.35f, 0.3f);
    glScalef(0.5f, 0.6f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // ===================================================================
    // COZINHA (Canto traseiro-direito)
    // ===================================================================
    drawKitchenCounterP5(7.0f, -7.0f, 0.0f);
    
    // Geladeira
    glPushMatrix();
    glTranslatef(8.5f, 1.0f, -7.5f);
    glColor3f(0.8f, 0.8f, 0.85f);
    glScalef(0.8f, 2.0f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Mesa pequena
    glPushMatrix();
    glTranslatef(6.0f, 0.4f, -5.5f);
    glColor3f(0.45f, 0.4f, 0.35f);
    glScalef(1.2f, 0.1f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // ===================================================================
    // SALA DE MUNIÇÃO (Canto frontal-esquerdo)
    // ===================================================================
    drawWeaponRackP5(-8.5f, 6.0f, 90.0f);
    
    // Caixas de munição empilhadas
    drawAmmoBoxP5(-7.5f, 7.0f);
    drawAmmoBoxP5(-7.0f, 7.0f);
    drawAmmoBoxP5(-7.3f, 7.35f); // Segunda camada
    drawAmmoBoxP5(-8.0f, 5.5f);
    drawAmmoBoxP5(-6.5f, 5.5f);
    
    // ===================================================================
    // ÁREA DE CONSOLE AUXILIAR (Canto frontal-direito)
    // ===================================================================
    drawControlConsoleP5(7.0f, 6.5f, 180.0f);
    drawCommandChairP5(6.5f, 5.5f, 180.0f);
    
    // Estante com equipamentos
    glPushMatrix();
    glTranslatef(8.5f, 1.0f, 7.0f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(0.8f, 2.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // ===================================================================
    // PAINEL DE DEPÓSITO (Slots no console principal - horizontal)
    // ===================================================================
    glDisable(GL_LIGHTING);
    for (int i = 0; i < numDepositsP5; i++) {
        glPushMatrix();
        glTranslatef(depositsP5[i].x, 1.25f, depositsP5[i].z);
        
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
        
        // Slot retangular no painel (horizontal - plataforma no topo do console)
        glPushMatrix();
        glScalef(0.6f, 0.05f, 0.6f); // Achatado horizontalmente
        glutSolidCube(1.0f);
        glPopMatrix();
        
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

    // Voltar para HUD 2D
    glMatrixMode(GL_PROJECTION);
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
            case SHAPE_CUBE: heldName = "Segurando: CUBO - Leve ao slot VERMELHO no painel"; break;
            case SHAPE_SPHERE: heldName = "Segurando: ESFERA - Leve ao slot VERDE no painel"; break;
            case SHAPE_PYRAMID: heldName = "Segurando: PIRAMIDE - Leve ao slot AZUL no painel"; break;
            case SHAPE_CYLINDER: heldName = "Segurando: CILINDRO - Leve ao slot AMARELO no painel"; break;
        }
        drawTextP5(10, windowHeight - 45, heldName);
    } else {
        glColor3f(1.0, 1.0, 1.0);
        drawTextP5(10, windowHeight - 45, "E para pegar | Explore os comodos da nave | Deposite no painel frontal");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Verificar colisão com obstáculos (paredes da nave e móveis)
bool checkCollisionP5(float x, float z) {
    float radius = 0.3f; // Raio de colisão do jogador
    
    // PAREDES EXTERNAS da nave (limites -10 a 10)
    if (x < -9.5f + radius || x > 9.5f - radius || z < -9.5f + radius || z > 9.5f - radius) return true;
    
    // PAREDES LATERAIS COM PORTAS (X = -6.5 e X = 6.5)
    // Parede lateral esquerda - seção traseira (Z de -10 até -2)
    if (fabs(x + 6.5f) < 0.15f && z < -2.0f + radius && z > -10.0f - radius) return true;
    // Parede lateral esquerda - seção frontal (Z de 2 até 10)
    if (fabs(x + 6.5f) < 0.15f && z > 2.0f - radius && z < 10.0f + radius) return true;
    
    // Parede lateral direita - seção traseira (Z de -10 até -2)
    if (fabs(x - 6.5f) < 0.15f && z < -2.0f + radius && z > -10.0f - radius) return true;
    // Parede lateral direita - seção frontal (Z de 2 até 10)
    if (fabs(x - 6.5f) < 0.15f && z > 2.0f - radius && z < 10.0f + radius) return true;
    
    // PAREDES DE FUNDO DOS QUARTOS LATERAIS (Z = -10 e Z = 10, entre X = ±6.5 e X = ±10)
    // Parede fundo esquerda traseira (Z=-10, X de -10 a -6.5)
    if (fabs(z + 10.0f) < 0.15f && x < -6.5f + radius && x > -10.0f) return true;
    // Parede fundo esquerda frontal (Z=10, X de -10 a -6.5)
    if (fabs(z - 10.0f) < 0.15f && x < -6.5f + radius && x > -10.0f) return true;
    // Parede fundo direita traseira (Z=-10, X de 6.5 a 10)
    if (fabs(z + 10.0f) < 0.15f && x > 6.5f - radius && x < 10.0f) return true;
    // Parede fundo direita frontal (Z=10, X de 6.5 a 10)
    if (fabs(z - 10.0f) < 0.15f && x > 6.5f - radius && x < 10.0f) return true;
    
    // CAMA no quarto (canto traseiro esquerdo)
    if (x > -8.5f - radius && x < -5.5f + radius && z > -8.5f - radius && z < -5.5f + radius) return true;
    
    // BANCADA DA COZINHA (canto traseiro direito)
    if (x > 5.5f - radius && x < 8.5f + radius && z > -8.5f - radius && z < -5.5f + radius) return true;
    
    // GELADEIRA na cozinha
    if (x > 7.8f - radius && x < 9.2f + radius && z > -8.2f - radius && z < -6.8f + radius) return true;
    
    // CAIXAS DE MUNIÇÃO (sala de munição - canto frontal esquerdo)
    if (fabs(x + 7.5f) < 1.0f && fabs(z - 7.0f) < 1.0f) return true;
    if (fabs(x + 8.0f) < 1.0f && fabs(z - 5.5f) < 1.0f) return true;
    
    // CONSOLE AUXILIAR (área de console - canto frontal direito)
    if (x > 5.5f - radius && x < 8.5f + radius && z > 5.0f - radius && z < 8.0f + radius) return true;
    
    // ESTANTE na área de console
    if (x > 7.8f - radius && x < 9.2f + radius && z > 6.0f - radius && z < 8.0f + radius) return true;
    
    // PAINEL DE CONTROLE PRINCIPAL e CADEIRA (centro frontal)
    if (fabs(x) < 1.5f + radius && z > 6.0f - radius && z < 8.0f + radius) return true;
    
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
