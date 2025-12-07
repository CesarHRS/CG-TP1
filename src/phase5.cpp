#include "phase5.h"
#include "audio.h"
#include "gameover.h"
#include "menu.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLuint textureWallP5 = 0;
GLuint textureFloorP5 = 0;
GLuint texturePyramidP5 = 0;
GLuint textureCubeP5 = 0;
GLuint textureSphereP5 = 0;
GLuint textureCylinderP5 = 0;
bool texturesLoadedP5 = false;


Player3D playerP5;
Collectible objectsP5[10];
DepositZone depositsP5[4];
int numObjectsP5;
int numDepositsP5;
int scoreP5;
bool phase5_won = false;
int heldObjectIndex = -1; // Índice do objeto sendo segurado (-1 = nenhum)

// Variáveis do temporizador
float timeRemainingP5 = 30.0f; // 30 segundos
bool gameOverP5 = false;

// Variáveis da contagem regressiva
bool showCountdownP5 = false;
int countdownTimerP5 = 0;
int countdownValueP5 = 3;

bool keyStateP5[256];

// Variáveis de controle do mouse
int lastMouseXP5;
int lastMouseYP5;


GLuint createProceduralTexture(int width, int height, int type) {
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            switch(type) {
                case 0: // Parede metálica
                    {
                        int noise = rand() % 30;
                        data[index] = 120 + noise;
                        data[index + 1] = 130 + noise;
                        data[index + 2] = 140 + noise;
                    }
                    break;
                case 1: // Piso
                    {
                        int checker = ((x / 16) % 2) ^ ((y / 16) % 2);
                        int base = checker ? 80 : 100;
                        data[index] = base;
                        data[index + 1] = base;
                        data[index + 2] = base;
                    }
                    break;
                case 2: // Pirâmide (gradiente vermelho)
                    {
                        int grad = (x + y) % 255;
                        data[index] = 200 + (grad % 55);
                        data[index + 1] = 50 + (grad % 50);
                        data[index + 2] = 50;
                    }
                    break;
                case 3: // Cubo (gradiente azul)
                    {
                        int grad = (x * y) % 255;
                        data[index] = 50 + (grad % 50);
                        data[index + 1] = 100 + (grad % 50);
                        data[index + 2] = 200 + (grad % 55);
                    }
                    break;
                case 4: // Esfera (gradiente verde)
                    {
                        int grad = (int)sqrt(x * x + y * y) % 255;
                        data[index] = 50 + (grad % 50);
                        data[index + 1] = 200 + (grad % 55);
                        data[index + 2] = 100 + (grad % 50);
                    }
                    break;
                case 5: // Cilindro (gradiente amarelo)
                    {
                        int grad = (x % 128) + (y % 128);
                        data[index] = 200 + (grad % 55);
                        data[index + 1] = 200 + (grad % 55);
                        data[index + 2] = 50;
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

void loadTexturesP5() {
    // Recriar texturas sempre para evitar conflitos com outras fases
    textureWallP5 = createProceduralTexture(256, 256, 0);
    textureFloorP5 = createProceduralTexture(256, 256, 1);
    texturePyramidP5 = createProceduralTexture(128, 128, 2);
    textureCubeP5 = createProceduralTexture(128, 128, 3);
    textureSphereP5 = createProceduralTexture(128, 128, 4);
    textureCylinderP5 = createProceduralTexture(128, 128, 5);
    texturesLoadedP5 = true;
    printf("Texturas procedurais da Fase 5 criadas\n");
}

void drawPyramidP5() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturePyramidP5);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_TRIANGLES);
        // Face 1
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f, 1.f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.f, -1.f, 1.f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.f, -1.f, 1.f);
        // Face 2
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(1.f, -1.f, 1.f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.f, -1.f, -1.f);
        // Face 3
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(1.f, -1.f, -1.f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.f, -1.f, -1.f);
        // Face 4
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f, 1.f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.f,-1.f,-1.f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.f,-1.f, 1.f);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void drawCylinderP5(float radius, float height, int sides) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureCylinderP5);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * i / sides;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float texS = (float)i / sides;
        glNormal3f(x/radius, 0.0, z/radius);
        glTexCoord2f(texS, 0.0f); glVertex3f(x, 0, z);
        glTexCoord2f(texS, 1.0f); glVertex3f(x, height, z);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * i / sides;
        float texS = 0.5f + 0.5f * cos(angle);
        float texT = 0.5f + 0.5f * sin(angle);
        glTexCoord2f(texS, texT);
        glVertex3f(radius * cos(angle), height, radius * sin(angle));
    }
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void drawTextP5(float x, float y, const char *string) {
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Desenhar texto 3D no mundo
void drawText3DP5(float x, float y, float z, const char *string, void* font) {
    // Desenhar texto branco
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, z);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Desenhar parede metálica
void drawMetalWallP5(float x, float z, float width, float height, float depth, float r, float g, float b) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureWallP5);
    glColor3f(r, g, b);
    
    glPushMatrix();
    glTranslatef(x, height/2, z);
    
    // Desenhar cubo com coordenadas de textura
    glBegin(GL_QUADS);
    // Frente
    glTexCoord2f(0, 0); glVertex3f(-width/2, -height/2, depth/2);
    glTexCoord2f(1, 0); glVertex3f(width/2, -height/2, depth/2);
    glTexCoord2f(1, 1); glVertex3f(width/2, height/2, depth/2);
    glTexCoord2f(0, 1); glVertex3f(-width/2, height/2, depth/2);
    // Trás
    glTexCoord2f(0, 0); glVertex3f(-width/2, -height/2, -depth/2);
    glTexCoord2f(1, 0); glVertex3f(width/2, -height/2, -depth/2);
    glTexCoord2f(1, 1); glVertex3f(width/2, height/2, -depth/2);
    glTexCoord2f(0, 1); glVertex3f(-width/2, height/2, -depth/2);
    // Topo
    glTexCoord2f(0, 0); glVertex3f(-width/2, height/2, -depth/2);
    glTexCoord2f(1, 0); glVertex3f(width/2, height/2, -depth/2);
    glTexCoord2f(1, 1); glVertex3f(width/2, height/2, depth/2);
    glTexCoord2f(0, 1); glVertex3f(-width/2, height/2, depth/2);
    // Base
    glTexCoord2f(0, 0); glVertex3f(-width/2, -height/2, -depth/2);
    glTexCoord2f(1, 0); glVertex3f(width/2, -height/2, -depth/2);
    glTexCoord2f(1, 1); glVertex3f(width/2, -height/2, depth/2);
    glTexCoord2f(0, 1); glVertex3f(-width/2, -height/2, depth/2);
    // Direita
    glTexCoord2f(0, 0); glVertex3f(width/2, -height/2, -depth/2);
    glTexCoord2f(1, 0); glVertex3f(width/2, -height/2, depth/2);
    glTexCoord2f(1, 1); glVertex3f(width/2, height/2, depth/2);
    glTexCoord2f(0, 1); glVertex3f(width/2, height/2, -depth/2);
    // Esquerda
    glTexCoord2f(0, 0); glVertex3f(-width/2, -height/2, -depth/2);
    glTexCoord2f(1, 0); glVertex3f(-width/2, -height/2, depth/2);
    glTexCoord2f(1, 1); glVertex3f(-width/2, height/2, depth/2);
    glTexCoord2f(0, 1); glVertex3f(-width/2, height/2, -depth/2);
    glEnd();
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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

