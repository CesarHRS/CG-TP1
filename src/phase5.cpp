#include <GL/glut.h>
#include <cmath>
#include <string>
#include "phase5.h"
#include "menu.h"
#include "game.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int windowW = 800;
static int windowH = 600;

void initPhase5() {
    windowW = windowWidth;
    windowH = windowHeight;
    // ensure drawPlayer (if ever used) sees correct values
    windowWidth_game = windowW;
    windowHeight_game = windowH;

    // initialize a basic player state to keep panel consistent
    player.width = 200.0f;
    player.height = windowH * 0.30f; // panel height matches 30% target
    player.x = windowW / 2.0f;
    player.y = 0.0f;
    player.speed = 0.0f;
    player.health = player.maxHealth = 100;
    player.isHit = false;
    player.hitTimer = 0;
    player.shakeOffsetX = player.shakeOffsetY = 0.0f;

    // hide cursor for this phase (like other phases)
    glutSetCursor(GLUT_CURSOR_NONE);
}

// draw a simple 3D panel occupying the bottom 30% of the screen
void drawPhase5() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Uniform illumination: just use solid colors (no lighting) and no vignette
    glDisable(GL_LIGHTING);

    // Enable depth so the panel looks slightly 3D
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float panelH = windowH * 0.30f; // 30% of display height
    float panelW = windowW * 0.98f; // almost full width
    float marginX = (windowW - panelW) * 0.5f;
    float frontZ = 0.0f;
    float backZ = -panelH * 0.2f;

    float x0 = marginX;
    float x1 = marginX + panelW;
    float y0 = 0.0f;
    float y1 = panelH;

    // Front face - polygonal, with angled sides to match the provided reference
    glColor3f(0.22f, 0.28f, 0.55f);
    glBegin(GL_POLYGON);
    glVertex3f(x0 + panelW*0.02f, y0 + panelH*0.15f, frontZ);
    glVertex3f(x0 + panelW*0.18f, y0 + panelH*0.88f, frontZ);
    glVertex3f(windowW*0.5f - panelW*0.02f, y0 + panelH*0.98f, frontZ);
    glVertex3f(windowW*0.5f + panelW*0.02f, y0 + panelH*0.98f, frontZ);
    glVertex3f(x1 - panelW*0.18f, y0 + panelH*0.88f, frontZ);
    glVertex3f(x1 - panelW*0.02f, y0 + panelH*0.15f, frontZ);
    glVertex3f(x1, y0, frontZ);
    glVertex3f(x0, y0, frontZ);
    glEnd();

    // Top slanted surface (give subtle extrusion)
    glColor3f(0.18f, 0.22f, 0.42f);
    glBegin(GL_QUADS);
    glVertex3f(x0 + panelW*0.18f, y0 + panelH*0.88f, frontZ);
    glVertex3f(x1 - panelW*0.18f, y0 + panelH*0.88f, frontZ);
    glVertex3f(x1 - panelW*0.12f, y0 + panelH*0.98f, backZ);
    glVertex3f(x0 + panelW*0.12f, y0 + panelH*0.98f, backZ);
    glEnd();

    // Side fins (left/right)
    glColor3f(0.16f, 0.18f, 0.36f);
    glBegin(GL_TRIANGLES);
    glVertex3f(x0, y0, frontZ);
    glVertex3f(x0 + panelW*0.02f, y0 + panelH*0.15f, frontZ);
    glVertex3f(x0 + panelW*0.06f, y0 + panelH*0.05f, backZ);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y0, frontZ);
    glVertex3f(x1 - panelW*0.02f, y0 + panelH*0.15f, frontZ);
    glVertex3f(x1 - panelW*0.06f, y0 + panelH*0.05f, backZ);
    glEnd();

    // Cockpit glass - polygonal reflective patch
    float glassW = panelW * 0.36f;
    float gx0 = windowW*0.5f - glassW*0.5f;
    float gx1 = gx0 + glassW;
    float gy0 = panelH*0.3f;
    float gy1 = panelH*0.78f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.65f, 0.92f, 0.98f, 0.95f);
    glBegin(GL_POLYGON);
    glVertex3f(gx0, gy0, frontZ + 0.6f);
    glVertex3f(gx1, gy0, frontZ + 0.6f);
    glVertex3f(gx1 - panelW*0.02f, gy1, frontZ + 0.6f);
    glVertex3f(gx0 + panelW*0.02f, gy1, frontZ + 0.6f);
    glEnd();
    glDisable(GL_BLEND);

    // subtle rim highlight along top edge
    glColor3f(0.12f, 0.16f, 0.32f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x0 + panelW*0.02f, y0 + panelH*0.15f, frontZ + 0.1f);
    glVertex3f(x0 + panelW*0.18f, y0 + panelH*0.88f, frontZ + 0.1f);
    glVertex3f(windowW*0.5f, y0 + panelH*0.98f, frontZ + 0.1f);
    glVertex3f(x1 - panelW*0.18f, y0 + panelH*0.88f, frontZ + 0.1f);
    glVertex3f(x1 - panelW*0.02f, y0 + panelH*0.15f, frontZ + 0.1f);
    glEnd();

    // =================================================================
    // 3D keyboard keys placed on top of the panel
    // Draw a centered grid of small extruded keys (top face + thin sides)
    // =================================================================
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    // key grid parameters: use 4x8 and place INSIDE the glass rectangle (gx0..gx1, gy0..gy1)
    const int rows = 4;
    const int cols = 8;
    // make the keys area fit comfortably inside the glass patch
    float glassH = gy1 - gy0;
    float keysAreaW = glassW * 0.92f; // slightly inset from glass edges
    float keysAreaH = glassH * 0.60f; // occupy ~60% of glass height
    float keysOriginX = gx0 + (glassW - keysAreaW) * 0.5f;
    float keysOriginY = gy0 + (glassH - keysAreaH) * 0.5f;

    float keyGapX = keysAreaW * 0.012f;
    float keyGapY = keysAreaH * 0.10f;
    float keyW = (keysAreaW - (cols+1)*keyGapX) / cols;
    float keyH = (keysAreaH - (rows+1)*keyGapY) / rows;

    // Use small Z offsets inside the orthographic near/far (-1..1)
    // Keep keys in front of the glass but within [-1,1]
    float keyTopZ = frontZ + 0.78f; // slightly in front of glass (~+0.6f)
    float keyDepth = 0.26f; // increased extrusion depth (keys appear taller)
    float keyBottomZ = keyTopZ - keyDepth;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float kx0 = keysOriginX + keyGapX + c * (keyW + keyGapX);
            float kx1 = kx0 + keyW;
            float ky0 = keysOriginY + keyGapY + r * (keyH + keyGapY);
            float ky1 = ky0 + keyH;

            // key top face
            glColor3f(0.92f, 0.92f, 0.94f); // light key top
            glBegin(GL_QUADS);
            glVertex3f(kx0, ky0, keyTopZ);
            glVertex3f(kx1, ky0, keyTopZ);
            glVertex3f(kx1, ky1, keyTopZ);
            glVertex3f(kx0, ky1, keyTopZ);
            glEnd();

            // key sides (4 quads) - give a slightly darker rim
            glColor3f(0.55f, 0.56f, 0.60f);
            // front face (towards the viewer)
            glBegin(GL_QUADS);
            glVertex3f(kx0, ky0, keyTopZ);
            glVertex3f(kx1, ky0, keyTopZ);
            glVertex3f(kx1, ky0, keyBottomZ);
            glVertex3f(kx0, ky0, keyBottomZ);
            glEnd();

            // back face
            glBegin(GL_QUADS);
            glVertex3f(kx0, ky1, keyTopZ);
            glVertex3f(kx1, ky1, keyTopZ);
            glVertex3f(kx1, ky1, keyBottomZ);
            glVertex3f(kx0, ky1, keyBottomZ);
            glEnd();

            // left face
            glBegin(GL_QUADS);
            glVertex3f(kx0, ky0, keyTopZ);
            glVertex3f(kx0, ky1, keyTopZ);
            glVertex3f(kx0, ky1, keyBottomZ);
            glVertex3f(kx0, ky0, keyBottomZ);
            glEnd();

            // right face
            glBegin(GL_QUADS);
            glVertex3f(kx1, ky0, keyTopZ);
            glVertex3f(kx1, ky1, keyTopZ);
            glVertex3f(kx1, ky1, keyBottomZ);
            glVertex3f(kx1, ky0, keyBottomZ);
            glEnd();

            // small inset label (dark rectangle) to simulate key cap legend
            float inset = 0.14f * keyW;
            glColor3f(0.12f, 0.14f, 0.18f);
            glBegin(GL_QUADS);
            float labelZ = keyTopZ + 0.05f; // keep within clip range
            glVertex3f(kx0 + inset, ky0 + inset, labelZ);
            glVertex3f(kx1 - inset, ky0 + inset, labelZ);
            glVertex3f(kx1 - inset, ky1 - inset, labelZ);
            glVertex3f(kx0 + inset, ky1 - inset, labelZ);
            glEnd();
        }
    }
    glPopAttrib();

    // Done: HUD overlays removed for a clean panel-only view
    glutSwapBuffers();
}

void updatePhase5() {
    // nothing dynamic yet
}

void handlePhase5MouseMove(int x, int y) { (void)x; (void)y; }
void handlePhase5MouseClick(int button, int state, int x, int y) { (void)button; (void)state; (void)x; (void)y; }
void handlePhase5Keyboard(unsigned char key) { if (key == 27) { changeState(MAIN_MENU); glutSetCursor(GLUT_CURSOR_INHERIT); } }
void handlePhase5KeyboardUp(unsigned char key) { (void)key; }
void handlePhase5SpecialKey(int key, int x, int y) { (void)key; (void)x; (void)y; }
void handlePhase5SpecialKeyUp(int key, int x, int y) { (void)key; (void)x; (void)y; }
