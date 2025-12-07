#include "phase7.h"
#include "audio.h"
#include "gameover.h"
#include "menu.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===================================================================
// Estrutura para modelo OBJ
// ===================================================================

struct Vec3 {
    float x, y, z;
};

struct Face {
    int v[3]; // Índices dos vértices
    int vt[3]; // Índices de textura
    int vn[3]; // Índices de normais
};

struct Vec2 {
    float u, v;
};

std::vector<Vec3> objVertices;
std::vector<Vec3> objNormals;
std::vector<Vec2> objTexCoords;
std::vector<Face> objFaces;
bool objLoaded = false;

// Material do MTL
struct Material {
    float Ka[3]; // Ambient
    float Kd[3]; // Diffuse
    float Ks[3]; // Specular
    float Ns;    // Shininess
} objMaterial;

// Texturas
GLuint monsterTextureID = 0;
GLuint textureWallP7 = 0;
GLuint textureFloorP7 = 0;
GLuint textureProjectileP7 = 0;
bool texturesLoadedP7 = false;

// ===================================================================
// Variáveis Globais
// ===================================================================

PlayerP7 playerP7;
CollectibleP7 objectsP7[10];
DepositZoneP7 depositsP7[4];
int numObjectsP7;
int numDepositsP7;
int scoreP7;
bool phase7_won = false;
int heldObjectIndexP7 = -1;

int phase7_municao = 20;
int phase7_vida = 10;

EnemyP7 enemiesP7[5];
int numEnemiesP7 = 5;

#define MAX_PROJECTILES 20
ProjectileP7 enemyProjectiles[MAX_PROJECTILES];
int maxProjectilesP7 = MAX_PROJECTILES;

// Projéteis do jogador
ProjectileP7 playerProjectiles[MAX_PROJECTILES];

float timeRemainingP7 = 15.0f;
bool gameOverP7 = false;
bool phase7_gameOver = false; 

// Countdown variables
bool showCountdownP7 = false;
int countdownTimerP7 = 0;
int countdownValueP7 = 3;
bool showFindEnemyMsg = false;
int findEnemyMsgTimer = 0;

bool keyStateP7[256];
int lastMouseXP7;
int lastMouseYP7;

// ===================================================================
// Funções para carregar e desenhar modelo OBJ
// ===================================================================

void loadOBJ(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir arquivo OBJ: %s\n", filename);
        objLoaded = false;
        return;
    }
    
    objVertices.clear();
    objNormals.clear();
    objTexCoords.clear();
    objFaces.clear();
    
    // Valores do material Oozey.mtl
    objMaterial.Ka[0] = 1.0f; objMaterial.Ka[1] = 1.0f; objMaterial.Ka[2] = 1.0f;
    objMaterial.Kd[0] = 0.2f; objMaterial.Kd[1] = 0.8f; objMaterial.Kd[2] = 0.3f; // Verde
    objMaterial.Ks[0] = 0.073f; objMaterial.Ks[1] = 0.072f; objMaterial.Ks[2] = 0.038f;
    objMaterial.Ns = 45.0f;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            Vec3 v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            objVertices.push_back(v);
        }
        else if (line[0] == 'v' && line[1] == 'n') {
            Vec3 vn;
            sscanf(line, "vn %f %f %f", &vn.x, &vn.y, &vn.z);
            objNormals.push_back(vn);
        }
        else if (line[0] == 'v' && line[1] == 't') {
            Vec2 vt;
            sscanf(line, "vt %f %f", &vt.u, &vt.v);
            objTexCoords.push_back(vt);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            Face f;
            int v1, v2, v3, v4;
            int vt1, vt2, vt3, vt4;
            int vn1, vn2, vn3, vn4;
            
            // Tentar formato f v/vt/vn v/vt/vn v/vt/vn v/vt/vn (quad)
            int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                                &v1, &vt1, &vn1, &v2, &vt2, &vn2, 
                                &v3, &vt3, &vn3, &v4, &vt4, &vn4);
            if (matches == 12) {
                // Quad - dividir em 2 triângulos
                // Triângulo 1: v1, v2, v3
                f.v[0] = v1 - 1; f.v[1] = v2 - 1; f.v[2] = v3 - 1;
                f.vt[0] = vt1 - 1; f.vt[1] = vt2 - 1; f.vt[2] = vt3 - 1;
                f.vn[0] = vn1 - 1; f.vn[1] = vn2 - 1; f.vn[2] = vn3 - 1;
                objFaces.push_back(f);
                // Triângulo 2: v1, v3, v4
                f.v[0] = v1 - 1; f.v[1] = v3 - 1; f.v[2] = v4 - 1;
                f.vt[0] = vt1 - 1; f.vt[1] = vt3 - 1; f.vt[2] = vt4 - 1;
                f.vn[0] = vn1 - 1; f.vn[1] = vn3 - 1; f.vn[2] = vn4 - 1;
                objFaces.push_back(f);
            } else {
                // Tentar formato f v/vt/vn v/vt/vn v/vt/vn (triângulo)
                matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                                &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
                if (matches == 9) {
                    f.v[0] = v1 - 1; f.v[1] = v2 - 1; f.v[2] = v3 - 1;
                    f.vt[0] = vt1 - 1; f.vt[1] = vt2 - 1; f.vt[2] = vt3 - 1;
                    f.vn[0] = vn1 - 1; f.vn[1] = vn2 - 1; f.vn[2] = vn3 - 1;
                    objFaces.push_back(f);
                }
            }
        }
    }
    
    fclose(file);
    objLoaded = true;
    printf("Modelo OBJ carregado: %d vértices, %d faces\n", (int)objVertices.size(), (int)objFaces.size());
}

