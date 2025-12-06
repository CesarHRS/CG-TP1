#include <GL/glut.h>
#include "phase6.h"
#include "menu.h"

void initPhase6() {
    // Placeholder initialization for Phase 6
    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawPhase6() {
    // Simple placeholder: draw a message so the phase can be selected and tested
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(220, 300, "FASE 6 - Placeholder (Exploration / Repair)");
    glutSwapBuffers();
}

void updatePhase6() {
    // nothing yet
}

void handlePhase6MouseClick(int button, int state, int x, int y) {
    (void)button; (void)state; (void)x; (void)y;
}
void handlePhase6MouseMove(int x, int y) { (void)x; (void)y; }
void handlePhase6Keyboard(unsigned char key) { if (key == 27) { changeState(MAIN_MENU); glutSetCursor(GLUT_CURSOR_INHERIT); } }
void handlePhase6KeyboardUp(unsigned char key) { (void)key; }
void handlePhase6SpecialKey(int key, int x, int y) { (void)key; (void)x; (void)y; }
void handlePhase6SpecialKeyUp(int key, int x, int y) { (void)key; (void)x; (void)y; }
