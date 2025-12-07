#include "phase7.h"
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

float timeRemainingP7 = 15.0f;
bool gameOverP7 = false;
bool phase7_gameOver = false; 

bool keyStateP7[256];
int lastMouseXP7;
int lastMouseYP7;

// Variáveis de Animação do Laser
int laserAnimTimer = 0;
float laserEndX = 0, laserEndY = 0, laserEndZ = 0;

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
    glColor3f(0.05f, 0.1f, 0.15f); glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.06f); glScalef(1.3f, 0.8f, 0.01f); glutSolidCube(1.0f); glPopMatrix();
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

void spawnEnemyProjectile(float x, float z) {
    int slot = -1;
    for(int i=0; i<maxProjectilesP7; i++) {
        if(!enemyProjectiles[i].active) { slot = i; break; }
    }
    if (slot == -1) return;

    float dx = playerP7.x - x;
    float dz = playerP7.z - z;
    float dist = sqrt(dx*dx + dz*dz);
    
    if (dist > 0.1f) {
        enemyProjectiles[slot].x = x;
        enemyProjectiles[slot].y = 1.0f;
        enemyProjectiles[slot].z = z;
        enemyProjectiles[slot].dirX = dx / dist;
        enemyProjectiles[slot].dirZ = dz / dist;
        enemyProjectiles[slot].active = true;
    }
}

void updateEnemiesAI() {
    float moveSpeed = 0.05f;
    float patrolRange = 2.0f; // Reduzido para quartos pequenos

    for(int i=0; i<numEnemiesP7; i++) {
        if(!enemiesP7[i].active) continue;

        if (enemiesP7[i].moveAxis == 0) {
            enemiesP7[i].x += enemiesP7[i].moveDir * moveSpeed;
            if (enemiesP7[i].x > enemiesP7[i].startX + patrolRange) enemiesP7[i].moveDir = -1.0f;
            if (enemiesP7[i].x < enemiesP7[i].startX - patrolRange) enemiesP7[i].moveDir = 1.0f;
        } else {
            enemiesP7[i].z += enemiesP7[i].moveDir * moveSpeed;
            if (enemiesP7[i].z > enemiesP7[i].startZ + patrolRange) enemiesP7[i].moveDir = -1.0f;
            if (enemiesP7[i].z < enemiesP7[i].startZ - patrolRange) enemiesP7[i].moveDir = 1.0f;
        }

        float dx = playerP7.x - enemiesP7[i].x;
        float dz = playerP7.z - enemiesP7[i].z;
        float dist = sqrt(dx*dx + dz*dz);

        // Tiro se estiver perto (ignora paredes para simplificar IA)
        if (dist < 10.0f) {
            enemiesP7[i].shootTimer++;
            if (enemiesP7[i].shootTimer > (90 + rand()%60)) {
                spawnEnemyProjectile(enemiesP7[i].x, enemiesP7[i].z);
                enemiesP7[i].shootTimer = 0;
            }
        }
    }
}

void updateProjectiles() {
    float projSpeed = 0.2f;
    float playerHitRadius = 0.6f;

    for(int i=0; i<maxProjectilesP7; i++) {
        if(!enemyProjectiles[i].active) continue;

        enemyProjectiles[i].x += enemyProjectiles[i].dirX * projSpeed;
        enemyProjectiles[i].z += enemyProjectiles[i].dirZ * projSpeed;

        float dx = enemyProjectiles[i].x - playerP7.x;
        float dz = enemyProjectiles[i].z - playerP7.z;
        float dist = sqrt(dx*dx + dz*dz);

        if (dist < playerHitRadius) {
            enemyProjectiles[i].active = false;
            phase7_vida--;
            if (phase7_vida <= 0) setGameOver(true);
        }

        // Limites da Fase 5 (aprox -10 a 10)
        if (enemyProjectiles[i].x < -12 || enemyProjectiles[i].x > 12 || 
            enemyProjectiles[i].z < -12 || enemyProjectiles[i].z > 12) {
            enemyProjectiles[i].active = false;
        }
    }
}