GLuint createProceduralTextureP7(int width, int height, int type) {
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
                case 1: // Piso xadrez
                    {
                        int checker = ((x / 16) % 2) ^ ((y / 16) % 2);
                        int base = checker ? 80 : 100;
                        data[index] = base;
                        data[index + 1] = base;
                        data[index + 2] = base;
                    }
                    break;
                case 2: // Projétil (energia amarela/laranja)
                    {
                        float cx = width / 2.0f;
                        float cy = height / 2.0f;
                        float dist = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
                        float maxDist = width / 2.0f;
                        float intensity = 1.0f - (dist / maxDist);
                        if (intensity < 0) intensity = 0;
                        data[index] = (unsigned char)(255 * intensity);
                        data[index + 1] = (unsigned char)(200 * intensity);
                        data[index + 2] = (unsigned char)(50 * intensity);
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

// Carregar textura PNG
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (!data) {
        printf("Erro ao carregar textura: %s\n", filename);
        return 0;
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Configurar parâmetros da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Carregar dados da textura
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    printf("Textura carregada: %s (%dx%d, %d canais)\n", filename, width, height, channels);
    
    return textureID;
}

void drawOBJ(float scale) {
    if (!objLoaded) return;
    
    // Habilitar textura se disponível
    if (monsterTextureID > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, monsterTextureID);
        // Configurar como a textura interage com a iluminação
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < objFaces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            int idx_v = objFaces[i].v[j];
            int idx_vt = objFaces[i].vt[j];
            int idx_n = objFaces[i].vn[j];
            
            // Usar coordenadas de textura se disponíveis (inverter V para corrigir orientação)
            if (idx_vt >= 0 && idx_vt < (int)objTexCoords.size()) {
                glTexCoord2f(objTexCoords[idx_vt].u, 1.0f - objTexCoords[idx_vt].v);
            }
            
            // Usar normal do arquivo OBJ se disponível
            if (idx_n >= 0 && idx_n < (int)objNormals.size()) {
                glNormal3f(objNormals[idx_n].x, objNormals[idx_n].y, objNormals[idx_n].z);
            }
            
            // Desenhar vértice
            if (idx_v >= 0 && idx_v < (int)objVertices.size()) {
                glVertex3f(objVertices[idx_v].x * scale, 
                          objVertices[idx_v].y * scale, 
                          objVertices[idx_v].z * scale);
            }
        }
    }
    glEnd();
    
    if (monsterTextureID > 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

// ===================================================================
// Funções Auxiliares de Desenho (Copiadas e adaptadas da Fase 5)
// ===================================================================

void drawTextP7(float x, float y, const char *string) {
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawCylinderP7(float radius, float height, int sides) {
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

// --- Funções de Móveis (Trazidas da Fase 5) ---

void drawDoorP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix(); glScalef(1.2f, 2.2f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix(); glScalef(1.0f, 2.0f, 0.08f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawBedP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.3f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix(); glScalef(2.0f, 0.3f, 1.2f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.2f, 0.3f, 0.6f);
    glPushMatrix(); glTranslatef(0.0f, 0.25f, 0.0f); glScalef(1.9f, 0.2f, 1.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawKitchenCounterP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix(); glScalef(2.5f, 1.0f, 0.8f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.7f, 0.7f, 0.8f);
    glPushMatrix(); glTranslatef(-0.5f, 0.4f, 0.0f); glScalef(0.6f, 0.2f, 0.5f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawSofaP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.25f, 0.25f, 0.3f);
    glPushMatrix(); glTranslatef(0.0f, 0.3f, 0.0f); glScalef(1.8f, 0.4f, 0.8f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.7f, -0.35f); glScalef(1.8f, 0.8f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.22f, 0.22f, 0.28f);
    glPushMatrix(); glTranslatef(-0.85f, 0.5f, 0.0f); glScalef(0.1f, 0.6f, 0.8f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.85f, 0.5f, 0.0f); glScalef(0.1f, 0.6f, 0.8f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawTVP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.1f, 0.1f, 0.1f); glPushMatrix(); glScalef(1.5f, 1.0f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    // Tela mais escura
    glColor3f(0.02f, 0.03f, 0.08f); glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.06f); glScalef(1.3f, 0.8f, 0.01f); glutSolidCube(1.0f); glPopMatrix();
    // Estrelas
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 20; i++) {
        float sx = ((i * 73) % 100 - 50) * 0.012f;
        float sy = ((i * 37) % 100 - 50) * 0.008f;
        glVertex3f(sx, sy, 0.065f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glColor3f(0.15f, 0.15f, 0.15f); glPushMatrix(); glTranslatef(0.0f, -0.6f, 0.0f); glScalef(0.3f, 0.2f, 0.2f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawAmmoBoxP7(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.3f, z);
    glColor3f(0.3f, 0.4f, 0.2f); glPushMatrix(); glScalef(0.6f, 0.4f, 0.4f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.8f, 0.7f, 0.0f); glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.21f); glScalef(0.3f, 0.05f, 0.01f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawWeaponRackP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 1.0f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.2f, 0.2f, 0.2f); glPushMatrix(); glScalef(0.1f, 2.0f, 1.5f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawControlConsoleP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.8f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.3f, 0.3f, 0.35f); glPushMatrix(); glScalef(1.5f, 0.8f, 0.6f); glutSolidCube(1.0f); glPopMatrix();
    glColor3f(0.1f, 0.3f, 0.5f); glPushMatrix(); glTranslatef(0.0f, 0.3f, 0.2f); glScalef(1.0f, 0.6f, 0.05f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawCommandChairP7(float x, float z, float rotation) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glRotatef(rotation, 0, 1, 0);
    glColor3f(0.2f, 0.2f, 0.3f); glPushMatrix(); glScalef(0.6f, 0.2f, 0.6f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.4f, -0.2f); glScalef(0.6f, 0.8f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

// ===================================================================
// Lógica de IA e Projéteis
// ===================================================================

void spawnEnemyProjectile(float x, float y, float z) {
    int slot = -1;
    for(int i=0; i<maxProjectilesP7; i++) {
        if(!enemyProjectiles[i].active) { slot = i; break; }
    }
    if (slot == -1) return;

    // Calcular direção 3D do projétil em direção ao jogador
    float dx = playerP7.x - x;
    float dy = playerP7.y - y;
    float dz = playerP7.z - z;
    float dist = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (dist > 0.1f) {
        enemyProjectiles[slot].x = x;
        enemyProjectiles[slot].y = y;
        enemyProjectiles[slot].z = z;
        enemyProjectiles[slot].startX = x;
        enemyProjectiles[slot].startY = y;
        enemyProjectiles[slot].startZ = z;
        enemyProjectiles[slot].dirX = dx / dist;
        enemyProjectiles[slot].dirY = dy / dist;
        enemyProjectiles[slot].dirZ = dz / dist;
        enemyProjectiles[slot].active = true;
    }
}

// Variáveis static para a IA do inimigo
static float enemyTargetX = 0.0f;
static float enemyTargetZ = 0.0f;
static int enemyFrameCounter = 0;

void resetEnemyAI() {
    enemyTargetX = 0.0f;
    enemyTargetZ = -5.0f;
    enemyFrameCounter = 0;
}

void updateEnemiesAI() {
    float moveSpeed = 0.02f; // Reduzido para ficar ainda mais lento

    for(int i=0; i<numEnemiesP7; i++) {
        if(!enemiesP7[i].active) continue;

        // A cada 120 frames (~2 segundos), decidir novo comportamento
        enemyFrameCounter++;
        if (enemyFrameCounter > 120) {
            // 40% de chance de perseguir o jogador, 60% de ir para posição aleatória
            if (rand() % 100 < 40) {
                // Perseguir jogador
                enemyTargetX = playerP7.x;
                enemyTargetZ = playerP7.z;
            } else {
                // Ir para posição aleatória
                enemyTargetX = ((rand() % 200) - 100) / 10.0f; // -10 a 10
                enemyTargetZ = ((rand() % 200) - 100) / 10.0f; // -10 a 10
            }
            enemyFrameCounter = 0;
        }

        // Mover em direção ao alvo
        float dx = enemyTargetX - enemiesP7[i].x;
        float dz = enemyTargetZ - enemiesP7[i].z;
        float dist = sqrt(dx*dx + dz*dz);

        if (dist > 0.5f) {
            enemiesP7[i].x += (dx / dist) * moveSpeed;
            enemiesP7[i].z += (dz / dist) * moveSpeed;
        }
        
        // Monstro sempre no chão
        enemiesP7[i].y = 0.0f;

        // Calcular distância ao jogador
        float pdx = playerP7.x - enemiesP7[i].x;
        float pdy = playerP7.y - enemiesP7[i].y;
        float pdz = playerP7.z - enemiesP7[i].z;
        float playerDist = sqrt(pdx*pdx + pdy*pdy + pdz*pdz);

        // Dano por contato se o monstro encostar no jogador
        if (playerDist < 2.0f) {
            static int contactDamageTimer = 0;
            contactDamageTimer++;
            if (contactDamageTimer >= 60) { // 1 dano por segundo
                phase7_vida--;
                contactDamageTimer = 0;
                if (phase7_vida <= 0) setGameOver(true);
            }
        }

        // Atirar no jogador se estiver próximo (projéteis atravessam paredes)
        if (playerDist < 15.0f) {
            enemiesP7[i].shootTimer++;
            if (enemiesP7[i].shootTimer > (120 + rand()%80)) { // Atira com menos frequência
                spawnEnemyProjectile(enemiesP7[i].x, enemiesP7[i].y, enemiesP7[i].z);
                enemiesP7[i].shootTimer = 0;
            }
        }
    }
}

void updateProjectiles() {
    float projSpeed = 0.15f;
    float playerHitRadius = 0.6f;
    float enemyHitRadius = 3.0f;  // Aumentado para cobrir todo o modelo (escala 0.5)

    // Atualizar projéteis do jogador
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!playerProjectiles[i].active) continue;

        // Mover projétil
        playerProjectiles[i].x += playerProjectiles[i].dirX * projSpeed * 2.0f; // Mais rápido
        playerProjectiles[i].y += playerProjectiles[i].dirY * projSpeed * 2.0f;
        playerProjectiles[i].z += playerProjectiles[i].dirZ * projSpeed * 2.0f;
        playerProjectiles[i].distanceTraveled += projSpeed * 2.0f;

        // Verificar colisão com inimigos
        for(int j=0; j<numEnemiesP7; j++) {
            if(!enemiesP7[j].active) continue;

            float dx = playerProjectiles[i].x - enemiesP7[j].x;
            float dy = playerProjectiles[i].y - enemiesP7[j].y;
            float dz = playerProjectiles[i].z - enemiesP7[j].z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);

            if (dist < enemyHitRadius) {
                playerProjectiles[i].active = false;
                enemiesP7[j].hp--;
                enemiesP7[j].y += 0.2f;

                if (enemiesP7[j].hp <= 0) {
                    enemiesP7[j].active = false;
                    scoreP7 += 100;
                    
                    // Som de explosão
                    Audio::getInstance().play(Audio::SOUND_EXPLOSION);
                    
                    bool anyAlive = false;
                    for(int k = 0; k < numEnemiesP7; k++) {
                        if (enemiesP7[k].active) { anyAlive = true; break; }
                    }
                    if (!anyAlive) {
                        phase7_won = true;
                        setVictory(true);
                        setVictoryPhase(7);
                        setGameOver(true);
                        
                        // Som de vitória quando o boss morre
                        Audio::getInstance().play(Audio::SOUND_VICTORY);
                    }
                }
                break;
            }
        }

        // Desativar após voar muito longe
        if (playerProjectiles[i].distanceTraveled > 50.0f) {
            playerProjectiles[i].active = false;
        }
    }

    // Atualizar projéteis dos inimigos
    for(int i=0; i<maxProjectilesP7; i++) {
        if(!enemyProjectiles[i].active) continue;

        // Projéteis do monstro atravessam paredes (sem verificação de colisão)
        enemyProjectiles[i].x += enemyProjectiles[i].dirX * projSpeed;
        enemyProjectiles[i].y += enemyProjectiles[i].dirY * projSpeed;
        enemyProjectiles[i].z += enemyProjectiles[i].dirZ * projSpeed;

        // Verificar colisão com jogador em 3D
        float dx = enemyProjectiles[i].x - playerP7.x;
        float dy = enemyProjectiles[i].y - playerP7.y;
        float dz = enemyProjectiles[i].z - playerP7.z;
        float dist = sqrt(dx*dx + dy*dy + dz*dz);

        if (dist < playerHitRadius) {
            enemyProjectiles[i].active = false;
            phase7_vida--;
            if (phase7_vida <= 0) setGameOver(true);
        }

        // Desativar projétil após voar muito longe
        float travelDist = sqrt(
            (enemyProjectiles[i].x - enemyProjectiles[i].startX) * (enemyProjectiles[i].x - enemyProjectiles[i].startX) +
            (enemyProjectiles[i].y - enemyProjectiles[i].startY) * (enemyProjectiles[i].y - enemyProjectiles[i].startY) +
            (enemyProjectiles[i].z - enemyProjectiles[i].startZ) * (enemyProjectiles[i].z - enemyProjectiles[i].startZ)
        );
        if (travelDist > 25.0f) {
            enemyProjectiles[i].active = false;
        }
    }
}

// ===================================================================
// Lógica de Combate e Raycasting
// ===================================================================

void checkShootingHit() {
    // Criar projétil do jogador
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!playerProjectiles[i].active) {
            playerProjectiles[i].x = playerP7.x;
            playerProjectiles[i].y = playerP7.y;
            playerProjectiles[i].z = playerP7.z;
            
            // Direção do tiro
            playerProjectiles[i].dirX = sin(playerP7.angle) * cos(playerP7.pitch);
            playerProjectiles[i].dirY = sin(playerP7.pitch);
            playerProjectiles[i].dirZ = -cos(playerP7.angle) * cos(playerP7.pitch);
            
            playerProjectiles[i].active = true;
            playerProjectiles[i].distanceTraveled = 0.0f;
            
            // Som de laser
            Audio::getInstance().play(Audio::SOUND_LASER);
            break;
        }
    }
}

// ===================================================================
// Funções Principais
// ===================================================================

void initPhase7() {
    printf("Initializing Phase 7 (Map P5 + FPS)...\n");
    srand((unsigned int)time(NULL));

    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0); 
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    
    // Configuração de luz principal (sala)
    GLfloat light_pos[] = {0.0f, 3.0f, 0.0f, 1.0f};
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Player começa no corredor central, olhando para o painel
    playerP7.x = 0.0f; playerP7.y = 1.6f; playerP7.z = 3.0f;
    playerP7.angle = 0.0f; playerP7.pitch = 0.0f;
    
    lastMouseXP7 = -1; lastMouseYP7 = -1;
    for(int i = 0; i < 256; i++) keyStateP7[i] = false;

    for(int i=0; i<MAX_PROJECTILES; i++) {
        enemyProjectiles[i].active = false;
        playerProjectiles[i].active = false;
    }
    numEnemiesP7 = 1;
    
    // 1 inimigo que aparece em posição aleatória escondida
    // Posições possíveis: nos cantos e longe do jogador
    float spawnPositions[][2] = {
        {-7.0f, -7.0f},  // Canto traseiro esquerdo
        {7.0f, -7.0f},   // Canto traseiro direito
        {-7.0f, 7.0f},   // Canto frontal esquerdo
        {7.0f, 7.0f}     // Canto frontal direito
    };
    int randomSpawn = rand() % 4;
    enemiesP7[0].x = spawnPositions[randomSpawn][0]; 
    enemiesP7[0].y = 0.0f; // No chão
    enemiesP7[0].z = spawnPositions[randomSpawn][1];
    enemiesP7[0].startX = enemiesP7[0].x; 
    enemiesP7[0].startZ = enemiesP7[0].z; 
    enemiesP7[0].moveAxis = 0;
    enemiesP7[0].hp = 10; // HP aumentado para 10 tiros
    enemiesP7[0].active = true;
    enemiesP7[0].moveDir = 1.0f; 
    enemiesP7[0].shootTimer = rand() % 100;

    phase7_municao = 20; phase7_vida = 10;
    phase7_won = false; gameOverP7 = false; phase7_gameOver = false;
    scoreP7 = 0;
    
    // Initialize countdown
    showCountdownP7 = true;
    countdownTimerP7 = 0;
    countdownValueP7 = 3;
    showFindEnemyMsg = false;
    findEnemyMsgTimer = 0;
    
    // Reset da IA do inimigo
    resetEnemyAI();
    
    // Carregar modelo 3D do inimigo
    loadOBJ("src/modelos/Oozey.obj");
    
    // Carregar texturas
    if (!texturesLoadedP7) {
        monsterTextureID = loadTexture("src/modelos/oozey_diffuse.png");
        textureWallP7 = createProceduralTextureP7(256, 256, 0);
        textureFloorP7 = createProceduralTextureP7(256, 256, 1);
        textureProjectileP7 = createProceduralTextureP7(64, 64, 2);
        texturesLoadedP7 = true;
        printf("Texturas da Fase 7 criadas\n");
    }
    
    setGameOver(false); setVictory(false);
    initGameOver(800, 700);
    registerRestartCallback(initPhase7);
    registerMenuCallback(returnToMenuFromPhase7);
    // Play boss music for Phase 7
    Audio::getInstance().playMusic("assets/music/boss2.mp3");
    
    // Garantir que o timer está rodando
    glutTimerFunc(16, updatePhase7, 0);
}

// Colisão Detalhada da Fase 5 (Adaptada para P7)
bool checkCollisionP7(float x, float z) {
    float radius = 0.3f;
    
    // Limites Externos (-10 a 10)
    if (x < -9.5f + radius || x > 9.5f - radius || z < -9.5f + radius || z > 9.5f - radius) return true;
    
    // --- Paredes dos Cômodos ---
    // Quarto
    if (fabs(x + 3.0f) < 0.15f && z > -10.0f - radius && z < -7.0f + radius) return true;
    if (fabs(x + 3.0f) < 0.15f && z > -5.0f - radius && z < -3.0f + radius) return true;
    if (fabs(z + 3.0f) < 0.15f && x < -3.0f + radius && x > -10.0f - radius) return true;
    
    // Cozinha
    if (fabs(x - 3.0f) < 0.15f && z > -10.0f - radius && z < -7.0f + radius) return true;
    if (fabs(x - 3.0f) < 0.15f && z > -5.0f - radius && z < -3.0f + radius) return true;
    if (fabs(z + 3.0f) < 0.15f && x > 3.0f - radius && x < 10.0f + radius) return true;
    
    // Munição
    if (fabs(x + 3.0f) < 0.15f && z > 3.0f - radius && z < 5.0f - radius) return true;
    if (fabs(z - 3.0f) < 0.15f && x < -3.0f + radius && x > -10.0f - radius) return true;
    if (fabs(z - 8.5f) < 0.15f && x > -10.0f - radius && x < -3.0f + radius) return true;
    
    // Console
    if (fabs(x - 3.0f) < 0.15f && z > 3.0f - radius && z < 5.0f - radius) return true;
    if (fabs(z - 3.0f) < 0.15f && x > 3.0f - radius && x < 10.0f + radius) return true;
    if (fabs(z - 8.5f) < 0.15f && x < 10.0f + radius && x > 3.0f - radius) return true;

    // --- Móveis Principais ---
    // Cama
    if (x > -8.0f - radius && x < -6.0f + radius && z > -8.0f - radius && z < -6.0f + radius) return true;
    // Bancada
    if (x > 6.0f - radius && x < 8.0f + radius && z > -8.0f - radius && z < -6.5f + radius) return true;
    // Caixas Munição
    if (x > -8.5f - radius && x < -6.5f + radius && z > 4.5f - radius && z < 6.0f + radius) return true;
    // Sofá
    if (x > 4.5f - radius && x < 6.5f + radius && z > 5.3f - radius && z < 6.3f + radius) return true;
    // Painel Frontal
    if (fabs(x) < 0.6f && z > 7.0f - radius && z < 8.0f + radius) return true;

    return false;
}

void drawPhase7(int windowWidth, int windowHeight) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / (double)windowHeight, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    float lookX = playerP7.x + sin(playerP7.angle);
    float lookY = playerP7.y + sin(playerP7.pitch);
    float lookZ = playerP7.z - cos(playerP7.angle);
    gluLookAt(playerP7.x, playerP7.y, playerP7.z, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);

    glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST);

    // ===================================
    // MAPA DA FASE 5 (Reconstruído)
    // ===================================
    
    // Chão com textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureFloorP7);
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

    // Teto
    glBegin(GL_QUADS); glColor3f(0.25f, 0.25f, 0.28f); glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-10.0f, 3.0f, -10.0f); glVertex3f(10.0f, 3.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f); glVertex3f(-10.0f, 3.0f, 10.0f);
    glEnd();

    // Paredes Externas com textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureWallP7);
    glColor3f(0.9f, 0.9f, 1.0f);
    
    glBegin(GL_QUADS);
    // Parede esquerda
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 3.0f, -10.0f);
    glTexCoord2f(5.0f, 1.0f); glVertex3f(-10.0f, 3.0f, 10.0f);
    glTexCoord2f(5.0f, 0.0f); glVertex3f(-10.0f, 0.0f, 10.0f);
    
    // Parede direita
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, 0.0f, -10.0f);
    glTexCoord2f(5.0f, 0.0f); glVertex3f(10.0f, 0.0f, 10.0f);
    glTexCoord2f(5.0f, 1.0f); glVertex3f(10.0f, 3.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 3.0f, -10.0f);
    
    // Parede traseira
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -10.0f);
    glTexCoord2f(5.0f, 0.0f); glVertex3f(10.0f, 0.0f, -10.0f);
    glTexCoord2f(5.0f, 1.0f); glVertex3f(10.0f, 3.0f, -10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 3.0f, -10.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Parabrisa Frontal
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.35f);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-10.0f, 0.0f, 10.0f); glVertex3f(-10.0f, 3.0f, 10.0f); glVertex3f(-4.0f, 3.0f, 10.0f); glVertex3f(-4.0f, 0.0f, 10.0f);
    glVertex3f(4.0f, 0.0f, 10.0f); glVertex3f(4.0f, 3.0f, 10.0f); glVertex3f(10.0f, 3.0f, 10.0f); glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(-4.0f, 0.0f, 10.0f); glVertex3f(-4.0f, 1.0f, 10.0f); glVertex3f(4.0f, 1.0f, 10.0f); glVertex3f(4.0f, 0.0f, 10.0f);
    glColor3f(0.1f, 0.15f, 0.3f); // Vidro
    glVertex3f(-4.0f, 1.0f, 10.0f); glVertex3f(-4.0f, 3.0f, 10.0f); glVertex3f(4.0f, 3.0f, 10.0f); glVertex3f(4.0f, 1.0f, 10.0f);
    glEnd();

    // Paredes Internas (Quartos)
    glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(1.0f, 1.0f);
    glColor3f(0.35f, 0.35f, 0.4f);
    
    // Quarto (Esquerda Trás)
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -3.0f); glVertex3f(-10.0f, 3.0f, -3.0f); glVertex3f(-3.0f, 3.0f, -3.0f); glVertex3f(-3.0f, 0.0f, -3.0f); // Frente
    glVertex3f(-3.0f, 0.0f, -10.0f); glVertex3f(-3.0f, 0.0f, -7.0f); glVertex3f(-3.0f, 3.0f, -7.0f); glVertex3f(-3.0f, 3.0f, -10.0f); // Dir (seção 1)
    glVertex3f(-3.0f, 0.0f, -5.0f); glVertex3f(-3.0f, 0.0f, -3.0f); glVertex3f(-3.0f, 3.0f, -3.0f); glVertex3f(-3.0f, 3.0f, -5.0f); // Dir (seção 2)
    glEnd();

    // Cozinha (Direita Trás)
    glBegin(GL_QUADS);
    glVertex3f(3.0f, 0.0f, -3.0f); glVertex3f(3.0f, 3.0f, -3.0f); glVertex3f(10.0f, 3.0f, -3.0f); glVertex3f(10.0f, 0.0f, -3.0f); // Frente
    glVertex3f(3.0f, 0.0f, -10.0f); glVertex3f(3.0f, 3.0f, -10.0f); glVertex3f(3.0f, 3.0f, -7.0f); glVertex3f(3.0f, 0.0f, -7.0f); // Esq (seção 1)
    glVertex3f(3.0f, 0.0f, -5.0f); glVertex3f(3.0f, 3.0f, -5.0f); glVertex3f(3.0f, 3.0f, -3.0f); glVertex3f(3.0f, 0.0f, -3.0f); // Esq (seção 2)
    glEnd();

    // Munição (Esquerda Frente)
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, 3.0f); glVertex3f(-3.0f, 0.0f, 3.0f); glVertex3f(-3.0f, 3.0f, 3.0f); glVertex3f(-10.0f, 3.0f, 3.0f); // Trás
    glVertex3f(-10.0f, 0.0f, 8.5f); glVertex3f(-10.0f, 3.0f, 8.5f); glVertex3f(-3.0f, 3.0f, 8.5f); glVertex3f(-3.0f, 0.0f, 8.5f); // Frente
    glVertex3f(-3.0f, 0.0f, 3.0f); glVertex3f(-3.0f, 0.0f, 5.0f); glVertex3f(-3.0f, 3.0f, 5.0f); glVertex3f(-3.0f, 3.0f, 3.0f); // Dir (seção 1)
    glVertex3f(-3.0f, 0.0f, 7.5f); glVertex3f(-3.0f, 0.0f, 8.5f); glVertex3f(-3.0f, 3.0f, 8.5f); glVertex3f(-3.0f, 3.0f, 7.5f); // Dir (seção 2)
    glEnd();

    // Console (Direita Frente)
    glBegin(GL_QUADS);
    glVertex3f(3.0f, 0.0f, 3.0f); glVertex3f(10.0f, 0.0f, 3.0f); glVertex3f(10.0f, 3.0f, 3.0f); glVertex3f(3.0f, 3.0f, 3.0f); // Trás
    glVertex3f(3.0f, 0.0f, 8.5f); glVertex3f(3.0f, 3.0f, 8.5f); glVertex3f(10.0f, 3.0f, 8.5f); glVertex3f(10.0f, 0.0f, 8.5f); // Frente
    glVertex3f(3.0f, 0.0f, 3.0f); glVertex3f(3.0f, 3.0f, 3.0f); glVertex3f(3.0f, 3.0f, 5.0f); glVertex3f(3.0f, 0.0f, 5.0f); // Esq (seção 1)
    glVertex3f(3.0f, 0.0f, 7.5f); glVertex3f(3.0f, 3.0f, 7.5f); glVertex3f(3.0f, 3.0f, 8.5f); glVertex3f(3.0f, 0.0f, 8.5f); // Esq (seção 2)
    glEnd();
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Móveis
    drawControlConsoleP7(0.0f, 7.5f, 0.0f); // Console principal
    drawCommandChairP7(0.0f, 6.0f, 0.0f);
    drawBedP7(-7.0f, -7.0f, 0.0f); // Cama
    drawKitchenCounterP7(7.0f, -7.0f, 180.0f); // Cozinha
    drawSofaP7(5.5f, 5.8f, 90.0f); // Sofá
    drawAmmoBoxP7(-8.0f, 5.0f); // Munição
    drawWeaponRackP7(-8.5f, 7.5f, 0.0f); // Rack
    drawTVP7(-9.4f, -4.5f, 90.0f); // TV

    // Inimigos
    for(int i = 0; i < numEnemiesP7; i++) {
        if(enemiesP7[i].active) {
            glPushMatrix();
            glTranslatef(enemiesP7[i].x, enemiesP7[i].y, enemiesP7[i].z);
            
            // Rotacionar o modelo para ficar de pé e virado para o jogador
            float angleToPlayer = atan2(playerP7.x - enemiesP7[i].x, playerP7.z - enemiesP7[i].z) * 180.0f / 3.14159f;
            glRotatef(angleToPlayer, 0, 1, 0);
            
            // Material com forte emissão verde para brilhar sozinho
            GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
            GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
            GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
            GLfloat mat_shininess[] = {32.0f};
            GLfloat mat_emission[] = {0.3f, 0.5f, 0.3f, 1.0f};  // Emissão verde forte
            
            // Ajustar emissão baseado no HP
            if (enemiesP7[i].hp >= 4) { 
                mat_emission[0] = 0.3f; mat_emission[1] = 0.5f; mat_emission[2] = 0.3f;
            }
            else if (enemiesP7[i].hp >= 2) { 
                mat_emission[0] = 0.2f; mat_emission[1] = 0.35f; mat_emission[2] = 0.2f;
            }
            else { 
                mat_emission[0] = 0.15f; mat_emission[1] = 0.25f; mat_emission[2] = 0.15f;
            }
            
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
            
            drawOBJ(0.5f);
            
            // Resetar material para padrão da sala
            GLfloat default_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
            GLfloat default_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
            GLfloat default_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
            GLfloat default_shininess[] = {0.0f};
            GLfloat no_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
            
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, default_ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default_diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, default_specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, default_shininess);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_emission);
            
            glPopMatrix();
        }
    }

    // Projéteis do jogador (vermelhos brilhantes)
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(playerProjectiles[i].active) {
            glPushMatrix(); 
            glTranslatef(playerProjectiles[i].x, playerProjectiles[i].y, playerProjectiles[i].z);
            
            // Brilho externo
            glColor3f(1.0f, 0.3f, 0.3f);
            glutSolidSphere(0.14f, 10, 10);
            
            // Núcleo brilhante
            glColor3f(1.0f, 0.9f, 0.9f);
            glutSolidSphere(0.08f, 8, 8);
            
            glPopMatrix();
        }
    }

    // Projéteis dos inimigos (amarelos brilhantes)
    for(int i=0; i<maxProjectilesP7; i++) {
        if(enemyProjectiles[i].active) {
            glPushMatrix(); 
            glTranslatef(enemyProjectiles[i].x, enemyProjectiles[i].y, enemyProjectiles[i].z);
            
            // Brilho externo
            glColor3f(1.0f, 1.0f, 0.2f);
            glutSolidSphere(0.17f, 10, 10);
            
            // Núcleo brilhante
            glColor3f(1.0f, 1.0f, 0.9f);
            glutSolidSphere(0.10f, 8, 8);
            
            glPopMatrix();
        }
    }
    
    glEnable(GL_LIGHTING);

    // HUD
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);

    // Draw countdown if active
    if (showCountdownP7 && countdownValueP7 > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 700);
        glVertex2f(0, 700);
        glEnd();
        glDisable(GL_BLEND);
        
        char countText[10];
        sprintf(countText, "%d", countdownValueP7);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(400.0f - 20.0f, 350.0f);
        for (char* p = countText; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
        }
    }
    
    // Mostrar mensagem "ENCONTRE O INIMIGO" após countdown
    if (showFindEnemyMsg) {
        glColor3f(1.0f, 0.2f, 0.2f); // Vermelho
        const char* msg = "ENCONTRE O INIMIGO!";
        glRasterPos2f(windowWidth / 2.0f - 120.0f, windowHeight / 2.0f + 50.0f);
        for (const char* p = msg; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
        }
    }

    // Desenhar tela de pausa se ESC foi pressionado
    if (getPaused()) {
        drawPauseScreen();
        glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
        return;
    }

    if (getGameOver()) {
        drawGameOver();
        glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
        return;
    }

    char text[100];
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(text, "Municao: %d/20", phase7_municao);
    drawTextP7(windowWidth - 180, windowHeight - 20, text);
    
    // Barra de vida (esquerda superior)
    glColor3f(0.2f, 0.2f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(20, windowHeight - 40);
    glVertex2f(220, windowHeight - 40);
    glVertex2f(220, windowHeight - 20);
    glVertex2f(20, windowHeight - 20);
    glEnd();
    
    // Borda da barra
    glColor3f(0.4f, 0.4f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, windowHeight - 40);
    glVertex2f(220, windowHeight - 40);
    glVertex2f(220, windowHeight - 20);
    glVertex2f(20, windowHeight - 20);
    glEnd();
    
    // Vida atual (verde para vermelho)
    float lifePercent = (float)phase7_vida / 10.0f;
    float barWidth = 196.0f * lifePercent;
    
    if (lifePercent > 0.6f) {
        glColor3f(0.2f, 0.8f, 0.3f); // Verde
    } else if (lifePercent > 0.3f) {
        glColor3f(0.9f, 0.8f, 0.2f); // Amarelo
    } else {
        glColor3f(0.9f, 0.2f, 0.2f); // Vermelho
    }
    
    glBegin(GL_QUADS);
    glVertex2f(22, windowHeight - 38);
    glVertex2f(22 + barWidth, windowHeight - 38);
    glVertex2f(22 + barWidth, windowHeight - 22);
    glVertex2f(22, windowHeight - 22);
    glEnd();
    
    // Texto da vida
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(text, "VIDA: %d/10", phase7_vida);
    drawTextP7(30, windowHeight - 33, text);
    
    // Mostrar HP do monstro
    if (numEnemiesP7 > 0 && enemiesP7[0].active) {
        if (enemiesP7[0].hp >= 7) glColor3f(0.2f, 0.9f, 0.3f); // Verde brilhante
        else if (enemiesP7[0].hp >= 4) glColor3f(0.9f, 0.8f, 0.2f); // Amarelo
        else glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
        sprintf(text, "Monstro HP: %d/10", enemiesP7[0].hp);
        drawTextP7(windowWidth - 180, windowHeight - 60, text);
    }

    float cx = windowWidth / 2.0f; float cy = windowHeight / 2.0f;
    glColor3f(0.0f, 1.0f, 0.0f); glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(cx - 10, cy); glVertex2f(cx + 10, cy); glVertex2f(cx, cy - 10); glVertex2f(cx, cy + 10);
    glEnd(); glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

void updatePhase7(int value) {
    (void)value;
    
    // Se não estamos mais na fase 7, parar de atualizar
    if (currentState != PHASE7_SCREEN) {
        return;
    }
    
    if (getPaused()) {
        glutTimerFunc(16, updatePhase7, 0);
        return;
    }
    
    // Update countdown
    if (showCountdownP7) {
        countdownTimerP7++;
        if (countdownTimerP7 >= 60) {
            countdownTimerP7 = 0;
            countdownValueP7--;
            if (countdownValueP7 <= 0) {
                showCountdownP7 = false;
                showFindEnemyMsg = true; // Mostrar mensagem após countdown
                findEnemyMsgTimer = 0;
            }
        }
        glutPostRedisplay();
        glutTimerFunc(16, updatePhase7, 0);
        return;
    }
    
    // Atualizar timer da mensagem "ENCONTRE O INIMIGO"
    if (showFindEnemyMsg) {
        findEnemyMsgTimer++;
        if (findEnemyMsgTimer >= 120) { // Mostrar por 2 segundos
            showFindEnemyMsg = false;
        }
    }
    
    if (getGameOver()) { glutPostRedisplay(); return; }

    float speed = 0.10f; // Velocidade um pouco mais lenta
    float fwdX = sin(playerP7.angle); float fwdZ = -cos(playerP7.angle);
    float strafeX = cos(playerP7.angle); float strafeZ = sin(playerP7.angle);
    float moveX = 0.0f; float moveZ = 0.0f;
    float moveY = 0.0f;

    if(keyStateP7['w'] || keyStateP7['W']) { moveX += fwdX; moveZ += fwdZ; }
    if(keyStateP7['s'] || keyStateP7['S']) { moveX -= fwdX; moveZ -= fwdZ; }
    if(keyStateP7['d'] || keyStateP7['D']) { moveX += strafeX; moveZ += strafeZ; }
    if(keyStateP7['a'] || keyStateP7['A']) { moveX -= strafeX; moveZ -= strafeZ; }
    
    // Movimento vertical
    if(keyStateP7['q'] || keyStateP7['Q']) { moveY += 1.0f; }
    if(keyStateP7['e'] || keyStateP7['E']) { moveY -= 1.0f; }

    float length = sqrt(moveX * moveX + moveZ * moveZ);
    if (length > 0.0f) {
        moveX /= length; moveZ /= length;
        float nextX = playerP7.x + moveX * speed;
        float nextZ = playerP7.z + moveZ * speed;
        if (!checkCollisionP7(nextX, playerP7.z)) playerP7.x = nextX;
        if (!checkCollisionP7(playerP7.x, nextZ)) playerP7.z = nextZ;
    }
    
    // Aplicar movimento vertical com limites
    if (moveY != 0.0f) {
        float nextY = playerP7.y + moveY * speed;
        // Limites de altura (chão a 0.5m, teto a 3.5m)
        if (nextY >= 0.5f && nextY <= 3.5f) {
            playerP7.y = nextY;
        }
    }

    updateEnemiesAI();
    updateProjectiles();

    glutPostRedisplay();
    glutTimerFunc(16, updatePhase7, 0);
}

void handlePhase7Keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    
    // ESC para pausar
    if (key == 27) {
        if (!getGameOver() && !getPaused()) {
            setPaused(true, 7);
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
    
    if (getGameOver()) { handleGameOverKeyboard(key); return; }
    
    keyStateP7[key] = true;

    if (key == ' ') {
        if (!getGameOver()) {
            if (phase7_municao > 0) {
                phase7_municao--;
                Audio::getInstance().play(Audio::SOUND_LASER);
                checkShootingHit();
            } else {
                Audio::getInstance().play(Audio::SOUND_ERROR);
            }
        }
    }
}

void handlePhase7KeyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    keyStateP7[key] = false;
}

void handlePhase7Mouse(int button, int state, int x, int y) {
    (void)x; (void)y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!getGameOver()) {
            if (phase7_municao > 0) {
                phase7_municao--;
                Audio::getInstance().play(Audio::SOUND_LASER);
                checkShootingHit();
            } else {
                Audio::getInstance().play(Audio::SOUND_ERROR);
            }
        }
    }
}

