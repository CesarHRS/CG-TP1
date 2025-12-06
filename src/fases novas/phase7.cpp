#include <GL/glut.h>
#include "phase7.h"
#include "menu.h"

void initPhase7() {
    // Placeholder initialization for Phase 7 (boss)
    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawPhase7() {
    // Placeholder boss screen: show boss label and phase title
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0f, 0.8f, 0.2f);
    drawText(220, 340, "FASE 7 - CHEF\xC3\x83O: SENHOR DOS DETRITOS");
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(220, 300, "Placeholder: implement boss fight here.");
    glutSwapBuffers();
}

void updatePhase7() {
    // nothing yet
}

void handlePhase7MouseClick(int button, int state, int x, int y) { (void)button; (void)state; (void)x; (void)y; }
void handlePhase7MouseMove(int x, int y) { (void)x; (void)y; }
void handlePhase7Keyboard(unsigned char key) { if (key == 27) { changeState(MAIN_MENU); glutSetCursor(GLUT_CURSOR_INHERIT); } }
void handlePhase7KeyboardUp(unsigned char key) { (void)key; }
void handlePhase7SpecialKey(int key, int x, int y) { (void)key; (void)x; (void)y; }
void handlePhase7SpecialKeyUp(int key, int x, int y) { (void)key; (void)x; (void)y; }