// ===================================================================
// Lógica de Combate e Raycasting
// ===================================================================

void checkShootingHit() {
    float hitThreshold = 1.2f; 
    float maxDist = 80.0f;     
    
    // Vetores de direção 3D
    float dirX = sin(playerP7.angle) * cos(playerP7.pitch);
    float dirY = sin(playerP7.pitch);
    float dirZ = -cos(playerP7.angle) * cos(playerP7.pitch);

    laserEndX = playerP7.x + dirX * maxDist;
    laserEndY = playerP7.y + dirY * maxDist;
    laserEndZ = playerP7.z + dirZ * maxDist;
    laserAnimTimer = 8; 

    float closestDist = maxDist;
    int hitIndex = -1;

    for (int i = 0; i < numEnemiesP7; i++) {
        if (!enemiesP7[i].active) continue;

        float ex = enemiesP7[i].x - playerP7.x;
        float ey = enemiesP7[i].y - playerP7.y;
        float ez = enemiesP7[i].z - playerP7.z;

        float t = ex * dirX + ey * dirY + ez * dirZ;
        
        if (t < 0 || t > closestDist) continue;

        float px = playerP7.x + t * dirX;
        float py = playerP7.y + t * dirY;
        float pz = playerP7.z + t * dirZ;

        float distSq = (enemiesP7[i].x - px)*(enemiesP7[i].x - px) +
                       (enemiesP7[i].y - py)*(enemiesP7[i].y - py) +
                       (enemiesP7[i].z - pz)*(enemiesP7[i].z - pz);

        if (distSq < hitThreshold * hitThreshold) {
            closestDist = t;
            hitIndex = i;
        }
    }

    if (hitIndex != -1) {
        laserEndX = playerP7.x + dirX * closestDist;
        laserEndY = playerP7.y + dirY * closestDist;
        laserEndZ = playerP7.z + dirZ * closestDist;

        enemiesP7[hitIndex].hp--;
        enemiesP7[hitIndex].y += 0.2f; 

        if (enemiesP7[hitIndex].hp <= 0) {
            enemiesP7[hitIndex].active = false;
            scoreP7 += 100;
            
            bool anyAlive = false;
            for(int k = 0; k < numEnemiesP7; k++) {
                if (enemiesP7[k].active) { anyAlive = true; break; }
            }
            if (!anyAlive) {
                phase7_won = true;
                setVictory(true);
                setGameOver(true);
            }
        }
    }
}

// ===================================================================
// Funções Principais
// ===================================================================