void handlePhase7PassiveMotion(int x, int y) {
    (void)y;
    if (getGameOver()) return;
    if (lastMouseXP7 == -1) { lastMouseXP7 = x; lastMouseYP7 = y; return; }
    int dx = x - lastMouseXP7;
    int dy = y - lastMouseYP7;
    lastMouseXP7 = x;
    lastMouseYP7 = y;
    
    float sensitivity = 0.008f;
    playerP7.angle += dx * sensitivity;
    playerP7.pitch -= dy * sensitivity;
    
    if (playerP7.pitch > 1.5f) playerP7.pitch = 1.5f;
    if (playerP7.pitch < -1.5f) playerP7.pitch = -1.5f;
}

void handlePhase7MouseClick(int button, int state, int x, int y) {
    handlePhase7Mouse(button, state, x, y);
}

void handlePhase7MouseMove(int x, int y) {
    handlePhase7PassiveMotion(x, y);
}

void handlePhase7SpecialKey(int key, int x, int y) {
    (void)x; (void)y;
    if (getGameOver()) return;
    const float turnSpeed = 0.05f; 
    const float pitchSpeed = 0.05f;
    
    if (key == GLUT_KEY_LEFT) playerP7.angle -= turnSpeed;
    if (key == GLUT_KEY_RIGHT) playerP7.angle += turnSpeed;
    if (key == GLUT_KEY_UP) playerP7.pitch += pitchSpeed;
    if (key == GLUT_KEY_DOWN) playerP7.pitch -= pitchSpeed;

    if (playerP7.pitch > 1.5f) playerP7.pitch = 1.5f;
    if (playerP7.pitch < -1.5f) playerP7.pitch = -1.5f;
}

void handlePhase7SpecialKeyUp(int key, int x, int y) {
    (void)key; (void)x; (void)y;
}

void returnToMenuFromPhase7() {
    phase7_gameOver = false;
    gameOverP7 = false;
    phase7_won = false;
    setGameOver(false);
    setVictory(false);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Restaurar cor de fundo do menu
    setPaused(false, 0);
    setCurrentPhase(0);
    // Restore menu music
    Audio::getInstance().playMusic("assets/music/menu.wav");
    glutPostRedisplay();
}

void drawPhase7() {
    drawPhase7(800, 700);
}
