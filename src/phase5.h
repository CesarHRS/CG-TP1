#ifndef PHASE5_H
#define PHASE5_H

#include <GL/glut.h>

// Struct for the player in 3D space
typedef struct {
    float x, y, z; // Player position
    float angle;   // Player's horizontal viewing angle
    float pitch;   // Player's vertical viewing angle (not used for now)
} Player3D;

// Enum for the geometric shapes
typedef enum {
    SHAPE_CUBE,
    SHAPE_SPHERE,
    SHAPE_PYRAMID,
    SHAPE_CYLINDER
} ShapeType;

// Struct for a collectible 3D object
typedef struct {
    float x, y, z;
    ShapeType type;
    bool collected;
    bool held; // Se o jogador está segurando o objeto
} Collectible;

// Struct for deposit zones
typedef struct {
    float x, y, z;
    ShapeType requiredType;
    bool filled;
} DepositZone;

// Function declarations for Phase 5 (3D Geometric Planet)
void initPhase5();
void drawPhase5(int windowWidth, int windowHeight);
void updatePhase5(int value);
void handlePhase5Keyboard(unsigned char key, int x, int y);
void handlePhase5KeyboardUp(unsigned char key, int x, int y);
void handlePhase5Mouse(int button, int state, int x, int y);
void handlePhase5PassiveMotion(int x, int y);


#endif // PHASE5_H