void initPhase7() {
    printf("Initializing Phase 7 (Map P5 + FPS)...\n");
    srand((unsigned int)time(NULL));

    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_COLOR_MATERIAL);
    GLfloat light_pos[] = {0.0, 3.0, 0.0, 1.0}; // Luz mais baixa
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_DEPTH_TEST);

    // Player começa no corredor central, olhando para o painel
    playerP7.x = 0.0f; playerP7.y = 1.6f; playerP7.z = 3.0f;
    playerP7.angle = 0.0f; playerP7.pitch = 0.0f;
    
    lastMouseXP7 = -1; lastMouseYP7 = -1;
    for(int i = 0; i < 256; i++) keyStateP7[i] = false;

    for(int i=0; i<MAX_PROJECTILES; i++) enemyProjectiles[i].active = false;
    numEnemiesP7 = 5;
    
    // Inimigos posicionados nos cômodos da Fase 5
    // 1. Quarto (Traseira Esquerda)
    enemiesP7[0].x = -7.0f; enemiesP7[0].z = -7.0f; enemiesP7[0].startX = -7.0f; enemiesP7[0].startZ = -7.0f; enemiesP7[0].moveAxis = 0; 
    // 2. Cozinha (Traseira Direita)
    enemiesP7[1].x = 7.0f; enemiesP7[1].z = -7.0f; enemiesP7[1].startX = 7.0f; enemiesP7[1].startZ = -7.0f; enemiesP7[1].moveAxis = 1;
    // 3. Sala Munição (Frente Esquerda)
    enemiesP7[2].x = -7.0f; enemiesP7[2].z = 5.0f; enemiesP7[2].startX = -7.0f; enemiesP7[2].startZ = 5.0f; enemiesP7[2].moveAxis = 0;
    // 4. Console (Frente Direita)
    enemiesP7[3].x = 7.0f; enemiesP7[3].z = 5.0f; enemiesP7[3].startX = 7.0f; enemiesP7[3].startZ = 5.0f; enemiesP7[3].moveAxis = 1;
    // 5. Corredor Traseiro (Centro)
    enemiesP7[4].x = 0.0f; enemiesP7[4].z = -7.0f; enemiesP7[4].startX = 0.0f; enemiesP7[4].startZ = -7.0f; enemiesP7[4].moveAxis = 0;

    for(int i = 0; i < numEnemiesP7; i++) {
        enemiesP7[i].y = 1.0f; enemiesP7[i].hp = 2; enemiesP7[i].active = true;
        enemiesP7[i].moveDir = 1.0f; enemiesP7[i].shootTimer = rand() % 100;
    }

    phase7_municao = 20; phase7_vida = 10;
    phase7_won = false; gameOverP7 = false; phase7_gameOver = false;
    scoreP7 = 0; laserAnimTimer = 0;
    
    setGameOver(false); setVictory(false);
    initGameOver(800, 600);
    registerRestartCallback(initPhase7);
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
    
    // Chão
    glBegin(GL_QUADS);
    for (int x = -10; x < 10; x++) {
        for (int z = -10; z < 10; z++) {
            if ((x + z) % 2 == 0) glColor3f(0.35f, 0.35f, 0.4f); else glColor3f(0.3f, 0.3f, 0.35f);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f((float)x, 0.0f, (float)z); glVertex3f((float)x, 0.0f, (float)(z + 1));
            glVertex3f((float)(x + 1), 0.0f, (float)(z + 1)); glVertex3f((float)(x + 1), 0.0f, (float)z);
        }
    }
    glEnd();

    // Teto
    glBegin(GL_QUADS); glColor3f(0.25f, 0.25f, 0.28f); glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-10.0f, 3.0f, -10.0f); glVertex3f(10.0f, 3.0f, -10.0f);
    glVertex3f(10.0f, 3.0f, 10.0f); glVertex3f(-10.0f, 3.0f, 10.0f);
    glEnd();

    // Paredes Externas
    glColor3f(0.3f, 0.3f, 0.35f);
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0); glVertex3f(-10.0f, 0.0f, -10.0f); glVertex3f(-10.0f, 3.0f, -10.0f); glVertex3f(-10.0f, 3.0f, 10.0f); glVertex3f(-10.0f, 0.0f, 10.0f);
    glNormal3f(-1.0, 0.0, 0.0); glVertex3f(10.0f, 0.0f, -10.0f); glVertex3f(10.0f, 0.0f, 10.0f); glVertex3f(10.0f, 3.0f, 10.0f); glVertex3f(10.0f, 3.0f, -10.0f);
    glNormal3f(0.0, 0.0, 1.0); glVertex3f(-10.0f, 0.0f, -10.0f); glVertex3f(10.0f, 0.0f, -10.0f); glVertex3f(10.0f, 3.0f, -10.0f); glVertex3f(-10.0f, 3.0f, -10.0f);
    glEnd();

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
            if (enemiesP7[i].hp == 2) { glColor3f(0.0f, 1.0f, 1.0f); glutSolidSphere(0.5f, 20, 20); }
            else { glColor3f(1.0f, 0.0f, 0.0f); glutSolidSphere(0.5f, 20, 20); glColor3f(1.0f, 1.0f, 1.0f); glutWireSphere(0.52f, 12, 12); }
            glPopMatrix();
            if (enemiesP7[i].y > 1.0f) enemiesP7[i].y -= 0.05f;
        }
    }

    // Projéteis
    glColor3f(1.0f, 1.0f, 0.0f);
    for(int i=0; i<maxProjectilesP7; i++) {
        if(enemyProjectiles[i].active) {
            glPushMatrix(); glTranslatef(enemyProjectiles[i].x, enemyProjectiles[i].y, enemyProjectiles[i].z);
            glutSolidSphere(0.15f, 8, 8); glPopMatrix();
        }
    }

    // Laser do Jogador (Canto Inferior Central)
    if (laserAnimTimer > 0) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float dirX = sin(playerP7.angle) * cos(playerP7.pitch);
        float dirY = sin(playerP7.pitch);
        float dirZ = -cos(playerP7.angle) * cos(playerP7.pitch);
        
        float startX = playerP7.x + dirX * 0.8f;
        float startY = playerP7.y + dirY * 0.8f - 0.35f;
        float startZ = playerP7.z + dirZ * 0.8f;

        glPushMatrix(); glTranslatef(startX, startY, startZ);
        glColor4f(0.2f, 1.0f, 0.2f, 0.8f); glutSolidSphere(0.08f, 12, 12);
        glPopMatrix();

        glLineWidth(5.0f); glBegin(GL_LINES);
        glColor4f(0.0f, 1.0f, 0.0f, 0.6f);
        glVertex3f(startX, startY, startZ); glVertex3f(laserEndX, laserEndY, laserEndZ);
        glEnd(); glLineWidth(1.0f);

        glPushMatrix(); glTranslatef(laserEndX, laserEndY, laserEndZ);
        glColor4f(0.8f, 1.0f, 0.2f, 0.8f); glutSolidSphere(0.15f, 12, 12);
        glPopMatrix();

        glDisable(GL_BLEND); glEnable(GL_LIGHTING);
    }

    // HUD
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);

    if (getGameOver()) {
        drawGameOver();
        glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
        return;
    }

    char text[100];
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(text, "Municao: %d/20", phase7_municao);
    drawTextP7(windowWidth - 180, windowHeight - 20, text);
    
    if (phase7_vida > 5) glColor3f(0.2f, 1.0f, 0.2f); else glColor3f(1.0f, 0.0f, 0.0f);
    sprintf(text, "Vida: %d/10", phase7_vida);
    drawTextP7(windowWidth - 180, windowHeight - 40, text);

    float cx = windowWidth / 2.0f; float cy = windowHeight / 2.0f;
    glColor3f(0.0f, 1.0f, 0.0f); glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(cx - 10, cy); glVertex2f(cx + 10, cy); glVertex2f(cx, cy - 10); glVertex2f(cx, cy + 10);
    glEnd(); glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