// Desenhar sofá
void drawSofaP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Assento
    glColor3f(0.25f, 0.25f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glScalef(1.8f, 0.4f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Encosto
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, -0.35f);
    glScalef(1.8f, 0.8f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Braços laterais
    glColor3f(0.22f, 0.22f, 0.28f);
    glPushMatrix();
    glTranslatef(-0.85f, 0.5f, 0.0f);
    glScalef(0.1f, 0.6f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.85f, 0.5f, 0.0f);
    glScalef(0.1f, 0.6f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Desenhar TV/Monitor
void drawTVP5(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    
    // Moldura da TV
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glScalef(1.5f, 1.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Tela
    glColor3f(0.05f, 0.1f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.06f);
    glScalef(1.3f, 0.8f, 0.01f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Base/suporte
    glColor3f(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, -0.6f, 0.0f);
    glScalef(0.3f, 0.2f, 0.2f);
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

bool isValidObjectPosition(float x, float z) {
    // Verificar limites da nave
    if (x < -9.3f || x > 9.3f || z < -9.3f || z > 9.3f) return false;
    
    // Evitar corredor central (onde o jogador começa e área dos vasos)
    if (x > -3.2f && x < 3.2f && z > -3.2f && z < 3.2f) return false;
    
    // Evitar área do painel frontal e console
    if (z > 6.3f) return false;
    
    // QUARTO
    if (x >= -10.0f && x <= -3.0f && z >= -10.0f && z <= -3.0f) {
        // Evitar cama 
        if (x > -8.5f && x < -5.5f && z > -8.5f && z < -5.5f) return false;
        // Evitar mesa de cabeceira
        if (x > -9.0f && x < -8.0f && z > -7.0f && z < -6.0f) return false;
        // Evitar armário
        if (x > -10.0f && x < -8.9f && z > -9.8f && z < -8.2f) return false;
        // Evitar cadeira
        if (x > -4.9f && x < -4.1f && z > -9.4f && z < -8.6f) return false;
        // Evitar muito perto das paredes e portas
        if (x < -9.0f || x > -3.8f || z < -9.0f || z > -3.8f) return false;
        return true;
    }
    
    // COZINHA 
    if (x >= 3.0f && x <= 10.0f && z >= -10.0f && z <= -3.0f) {
        // Evitar bancada 
        if (x > 5.5f && x < 8.5f && z > -8.5f && z < -6.0f) return false;
        // Evitar geladeira
        if (x > 7.8f && x < 9.8f && z > -8.0f && z < -6.0f) return false;
        // Evitar mesa
        if (x > 4.3f && x < 5.7f && z > -5.0f && z < -4.0f) return false;
        // Evitar fogão
        if (x > 3.9f && x < 5.1f && z > -8.1f && z < -6.9f) return false;
        // Evitar prateleiras na parede
        if (x > 9.0f && z > -6.5f && z < -4.5f) return false;
        // Evitar muito perto das paredes
        if (x < 3.8f || x > 9.0f || z < -9.0f || z > -3.8f) return false;
        return true;
    }
    
    // SALA DE MUNIÇÃO 
    if (x >= -10.0f && x <= -3.0f && z >= 3.0f && z <= 8.5f) {
        // Evitar rack de armas no canto frontal
        if (x > -9.1f && x < -7.9f && z > 6.7f && z < 8.2f) return false;
        // Evitar todas as caixas de munição empilhadas
        if (x > -8.3f && x < -6.5f && z > 4.5f && z < 6.0f) return false;
        // Evitar mesa de trabalho (X=-9, Z=7.5) - canto esquerdo frontal
        if (x > -9.8f && x < -8.2f && z > 7.0f && z < 8.0f) return false;
        // Evitar caixas extras no canto traseiro e frontal
        if (x > -10.0f && x < -8.5f && z > 3.0f && z < 4.8f) return false;
        if (x > -10.0f && x < -9.0f && z > 7.0f && z < 8.0f) return false;
        if (x > -5.5f && x < -4.5f && z > 7.0f && z < 8.0f) return false;
        // Manter distância segura de TODAS as paredes (0.5 unidades de margem)
        // X: -10 a -3, então válido: -9.5 a -3.5
        // Z: 3 a 8.5, então válido: 3.5 a 8.0
        if (x < -9.5f || x > -3.5f || z < 3.5f || z > 8.0f) return false;
        return true;
    }
    
    // ÁREA DE CONSOLE (X=3 a 10, Z=3 a 8.5) - sala FRONTAL estendida
    if (x >= 3.0f && x <= 10.0f && z >= 3.0f && z <= 8.5f) {
        // Evitar console auxiliar (X=7.0, Z=7.8) - na área frontal
        if (x > 6.5f && x < 7.5f && z > 7.3f && z < 8.3f) return false;
        // Evitar cadeira (X=6.5, Z=6.8) - próxima ao console
        if (x > 6.0f && x < 7.0f && z > 6.3f && z < 7.3f) return false;
        // Evitar sofá (X=5.5, Z=5.8, largura 1.8, profundidade 0.8)
        if (x > 4.5f && x < 6.5f && z > 5.3f && z < 6.3f) return false;
        // Evitar estante (X=8.5, Z=7.8) - na área frontal expandida
        if (x > 8.0f && x < 9.0f && z > 7.3f && z < 8.3f) return false;
        // Evitar monitor na parede (X=9.4, Z=4.5)
        if (x > 9.0f && z > 4.0f && z < 5.0f) return false;
        // Evitar caixa de ferramentas (X=6.0, Z=6.0)
        if (x > 5.5f && x < 6.5f && z > 5.7f && z < 6.3f) return false;
        // Manter distância segura de TODAS as paredes (0.5 unidades de margem)
        // X: 3 a 10, então válido: 3.5 a 9.5
        // Z: 3 a 8.5, então válido: 3.5 a 8.0
        if (x < 3.5f || x > 9.5f || z < 3.5f || z > 8.0f) return false;
        return true;
    }
    
    return false;
}

// Verificar se a posição está muito próxima de objetos existentes
bool isTooCloseToOtherObjects(float x, float z, int currentObjectCount) {
    float minDistance = 1.2f;
    
    for (int i = 0; i < currentObjectCount; i++) {
        float dx = x - objectsP5[i].x;
        float dz = z - objectsP5[i].z;
        float distance = sqrt(dx * dx + dz * dz);
        
        if (distance < minDistance) {
            return true; // Muito próximo de outro objeto
        }
    }
    return false;
}

// Gerar posição aleatória dentro de uma sala
void generateRandomObjectPosition(float &x, float &z, int roomIndex, int currentObjectCount) {
    int maxAttempts = 100;
    int attempts = 0;
    
    // Define room bounds (expandidos para evitar amontoamento)
    float minX, maxX, minZ, maxZ;
    switch(roomIndex) {
        case 0: // Quarto - áreas livres
            minX = -8.5f; maxX = -4.5f;
            minZ = -8.5f; maxZ = -4.5f;
            break;
        case 1: // Cozinha - área central livre
            minX = 4.5f; maxX = 8.5f;
            minZ = -8.5f; maxZ = -4.5f;
            break;
        case 2: // Sala de Munição - evitando rack/mesa frontal
            minX = -7.0f; maxX = -4.0f;
            minZ = 3.8f; maxZ = 6.5f;  // Evita área frontal
            break;
        case 3: // Área de Console
            minX = 4.0f; maxX = 6.0f;  // Área central, evita console e estante
            minZ = 3.8f; maxZ = 5.0f;  // Evita sofá e console frontal
            break;
        default:
            minX = -6.0f; maxX = 6.0f;
            minZ = -6.0f; maxZ = 6.0f;
            break;
    }
    
    do {
        x = minX + ((float)rand() / RAND_MAX) * (maxX - minX);
        z = minZ + ((float)rand() / RAND_MAX) * (maxZ - minZ);
        attempts++;
    } while ((!isValidObjectPosition(x, z) || isTooCloseToOtherObjects(x, z, currentObjectCount)) && attempts < maxAttempts);
    
    // Fallback para posição segura se não encontrar posição válida
    if (attempts >= maxAttempts) {
        switch(roomIndex) {
            case 0: x = -6.5f; z = -5.0f; break;  // Quarto (centro)
            case 1: x = 6.0f; z = -5.0f; break;   // Cozinha (centro)
            case 2: x = -5.5f; z = 5.5f; break;   // Sala Munição (centro, longe de móveis)
            case 3: x = 5.0f; z = 4.5f; break;    // Área Console (centro, longe do sofá)
        }
        printf("AVISO: Objeto %d colocado em posicao fallback: (%.2f, %.2f)\n", roomIndex, x, z);
    }
}

void returnToMenuFromPhase5();

void initPhase5() {
    printf("Initializing Phase 5 (Inside the Spaceship)...\n");
    srand((unsigned int)time(NULL));

    // Garantir que a iluminação esteja ativa
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Configurar luz ambiente e difusa
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_pos[] = {0.0, 3.0, 0.0, 1.0}; // Luz no centro da nave
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    
    // Configurar propriedades de material padrão
    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat mat_shininess[] = {0.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    glEnable(GL_DEPTH_TEST);
    
    // Carregar texturas
    loadTexturesP5();

    playerP5.x = 0.0f;
    playerP5.y = 1.6f; // Altura dos olhos
    playerP5.z = 3.0f; // Começa de frente para o painel
    playerP5.angle = 0.0f; // Olhando para frente (norte - direção do painel)
    playerP5.pitch = 0.0f;
    
    lastMouseXP5 = -1;
    lastMouseYP5 = -1;
    for(int i = 0; i < 256; i++) keyStateP5[i] = false;

    // 8 objetos escondidos em diferentes cômodos da nave (2 de cada tipo)
    numObjectsP5 = 8;
    
    // CUBO 1 - Quarto
    float cubeX, cubeZ;
    generateRandomObjectPosition(cubeX, cubeZ, 0, 0);
    objectsP5[0] = (Collectible){cubeX, 0.2f, cubeZ, SHAPE_CUBE, false, false};
    printf("CUBO 1 gerado em: (%.2f, %.2f)\n", cubeX, cubeZ);
    
    // CUBO 2 - Quarto
    generateRandomObjectPosition(cubeX, cubeZ, 0, 1);
    objectsP5[1] = (Collectible){cubeX, 0.2f, cubeZ, SHAPE_CUBE, false, false};
    printf("CUBO 2 gerado em: (%.2f, %.2f)\n", cubeX, cubeZ);
    
    // ESFERA 1 - Cozinha
    float sphereX, sphereZ;
    generateRandomObjectPosition(sphereX, sphereZ, 1, 2);
    objectsP5[2] = (Collectible){sphereX, 0.2f, sphereZ, SHAPE_SPHERE, false, false};
    printf("ESFERA 1 gerada em: (%.2f, %.2f)\n", sphereX, sphereZ);
    
    // ESFERA 2 - Cozinha
    generateRandomObjectPosition(sphereX, sphereZ, 1, 3);
    objectsP5[3] = (Collectible){sphereX, 0.2f, sphereZ, SHAPE_SPHERE, false, false};
    printf("ESFERA 2 gerada em: (%.2f, %.2f)\n", sphereX, sphereZ);
    
    // PIRÂMIDE 1 - Sala de Munição
    float pyramidX, pyramidZ;
    generateRandomObjectPosition(pyramidX, pyramidZ, 2, 4);
    objectsP5[4] = (Collectible){pyramidX, 0.2f, pyramidZ, SHAPE_PYRAMID, false, false};
    printf("PIRÂMIDE 1 gerada em: (%.2f, %.2f)\n", pyramidX, pyramidZ);
    
    // PIRÂMIDE 2 - Sala de Munição
    generateRandomObjectPosition(pyramidX, pyramidZ, 2, 5);
    objectsP5[5] = (Collectible){pyramidX, 0.2f, pyramidZ, SHAPE_PYRAMID, false, false};
    printf("PIRÂMIDE 2 gerada em: (%.2f, %.2f)\n", pyramidX, pyramidZ);
    
    // CILINDRO 1 - Área de Console
    float cylinderX, cylinderZ;
    generateRandomObjectPosition(cylinderX, cylinderZ, 3, 6);
    objectsP5[6] = (Collectible){cylinderX, 0.2f, cylinderZ, SHAPE_CYLINDER, false, false};
    printf("CILINDRO 1 gerado em: (%.2f, %.2f)\n", cylinderX, cylinderZ);
    
    // CILINDRO 2 - Área de Console
    generateRandomObjectPosition(cylinderX, cylinderZ, 3, 7);
    objectsP5[7] = (Collectible){cylinderX, 0.2f, cylinderZ, SHAPE_CYLINDER, false, false};
    printf("CILINDRO 2 gerado em: (%.2f, %.2f)\n", cylinderX, cylinderZ);
    
    // Zonas de depósito no painel de controle frontal (4 bases, 2 objetos por base)
    numDepositsP5 = 4;
    depositsP5[0] = (DepositZone){-1.5f, 0.0f, 7.5f, SHAPE_CUBE, 0}; // Slot vermelho
    depositsP5[1] = (DepositZone){-0.5f, 0.0f, 7.5f, SHAPE_SPHERE, 0}; // Slot verde
    depositsP5[2] = (DepositZone){0.5f, 0.0f, 7.5f, SHAPE_PYRAMID, 0}; // Slot azul
    depositsP5[3] = (DepositZone){1.5f, 0.0f, 7.5f, SHAPE_CYLINDER, 0}; // Slot amarelo
    
    scoreP5 = 0;
    phase5_won = false;
    heldObjectIndex = -1;
    timeRemainingP5 = 30.0f;
    gameOverP5 = false;
    
    // Initialize countdown
    showCountdownP5 = true;
    countdownTimerP5 = 0;
    countdownValueP5 = 3;

    setGameOver(false);
    setVictory(false);
    initGameOver(800, 700);
    registerRestartCallback(initPhase5);
    registerMenuCallback(returnToMenuFromPhase5);
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
    float lookY = playerP5.y + sin(playerP5.pitch);
    float lookZ = playerP5.z - cos(playerP5.angle);
    gluLookAt(playerP5.x, playerP5.y, playerP5.z, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);

    // Garantir que a iluminação esteja ativa (pode ter sido desabilitada por outra fase)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Reconfigurar propriedades de material (podem ter sido alteradas por outra fase)
    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat mat_shininess[] = {0.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    glEnable(GL_DEPTH_TEST);

    // Chão metálico da nave (placas com padrão)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureFloorP5);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_QUADS);
    for (int x = -10; x < 10; x++) {
        for (int z = -10; z < 10; z++) {
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f((float)x, 0.0f, (float)z);
            glTexCoord2f(0.0f, 1.0f); glVertex3f((float)x, 0.0f, (float)(z + 1));
            glTexCoord2f(1.0f, 1.0f); glVertex3f((float)(x + 1), 0.0f, (float)(z + 1));
            glTexCoord2f(1.0f, 0.0f); glVertex3f((float)(x + 1), 0.0f, (float)z);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
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
    
    // Parede frontal esquerda (lateral do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(-10.0f, 3.0f, 10.0f);
    glVertex3f(-4.0f, 3.0f, 10.0f);
    glVertex3f(-4.0f, 0.0f, 10.0f);
    glEnd();
    
    // Parede frontal direita (lateral do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(4.0f, 0.0f, 10.0f);
    glVertex3f(4.0f, 3.0f, 10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glEnd();
    
    // Parede frontal inferior (abaixo do parabrisa)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-4.0f, 0.0f, 10.0f);
    glVertex3f(-4.0f, 1.0f, 10.0f);
    glVertex3f(4.0f, 1.0f, 10.0f);
    glVertex3f(4.0f, 0.0f, 10.0f);
    glEnd();
    
    // Parabrisa
    glColor3f(0.1f, 0.15f, 0.3f); // Azul escuro do espaço
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-4.0f, 1.0f, 10.0f);
    glVertex3f(-4.0f, 3.0f, 10.0f);
    glVertex3f(4.0f, 3.0f, 10.0f);
    glVertex3f(4.0f, 1.0f, 10.0f);
    glEnd();
    
    // Estrelas vistas através do parabrisa
    glDisable(GL_LIGHTING);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-3.5f, 2.8f, 9.95f);
    glVertex3f(-2.8f, 2.2f, 9.95f);
    glVertex3f(-1.5f, 2.9f, 9.95f);
    glVertex3f(-0.3f, 2.5f, 9.95f);
    glVertex3f(0.8f, 2.7f, 9.95f);
    glVertex3f(2.2f, 2.3f, 9.95f);
    glVertex3f(3.3f, 2.8f, 9.95f);
    glVertex3f(-3.0f, 2.1f, 9.95f);
    glVertex3f(-1.8f, 1.4f, 9.95f);
    glVertex3f(0.5f, 1.9f, 9.95f);
    glVertex3f(1.9f, 1.7f, 9.95f);
    glVertex3f(3.5f, 1.5f, 9.95f);
    glVertex3f(-2.2f, 2.6f, 9.95f);
    glVertex3f(2.8f, 1.3f, 9.95f);
    glEnd();
    glEnable(GL_LIGHTING);
    
    // Painel de controle principal (console grande na frente)
    drawControlConsoleP5(0.0f, 7.5f, 0.0f);
    drawCommandChairP5(0.0f, 6.0f, 0.0f);

    // Ativar polygon offset para evitar z-fighting (linhas brancas)
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    
    glColor3f(0.35f, 0.35f, 0.4f);
    
    // Parede traseira (Z=-10)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-3.0f, 0.0f, -10.0f);
    glVertex3f(-3.0f, 3.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, -10.0f);
    glEnd();
    
    // Parede esquerda (X=-10)
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, -10.0f);
    glVertex3f(-10.0f, 3.0f, -3.0f);
    glVertex3f(-10.0f, 0.0f, -3.0f);
    glEnd();
    
    // Parede frontal (Z=-3)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, -3.0f);
    glVertex3f(-10.0f, 3.0f, -3.0f);
    glVertex3f(-3.0f, 3.0f, -3.0f);
    glVertex3f(-3.0f, 0.0f, -3.0f);
    glEnd();
    
    // Parede direita  com porta - seção inferior
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 0.0f, -10.0f);
    glVertex3f(-3.0f, 0.0f, -7.0f);
    glVertex3f(-3.0f, 3.0f, -7.0f);
    glVertex3f(-3.0f, 3.0f, -10.0f);
    glEnd();
    
    // Parede direita com porta
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 0.0f, -5.0f);
    glVertex3f(-3.0f, 0.0f, -3.0f);
    glVertex3f(-3.0f, 3.0f, -3.0f);
    glVertex3f(-3.0f, 3.0f, -5.0f);
    glEnd();
    
    // Verga da porta do quarto
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 2.1f, -7.0f);
    glVertex3f(-3.0f, 2.1f, -5.0f);
    glVertex3f(-3.0f, 3.0f, -5.0f);
    glVertex3f(-3.0f, 3.0f, -7.0f);
    glEnd();
    
    // Parede traseira
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(3.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, -10.0f);
    glVertex3f(3.0f, 3.0f, -10.0f);
    glEnd();
    
    // Parede direita
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -3.0f);
    glVertex3f(10.0f, 3.0f, -3.0f);
    glVertex3f(10.0f, 3.0f, -10.0f);
    glEnd();
    
    // Parede frontal
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(3.0f, 0.0f, -3.0f);
    glVertex3f(3.0f, 3.0f, -3.0f);
    glVertex3f(10.0f, 3.0f, -3.0f);
    glVertex3f(10.0f, 0.0f, -3.0f);
    glEnd();
    
    // Parede esquerda com porta - seção inferior
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 0.0f, -10.0f);
    glVertex3f(3.0f, 3.0f, -10.0f);
    glVertex3f(3.0f, 3.0f, -7.0f);
    glVertex3f(3.0f, 0.0f, -7.0f);
    glEnd();
    
    // Parede esquerda com porta - seção superior 
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 0.0f, -5.0f);
    glVertex3f(3.0f, 3.0f, -5.0f);
    glVertex3f(3.0f, 3.0f, -3.0f);
    glVertex3f(3.0f, 0.0f, -3.0f);
    glEnd();
    
    // Verga da porta da cozinha
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 2.1f, -7.0f);
    glVertex3f(3.0f, 3.0f, -7.0f);
    glVertex3f(3.0f, 3.0f, -5.0f);
    glVertex3f(3.0f, 2.1f, -5.0f);
    glEnd();

    // Parede frontal
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 8.5f);
    glVertex3f(-10.0f, 3.0f, 8.5f);
    glVertex3f(-3.0f, 3.0f, 8.5f);
    glVertex3f(-3.0f, 0.0f, 8.5f);
    glEnd();
    
    // Parede esquerda
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(-10.0f, 0.0f, 3.0f);
    glVertex3f(-10.0f, 3.0f, 3.0f);
    glVertex3f(-10.0f, 3.0f, 8.5f);
    glVertex3f(-10.0f, 0.0f, 8.5f);
    glEnd();
    
    // Parede traseira
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-10.0f, 0.0f, 3.0f);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glVertex3f(-3.0f, 3.0f, 3.0f);
    glVertex3f(-10.0f, 3.0f, 3.0f);
    glEnd();
    
    // Parede direita com porta - seção inferior
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glVertex3f(-3.0f, 0.0f, 5.0f);
    glVertex3f(-3.0f, 3.0f, 5.0f);
    glVertex3f(-3.0f, 3.0f, 3.0f);
    glEnd();
    
    // Verga da porta da sala de munição
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 2.1f, 5.0f);
    glVertex3f(-3.0f, 2.1f, 7.5f);
    glVertex3f(-3.0f, 3.0f, 7.5f);
    glVertex3f(-3.0f, 3.0f, 5.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-3.0f, 0.0f, 7.5f);
    glVertex3f(-3.0f, 0.0f, 8.5f);
    glVertex3f(-3.0f, 3.0f, 8.5f);
    glVertex3f(-3.0f, 3.0f, 7.5f);
    glEnd();
    
    // Parede frontal
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(3.0f, 0.0f, 8.5f);
    glVertex3f(3.0f, 3.0f, 8.5f);
    glVertex3f(10.0f, 3.0f, 8.5f);
    glVertex3f(10.0f, 0.0f, 8.5f);
    glEnd();
    
    // Parede direita
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(10.0f, 0.0f, 3.0f);
    glVertex3f(10.0f, 0.0f, 8.5f);
    glVertex3f(10.0f, 3.0f, 8.5f);
    glVertex3f(10.0f, 3.0f, 3.0f);
    glEnd();
    
    // Parede traseira
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(3.0f, 0.0f, 3.0f);
    glVertex3f(10.0f, 0.0f, 3.0f);
    glVertex3f(10.0f, 3.0f, 3.0f);
    glVertex3f(3.0f, 3.0f, 3.0f);
    glEnd();
    
    // Parede esquerda com porta
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 0.0f, 3.0f);
    glVertex3f(3.0f, 3.0f, 3.0f);
    glVertex3f(3.0f, 3.0f, 5.0f);
    glVertex3f(3.0f, 0.0f, 5.0f);
    glEnd();
    

    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 2.1f, 5.0f);
    glVertex3f(3.0f, 3.0f, 5.0f);
    glVertex3f(3.0f, 3.0f, 7.5f);
    glVertex3f(3.0f, 2.1f, 7.5f);
    glEnd();
    
    // Parede esquerda
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(3.0f, 0.0f, 7.5f);
    glVertex3f(3.0f, 3.0f, 7.5f);
    glVertex3f(3.0f, 3.0f, 8.5f);
    glVertex3f(3.0f, 0.0f, 8.5f);
    glEnd();
    
    // Desativar polygon offset após desenhar as paredes
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Cama menor e reposicionada
    glPushMatrix();
    glTranslatef(-7.0f, 0.3f, -7.0f);
    glColor3f(0.6f, 0.3f, 0.2f);
    glScalef(2.0f, 0.6f, 1.8f); 
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Mesa de cabeceira
    glPushMatrix();
    glTranslatef(-8.5f, 0.3f, -6.5f);
    glColor3f(0.4f, 0.35f, 0.3f);
    glScalef(0.5f, 0.6f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Armário no quarto
    glPushMatrix();
    glTranslatef(-9.5f, 1.0f, -9.0f);
    glColor3f(0.35f, 0.3f, 0.25f);
    glScalef(0.8f, 2.0f, 1.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Cadeira no quarto
    glPushMatrix();
    glTranslatef(-4.5f, 0.3f, -9.0f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(0.6f, 0.6f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Tapete (decorativo)
    glPushMatrix();
    glTranslatef(-7.0f, 0.01f, -6.5f);
    glColor3f(0.5f, 0.2f, 0.2f);
    glScalef(1.5f, 0.02f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // TV montada na parede do quarto
    drawTVP5(-9.4f, -4.5f, 90.0f);
    
    // Bancada menor
    glPushMatrix();
    glTranslatef(7.0f, 0.5f, -7.25f);
    glColor3f(0.5f, 0.45f, 0.4f);
    glScalef(2.0f, 1.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Geladeira menor
    glPushMatrix();
    glTranslatef(8.8f, 1.0f, -7.0f);
    glColor3f(0.8f, 0.8f, 0.85f);
    glScalef(0.7f, 2.0f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Mesa pequena
    glPushMatrix();
    glTranslatef(5.0f, 0.4f, -4.5f);
    glColor3f(0.45f, 0.4f, 0.35f);
    glScalef(1.2f, 0.1f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Fogão/Micro-ondas na bancada
    glPushMatrix();
    glTranslatef(7.0f, 0.9f, -7.5f);
    glColor3f(0.2f, 0.2f, 0.25f);
    glScalef(0.6f, 0.4f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Prateleira na parede
    glPushMatrix();
    glTranslatef(9.4f, 1.5f, -9.0f);
    glColor3f(0.4f, 0.35f, 0.3f);
    glScalef(0.2f, 0.8f, 1.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Caixas de suprimentos
    glPushMatrix();
    glTranslatef(4.5f, 0.3f, -9.5f);
    glColor3f(0.6f, 0.5f, 0.4f);
    glScalef(0.6f, 0.6f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Cadeiras ao redor da mesa
    glPushMatrix();
    glTranslatef(5.8f, 0.3f, -4.5f);
    glColor3f(0.25f, 0.25f, 0.3f);
    glScalef(0.4f, 0.4f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(4.2f, 0.3f, -4.5f);
    glColor3f(0.25f, 0.25f, 0.3f);
    glScalef(0.4f, 0.4f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    drawWeaponRackP5(-8.5f, 7.5f, 90.0f);
    
    // Caixas de munição empilhadas
    drawAmmoBoxP5(-7.5f, 5.5f);
    drawAmmoBoxP5(-7.0f, 5.5f);
    drawAmmoBoxP5(-7.3f, 5.85f);
    drawAmmoBoxP5(-8.0f, 4.5f);
    drawAmmoBoxP5(-6.5f, 4.5f);
    
    // Mesa de trabalho/limpeza
    glPushMatrix();
    glTranslatef(-9.0f, 0.5f, 7.5f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(1.5f, 0.15f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Caixas extras de munição no canto frontal
    glPushMatrix();
    glTranslatef(-9.5f, 0.3f, 7.5f);
    glColor3f(0.4f, 0.35f, 0.2f);
    glScalef(0.6f, 0.6f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-9.0f, 0.3f, 4.5f);
    glColor3f(0.35f, 0.3f, 0.2f);
    glScalef(0.5f, 0.6f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Caixa adicional na área expandida
    glPushMatrix();
    glTranslatef(-5.0f, 0.3f, 7.5f);
    glColor3f(0.38f, 0.33f, 0.22f);
    glScalef(0.5f, 0.5f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Ferramentas na mesa de trabalho
    glPushMatrix();
    glTranslatef(-9.2f, 0.6f, 7.5f);
    glColor3f(0.6f, 0.6f, 0.6f);
    glScalef(0.2f, 0.05f, 0.15f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-8.7f, 0.6f, 7.3f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glScalef(0.15f, 0.05f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    drawControlConsoleP5(7.0f, 7.8f, 180.0f);  // Movido mais para frente
    drawCommandChairP5(6.5f, 6.8f, 180.0f);    // Cadeira próxima ao console
    
    // Sofá para descanso da tripulação
    drawSofaP5(5.5f, 5.8f, 180.0f);
    
    // Estante com equipamentos
    glPushMatrix();
    glTranslatef(8.5f, 1.0f, 7.8f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(0.8f, 2.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Monitor auxiliar na parede
    glPushMatrix();
    glTranslatef(9.4f, 1.3f, 4.5f);
    glColor3f(0.15f, 0.15f, 0.2f);
    glScalef(0.2f, 0.8f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Caixa de ferramentas
    glPushMatrix();
    glTranslatef(6.0f, 0.2f, 6.0f);
    glColor3f(0.6f, 0.2f, 0.2f);
    glScalef(0.8f, 0.4f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Bancos laterais no corredor
    glPushMatrix();
    glTranslatef(-4.0f, 0.25f, -2.0f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(0.8f, 0.5f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Bancos laterais no corredor
    glPushMatrix();
    glTranslatef(4.0f, 0.25f, -2.0f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(0.8f, 0.5f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Vasos com plantas no corredor
    // Vaso 1 - lado esquerdo
    glPushMatrix();
    glTranslatef(-2.5f, 0.0f, 0.0f);
    // Vaso (marrom)
    glColor3f(0.4f, 0.25f, 0.1f);
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(0.3f, 0.5f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Planta do vaso 1 (verde)
    glPushMatrix();
    glTranslatef(-2.5f, 0.7f, 0.0f);
    glColor3f(0.1f, 0.6f, 0.1f);
    glScalef(0.4f, 0.6f, 0.4f);
    glutSolidSphere(0.3f, 8, 8);
    glPopMatrix();
    
    // Vaso 2 - lado direito
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, 0.0f);
    // Vaso (marrom)
    glColor3f(0.4f, 0.25f, 0.1f);
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(0.3f, 0.5f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Planta do vaso 2 (verde)
    glPushMatrix();
    glTranslatef(2.5f, 0.7f, 0.0f);
    glColor3f(0.1f, 0.6f, 0.1f);
    glScalef(0.4f, 0.6f, 0.4f);
    glutSolidSphere(0.3f, 8, 8);
    glPopMatrix();
    
    // Vaso 3 - centro, próximo à parte traseira do corredor
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.0f);
    // Vaso (marrom)
    glColor3f(0.4f, 0.25f, 0.1f);
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(0.3f, 0.5f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Planta do vaso 3 (verde mais escuro)
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, -2.0f);
    glColor3f(0.08f, 0.5f, 0.08f);
    glScalef(0.4f, 0.6f, 0.4f);
    glutSolidSphere(0.3f, 8, 8);
    glPopMatrix();
    
    for (int i = 0; i < numDepositsP5; i++) {
        glPushMatrix();
        glTranslatef(depositsP5[i].x, 1.25f, depositsP5[i].z);
        
        if (depositsP5[i].count >= 2) {
            glColor3f(0.2f, 0.8f, 0.2f); // Verde quando cheia (2 objetos)
        } else {
            
            switch(depositsP5[i].requiredType) {
                case SHAPE_CUBE: glColor3f(0.8f, 0.2f, 0.2f); break;
                case SHAPE_SPHERE: glColor3f(1.0f, 0.5f, 0.0f); break; // Laranja
                case SHAPE_PYRAMID: glColor3f(0.2f, 0.2f, 0.8f); break;
                case SHAPE_CYLINDER: glColor3f(0.8f, 0.8f, 0.2f); break;
            }
        }
        
        // Slot retangular no painel (horizontal - plataforma no topo do console)
        glPushMatrix();
        glScalef(0.6f, 0.05f, 0.6f); // Achatado horizontalmente
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Texto indicando qual forma colocar e quantidade
        glColor3f(1.0f, 1.0f, 1.0f);
        char labelText[32];
        const char* shapeName = "";
        switch(depositsP5[i].requiredType) {
            case SHAPE_CUBE: shapeName = "CUBO"; break;
            case SHAPE_SPHERE: shapeName = "ESFERA"; break;
            case SHAPE_PYRAMID: shapeName = "PIRAMIDE"; break;
            case SHAPE_CYLINDER: shapeName = "CILINDRO"; break;
        }
        sprintf(labelText, "%s %d/2", shapeName, depositsP5[i].count);
        drawText3DP5(0.0f, 0.3f, 0.0f, labelText, GLUT_BITMAP_HELVETICA_12);
        
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    
    // Desenhar objetos depositados nas plataformas
    for (int i = 0; i < numObjectsP5; ++i) {
        if (objectsP5[i].collected && !objectsP5[i].held) {
            glPushMatrix();
            glTranslatef(objectsP5[i].x, objectsP5[i].y, objectsP5[i].z); // Usar Y armazenado
            glScalef(0.18f, 0.18f, 0.18f);
            
            switch (objectsP5[i].type) {
                case SHAPE_CUBE: 
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureCubeP5);
                    glColor3f(1.0, 1.0, 1.0);
                    glutSolidCube(1.0);
                    glDisable(GL_TEXTURE_2D);
                    break;
                case SHAPE_SPHERE: 
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureSphereP5);
                    glColor3f(1.0, 1.0, 1.0);
                    glutSolidSphere(0.6, 20, 20);
                    glDisable(GL_TEXTURE_2D);
                    break;
                case SHAPE_PYRAMID:
                    glScalef(0.5f, 0.5f, 0.5f);
                    drawPyramidP5(); 
                    break;
                case SHAPE_CYLINDER: 
                    glTranslatef(0.0f, -0.5f, 0.0f);
                    drawCylinderP5(0.5, 1.0, 20); 
                    break;
            }
            glPopMatrix();
        }
    }

    // Desenhar objetos coletáveis
    for (int i = 0; i < numObjectsP5; ++i) {
        if (!objectsP5[i].collected && !objectsP5[i].held) {
            glPushMatrix();
            glTranslatef(objectsP5[i].x, 0.13f, objectsP5[i].z); // Altura fixa no chão
            
            glScalef(0.25f, 0.25f, 0.25f);
            
            switch (objectsP5[i].type) {
                case SHAPE_CUBE: 
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureCubeP5);
                    glColor3f(1.0, 1.0, 1.0);
                    glutSolidCube(1.0);
                    glDisable(GL_TEXTURE_2D);
                    break;
                case SHAPE_SPHERE: 
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureSphereP5);
                    glColor3f(1.0, 1.0, 1.0);
                    glutSolidSphere(0.6, 20, 20);
                    glDisable(GL_TEXTURE_2D);
                    break;
                case SHAPE_PYRAMID:
                    glScalef(0.5f, 0.5f, 0.5f);
                    drawPyramidP5(); 
                    break;
                case SHAPE_CYLINDER: 
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
            case SHAPE_CUBE: 
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureCubeP5);
                glColor3f(1.0, 1.0, 1.0);
                glutSolidCube(1.0);
                glDisable(GL_TEXTURE_2D);
                break;
            case SHAPE_SPHERE: 
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureSphereP5);
                glColor3f(1.0, 1.0, 1.0);
                glutSolidSphere(0.6, 20, 20);
                glDisable(GL_TEXTURE_2D);
                break;
            case SHAPE_PYRAMID:
                glScalef(0.5f, 0.5f, 0.5f);
                drawPyramidP5(); 
                break;
            case SHAPE_CYLINDER: 
                glTranslatef(0.0f, -0.5f, 0.0f);
                drawCylinderP5(0.5, 1.0, 20); 
                break;
        }
        glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    char text[100];
    
    // Display timer
    if (timeRemainingP5 <= 5.0f && !phase5_won && !gameOverP5) {
        glColor3f(1.0, 0.0, 0.0); 
    } else {
        glColor3f(0.0, 1.0, 1.0);
    }
    sprintf(text, "Tempo: %.1f s | Objetos: %d/8", timeRemainingP5, scoreP5);
    drawTextP5(10, windowHeight - 20, text);

    if (phase5_won) {
    } else if (heldObjectIndex >= 0) {
        glColor3f(1.0, 1.0, 0.3);
        const char* heldName = "";
        switch(objectsP5[heldObjectIndex].type) {
            case SHAPE_CUBE: heldName = "Segurando: CUBO - Leve ao slot VERMELHO no painel"; break;
            case SHAPE_SPHERE: heldName = "Segurando: ESFERA - Leve ao slot LARANJA no painel"; break;
            case SHAPE_PYRAMID: heldName = "Segurando: PIRAMIDE - Leve ao slot AZUL no painel"; break;
            case SHAPE_CYLINDER: heldName = "Segurando: CILINDRO - Leve ao slot AMARELO no painel"; break;
        }
        drawTextP5(10, windowHeight - 45, heldName);
    } else {
        glColor3f(1.0, 1.0, 1.0);
        drawTextP5(10, windowHeight - 45, "'E' para pegar | Explore os comodos da nave | Deposite no painel frontal");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Desenhar contagem regressiva se ativa
    if (showCountdownP5 && countdownValueP5 > 0) {
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
        sprintf(countText, "%d", countdownValueP5);
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
    
    // Desenhar tela de game over se necessário
    drawGameOver();
}

// Verificar colisão com obstáculos (paredes da nave e móveis)
bool checkCollisionP5(float x, float z) {
    float radius = 0.3f; // Raio de colisão do jogador
    
    // PAREDES EXTERNAS da nave (limites -10 a 10)
    if (x < -9.5f + radius || x > 9.5f - radius || z < -9.5f + radius || z > 9.5f - radius) return true;
    
    // PAREDES DO CÔMODO 1: QUARTO (X=-10 a -3, Z=-10 a -3)
    // Porta em X=-3, Z=-7 a -5
    // Parede direita com porta (X=-3) - seção 1: Z=-10 a Z=-7
    if (fabs(x + 3.0f) < 0.15f && z > -10.0f - radius && z < -7.0f + radius) return true;
    // Parede direita com porta (X=-3) - seção 2: Z=-5 a Z=-3
    if (fabs(x + 3.0f) < 0.15f && z > -5.0f - radius && z < -3.0f + radius) return true;
    // Parede frontal (Z=-3) do quarto
    if (fabs(z + 3.0f) < 0.15f && x < -3.0f + radius && x > -10.0f - radius) return true;
    // Parede esquerda (X=-10) do quarto
    if (fabs(x + 10.0f) < 0.15f && z < -3.0f + radius && z > -10.0f - radius) return true;
    // Parede traseira (Z=-10) do quarto
    if (fabs(z + 10.0f) < 0.15f && x > -10.0f - radius && x < -3.0f + radius) return true;
    
    // PAREDES DO CÔMODO 2: COZINHA (X=3 a 10, Z=-10 a -3)
    // Porta em X=3, Z=-7 a -5
    // Parede esquerda com porta (X=3) - seção 1: Z=-10 a Z=-7
    if (fabs(x - 3.0f) < 0.15f && z > -10.0f - radius && z < -7.0f + radius) return true;
    // Parede esquerda com porta (X=3) - seção 2: Z=-5 a Z=-3
    if (fabs(x - 3.0f) < 0.15f && z > -5.0f - radius && z < -3.0f + radius) return true;
    // Parede frontal (Z=-3) da cozinha
    if (fabs(z + 3.0f) < 0.15f && x > 3.0f - radius && x < 10.0f + radius) return true;
    // Parede direita (X=10) da cozinha
    if (fabs(x - 10.0f) < 0.15f && z < -3.0f + radius && z > -10.0f - radius) return true;
    // Parede traseira (Z=-10) da cozinha
    if (fabs(z + 10.0f) < 0.15f && x < 10.0f + radius && x > 3.0f - radius) return true;
    
    // PAREDES DO CÔMODO 3: SALA DE MUNIÇÃO (X=-10 a -3, Z=3 a 8.5)
    // Porta em X=-3, Z=5 a 6.5 (sem colisão na porta)
    // Parede direita com porta (X=-3) - seção 1: Z=3 a Z=5
    if (fabs(x + 3.0f) < 0.15f && z >= 3.0f - radius && z < 5.0f) return true;
    // Parede direita com porta (X=-3) - seção 2: Z=6.5 a Z=8.5
    if (fabs(x + 3.0f) < 0.15f && z > 6.5f && z <= 8.5f + radius) return true;
    // Parede traseira (Z=3) da sala de munição
    if (fabs(z - 3.0f) < 0.15f && x < -3.0f + radius && x > -10.0f - radius) return true;
    // Parede esquerda (X=-10) da sala de munição
    if (fabs(x + 10.0f) < 0.15f && z > 3.0f - radius && z < 8.5f + radius) return true;
    // Parede frontal (Z=8.5) da sala de munição
    if (fabs(z - 8.5f) < 0.15f && x > -10.0f - radius && x < -3.0f + radius) return true;
    
    // PAREDES DO CÔMODO 4: ÁREA DE CONSOLE (X=3 a 10, Z=3 a 8.5)
    // Porta em X=3, Z=5 a 6.5 (sem colisão na porta)
    // Parede esquerda com porta (X=3) - seção 1: Z=3 a Z=5
    if (fabs(x - 3.0f) < 0.15f && z >= 3.0f - radius && z < 5.0f) return true;
    // Parede esquerda com porta (X=3) - seção 2: Z=6.5 a Z=8.5
    if (fabs(x - 3.0f) < 0.15f && z > 6.5f && z <= 8.5f + radius) return true;
    // Parede traseira (Z=3) da área de console
    if (fabs(z - 3.0f) < 0.15f && x > 3.0f - radius && x < 10.0f + radius) return true;
    // Parede direita (X=10) da área de console
    if (fabs(x - 10.0f) < 0.15f && z > 3.0f - radius && z < 8.5f + radius) return true;
    // Parede frontal (Z=8.5) da área de console
    if (fabs(z - 8.5f) < 0.15f && x < 10.0f + radius && x > 3.0f - radius) return true;
    
    // MÓVEIS E OBJETOS - QUARTO
    // CAMA no quarto
    if (x > -8.0f - radius && x < -6.0f + radius && z > -8.0f - radius && z < -6.0f + radius) return true;
    // Mesa de cabeceira
    if (fabs(x + 8.5f) < 0.5f && fabs(z + 6.5f) < 0.5f) return true;
    // Armário
    if (fabs(x + 9.5f) < 0.6f && fabs(z + 9.0f) < 0.8f) return true;
    // Cadeira
    if (fabs(x + 4.5f) < 0.4f && fabs(z + 9.0f) < 0.4f) return true;
    
    // MÓVEIS E OBJETOS - COZINHA
    // BANCADA DA COZINHA
    if (x > 6.0f - radius && x < 8.0f + radius && z > -8.0f - radius && z < -6.5f + radius) return true;
    // GELADEIRA
    if (x > 8.3f - radius && x < 9.3f + radius && z > -7.5f - radius && z < -6.5f + radius) return true;
    // Mesa pequena
    if (fabs(x - 5.0f) < 0.7f && fabs(z + 4.5f) < 0.5f) return true;
    // Prateleira (colada na parede, não precisa colisão forte)
    // Caixas de suprimentos
    if (fabs(x - 4.5f) < 0.4f && fabs(z + 9.5f) < 0.4f) return true;
    
    // MÓVEIS E OBJETOS - SALA DE MUNIÇÃO
    // CAIXAS DE MUNIÇÃO empilhadas (múltiplas caixas agrupadas)
    if (x > -8.5f - radius && x < -6.5f + radius && z > 4.5f - radius && z < 6.0f + radius) return true;
    // Rack de armas (X=-8.5, Z=7.5) - movido para frente
    if (fabs(x + 8.5f) < 0.6f && fabs(z - 7.5f) < 0.8f) return true;
    // Mesa de trabalho (X=-9.0, Z=7.5, escala 1.5x0.8) - movida para frente
    if (x > -9.8f - radius && x < -8.2f + radius && z > 7.0f - radius && z < 8.0f + radius) return true;
    // Caixas extras no canto frontal e traseiro
    if (fabs(x + 9.5f) < 0.5f && fabs(z - 7.5f) < 0.5f) return true;
    if (fabs(x + 9.0f) < 0.4f && fabs(z - 4.5f) < 0.5f) return true;
    if (fabs(x + 5.0f) < 0.4f && fabs(z - 7.5f) < 0.4f) return true;
    
    // MÓVEIS E OBJETOS - ÁREA DE CONSOLE
    // CONSOLE AUXILIAR e CADEIRA - removidos colisão para não bloquear entrada da sala
    // Estante (X=8.5, Z=7.8) - na área frontal
    if (x > 8.0f - radius && x < 9.0f + radius && z > 7.3f - radius && z < 8.3f + radius) return true;
    // SOFÁ - removido colisão para não bloquear acesso à porta
    // ESTANTE com equipamentos (X=8.5, Z=7.0, escala 0.8x1.5)
    if (x > 8.0f - radius && x < 9.0f + radius && z > 6.2f - radius && z < 7.8f + radius) return true;
    // Caixa de ferramentas - removido colisão para não bloquear acesso à porta
    
    // OBJETOS DO CORREDOR
    // Bancos laterais
    if (fabs(x + 4.0f) < 0.6f && fabs(z + 2.0f) < 0.7f) return true;
    if (fabs(x - 4.0f) < 0.6f && fabs(z + 2.0f) < 0.7f) return true;
    // Colunas decorativas
    if (fabs(x + 4.5f) < 0.3f && fabs(z - 3.0f) < 0.3f) return true;
    if (fabs(x - 4.5f) < 0.3f && fabs(z - 3.0f) < 0.3f) return true;
    // Caixas de armazenamento
    if (fabs(x + 3.5f) < 0.5f && fabs(z - 4.5f) < 0.5f) return true;
    if (fabs(x - 3.5f) < 0.5f && fabs(z - 4.5f) < 0.5f) return true;
    
    // PAINEL DE CONTROLE PRINCIPAL e CADEIRA (centro frontal)
    if (fabs(x) < 0.6f && z > 7.0f - radius && z < 8.0f + radius) return true;
    // CADEIRA
    if (fabs(x) < 0.4f && z > 5.5f - radius && z < 6.5f + radius) return true;
    
    return false;
}

void updatePhase5(int value) {
    (void)value;
    
    // Parar o timer se não estiver mais na fase 5
    if (currentState != PHASE5_SCREEN) {
        return;
    }
    
    if (getPaused()) {
        glutTimerFunc(16, updatePhase5, 0);
        return;
    }
    
    // Update countdown
    if (showCountdownP5) {
        countdownTimerP5++;
        if (countdownTimerP5 >= 60) {
            countdownTimerP5 = 0;
            countdownValueP5--;
            if (countdownValueP5 <= 0) {
                showCountdownP5 = false;
            }
        }
        glutPostRedisplay();
        glutTimerFunc(16, updatePhase5, 0);
        return;
    }
    
    float speed = 0.05f; // Velocidade igual à fase 7
    float lookX = sin(playerP5.angle);
    float lookZ = -cos(playerP5.angle);

    // Update timer
    if (!phase5_won && !gameOverP5) {
        timeRemainingP5 -= 0.016f; // ~16ms per frame
        if (timeRemainingP5 <= 0.0f) {
            timeRemainingP5 = 0.0f;
            gameOverP5 = true;
            printf("Tempo esgotado! Game Over.\n");
            if (currentState == PHASE5_SCREEN) {
                Audio::getInstance().play(Audio::SOUND_ERROR);
            }
            setGameOver(true);
            setVictory(false);
        }
    }

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

    if (!phase5_won && !gameOverP5) {
        // Se estiver segurando um objeto, verificar se está perto de uma zona de depósito
        if (heldObjectIndex >= 0) {
            // Encontrar a plataforma MAIS PRÓXIMA
            int closestDeposit = -1;
            float closestDist = 999.0f;
            for (int i = 0; i < numDepositsP5; i++) {
                if (depositsP5[i].count < 2) {
                    float dist = sqrt(pow(playerP5.x - depositsP5[i].x, 2) + pow(playerP5.z - depositsP5[i].z, 2));
                    if (dist < 2.0f && dist < closestDist) {
                        closestDist = dist;
                        closestDeposit = i;
                    }
                }
            }
            
            // Se encontrou uma plataforma próxima, verificar ao pressionar E
            if (closestDeposit >= 0 && (keyStateP5['e'] || keyStateP5['E'])) {
                if (objectsP5[heldObjectIndex].type == depositsP5[closestDeposit].requiredType) {
                    // Verificar se a base ainda tem espaço (máximo 2 objetos)
                    if (depositsP5[closestDeposit].count < 2) {
                        // Correto! Posicionar objeto na plataforma
                        printf("Objeto depositado corretamente! Score: %d\n", scoreP5 + 1);
                        
                        float xOffset = depositsP5[closestDeposit].count == 0 ? -0.15f : 0.15f;
                        
                        depositsP5[closestDeposit].count++;
                        objectsP5[heldObjectIndex].x = depositsP5[closestDeposit].x + xOffset;
                        objectsP5[heldObjectIndex].y = 1.35f;
                        objectsP5[heldObjectIndex].z = depositsP5[closestDeposit].z;
                        objectsP5[heldObjectIndex].collected = true;
                        objectsP5[heldObjectIndex].held = false;
                        scoreP5++;
                        Audio::getInstance().play(Audio::SOUND_VICTORY);
                        heldObjectIndex = -1;
                        
                        // Verificar vitória
                        if (scoreP5 >= 8) {
                            printf("Fase 5 completa! Indo para Fase 6...\n");
                            phase5_won = true;
                            gameOverP5 = true;
                            showStoryForPhase(6);
                            return; // Sair imediatamente
                        }
                        
                        // Reset timer on successful deposit
                        timeRemainingP5 = 30.0f;
                        printf("Cronometro reiniciado! Tempo: 30 segundos\n");
                    } else {
                        printf("Esta base já está cheia (2 objetos)!\n");
                        Audio::getInstance().play(Audio::SOUND_ERROR);
                    }
                } else {
                    // Errado - tipo incorreto
                    printf("Objeto errado! Voce tem: %d, plataforma quer: %d\n", 
                           objectsP5[heldObjectIndex].type, depositsP5[closestDeposit].requiredType);
                    Audio::getInstance().play(Audio::SOUND_ERROR);
                }
                keyStateP5['e'] = false;
                keyStateP5['E'] = false;
            }
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, updatePhase5, 0);
}

void handlePhase5Keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    
    // ESC para pausar
    if (key == 27) {
        if (!getGameOver() && !getPaused()) {
            setPaused(true, 5);
        } else if (getPaused()) {
            handlePauseKeyboard(key);
        }
        glutPostRedisplay();
        return;
    }
    
    // Se está pausado, tratar teclas de pausa
    if (getPaused()) {
        handlePauseKeyboard(key);
        glutPostRedisplay();
        return;
    }
    
    // Tratar entrada da tela de game over
    if (getGameOver()) {
        handleGameOverKeyboard(key);
        return;
    }
    
    keyStateP5[key] = true;
    
    // Pegar/soltar objeto com 'E'
    if (key == 'e' || key == 'E') {
        if (heldObjectIndex >= 0) {
            // Encontrar a plataforma MAIS PRÓXIMA
            bool deposited = false;
            int closestDeposit = -1;
            float closestDist = 999.0f;
            for (int i = 0; i < numDepositsP5; i++) {
                if (depositsP5[i].count < 2) {
                    float dist = sqrt(pow(playerP5.x - depositsP5[i].x, 2) + pow(playerP5.z - depositsP5[i].z, 2));
                    if (dist < 2.0f && dist < closestDist) {
                        closestDist = dist;
                        closestDeposit = i;
                    }
                }
            }
            
            // Se encontrou uma plataforma próxima
            if (closestDeposit >= 0) {
                if (objectsP5[heldObjectIndex].type == depositsP5[closestDeposit].requiredType) {
                    // Verificar se a base ainda tem espaço (máximo 2 objetos)
                    if (depositsP5[closestDeposit].count < 2) {
                        // Correto! Posicionar objeto na plataforma
                        printf("Objeto depositado corretamente! Score: %d\n", scoreP5 + 1);
                        
                        float xOffset = depositsP5[closestDeposit].count == 0 ? -0.15f : 0.15f;
                        
                        depositsP5[closestDeposit].count++;
                        objectsP5[heldObjectIndex].x = depositsP5[closestDeposit].x + xOffset;
                        objectsP5[heldObjectIndex].y = 1.35f;
                        objectsP5[heldObjectIndex].z = depositsP5[closestDeposit].z;
                        objectsP5[heldObjectIndex].collected = true;
                        objectsP5[heldObjectIndex].held = false;
                        scoreP5++;
                        Audio::getInstance().play(Audio::SOUND_VICTORY);
                        heldObjectIndex = -1;
                        deposited = true;
                        
                        // Verificar vitória
                        if (scoreP5 >= 8) {
                            printf("Fase 5 completa! Indo para Fase 6...\n");
                            phase5_won = true;
                            gameOverP5 = true;
                            showStoryForPhase(6);
                            return; // Sair imediatamente
                        }
                        
                        // Reset timer on successful deposit
                        timeRemainingP5 = 30.0f;
                        printf("Cronometro reiniciado! Tempo: 30 segundos\n");
                    } else {
                        printf("Esta base já está cheia (2 objetos)!\n");
                        Audio::getInstance().play(Audio::SOUND_ERROR);
                    }
                } else {
                    // Errado - tipo incorreto
                    printf("Objeto errado! Voce tem: %d, plataforma quer: %d\n", 
                           objectsP5[heldObjectIndex].type, depositsP5[closestDeposit].requiredType);
                    Audio::getInstance().play(Audio::SOUND_ERROR);
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
        } else if (!gameOverP5 && !phase5_won) {
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
        lastMouseYP5 = y;
        return;
    }
    int dx = x - lastMouseXP5;
    int dy = y - lastMouseYP5;
    lastMouseXP5 = x;
    lastMouseYP5 = y;
    
    float sensitivity = 0.01f;
    playerP5.angle += dx * sensitivity;
    playerP5.pitch -= dy * sensitivity; // Inverter para controle natural
    
    // Limitar pitch para evitar gimbal lock (-85° a +85°)
    float maxPitch = 1.48f; // ~85 graus em radianos
    if (playerP5.pitch > maxPitch) playerP5.pitch = maxPitch;
    if (playerP5.pitch < -maxPitch) playerP5.pitch = -maxPitch;
}

void returnToMenuFromPhase5() {
    printf("Voltando ao menu principal da Fase 5...\n");
    setGameOver(false);
    setVictory(false);
    setPaused(false, 0);
    setCurrentPhase(0);
    glutPostRedisplay();
}