void updatePhase7(int value) {
    (void)value;
    if (getGameOver()) { glutPostRedisplay(); return; }
    
    if (laserAnimTimer > 0) laserAnimTimer--;

    float speed = 0.2f; 
    float fwdX = sin(playerP7.angle); float fwdZ = -cos(playerP7.angle);
    float strafeX = cos(playerP7.angle); float strafeZ = sin(playerP7.angle);
    float moveX = 0.0f; float moveZ = 0.0f;

    if(keyStateP7['w'] || keyStateP7['W']) { moveX += fwdX; moveZ += fwdZ; }
    if(keyStateP7['s'] || keyStateP7['S']) { moveX -= fwdX; moveZ -= fwdZ; }
    if(keyStateP7['d'] || keyStateP7['D']) { moveX += strafeX; moveZ += strafeZ; }
    if(keyStateP7['a'] || keyStateP7['A']) { moveX -= strafeX; moveZ -= strafeZ; }

    float length = sqrt(moveX * moveX + moveZ * moveZ);
    if (length > 0.0f) {
        moveX /= length; moveZ /= length;
        float nextX = playerP7.x + moveX * speed;
        float nextZ = playerP7.z + moveZ * speed;
        if (!checkCollisionP7(nextX, playerP7.z)) playerP7.x = nextX;
        if (!checkCollisionP7(playerP7.x, nextZ)) playerP7.z = nextZ;
    }

    updateEnemiesAI();
    updateProjectiles();

    glutPostRedisplay();
    glutTimerFunc(16, updatePhase7, 0);
}

void handlePhase7Keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) { setCurrentPhase(0); return; }
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

void drawPhase7() {
    drawPhase7(800, 600);
}