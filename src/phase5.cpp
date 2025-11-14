#include <GL/glut.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "phase5.h"
#include "menu.h"
#include "game.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int windowW = 800;
static int windowH = 600;

static int mouseX5 = 400;
static int mouseY5 = 300;

static int frameCounter5 = 0;

enum ShapeType { SHAPE_CUBE=0, SHAPE_SPHERE=1, SHAPE_CONE=2 };
static const char* shapeNames[] = { "Cubo", "Esfera", "Cone" };

struct Option {
    int shape;
    float x, y;
    float size;
};

static std::vector<Option> options;
static int targetShape = 0;
static int correctCount5 = 0;
static int correctTarget5 = 5;

static void generateQuestion() {
    options.clear();
    // choose target among first three types
    targetShape = rand() % 3; // 0..2

    // build pool with target first then others
    std::vector<int> pool = {SHAPE_CUBE, SHAPE_SPHERE, SHAPE_CONE};
    // shuffle pool
    for (int i = 0; i < 3; ++i) {
        int j = rand() % 3;
        std::swap(pool[i], pool[j]);
    }
    // ensure target is present at pool[0]
    int idx = 0;
    for (int i = 0; i < 3; ++i) if (pool[i] == targetShape) { idx = i; break; }
    std::swap(pool[0], pool[idx]);

    // positions (upper part of screen, above cockpit)
    float areaYMin = windowH * 0.35f;
    float areaYMax = windowH * 0.75f;
    for (int i = 0; i < 3; ++i) {
        Option o;
        o.shape = pool[i];
        o.size = 40.0f + (rand() % 30);
        float segment = (float)(i+1) / 4.0f;
        o.x = windowW * segment + (rand() % 40 - 20);
        o.y = areaYMin + (rand() % (int)(areaYMax - areaYMin));
        options.push_back(o);
    }
}

static void drawParallaxBackground() {
    // three layers of moving stars
    glPointSize(2.0f);
    for (int layer = 0; layer < 3; ++layer) {
        int count = 40 + layer * 20;
        glBegin(GL_POINTS);
        for (int i = 0; i < count; ++i) {
            int sx = (i * 37 * (layer+1) + frameCounter5 * (layer+1) * 3) % windowW;
            int sy = (i * 91 + frameCounter5 * (layer+1) * 2) % windowH;
            float bright = 0.3f + 0.7f * ((i % (5+layer)) / (float)(5+layer));
            glColor3f(bright*0.8f, bright*0.9f, bright);
            glVertex2f(sx, sy);
        }
        glEnd();
    }
}

static void drawShapeAt(const Option &o, bool lit) {
    float bx = o.x;
    float by = o.y;
    float s = o.size;

    glPushMatrix();
    glTranslatef(bx, by, 0.0f);
    if (lit) glColor3f(0.9f, 0.9f, 0.6f);
    else glColor3f(0.4f, 0.45f, 0.5f);

    switch (o.shape) {
        case SHAPE_CUBE:
            glPushMatrix();
            glScalef(s, s, s);
            glutSolidCube(1.0);
            glPopMatrix();
            break;
        case SHAPE_SPHERE:
            glutSolidSphere(s*0.4f, 16, 12);
            break;
        case SHAPE_CONE:
            glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            glutSolidCone(s*0.45f, s*0.9f, 16, 8);
            glPopMatrix();
            break;
        default:
            glutSolidSphere(s*0.4f, 12, 8);
            break;
    }
    glPopMatrix();
}

static void drawProgressBar5() {
    float barWidth = 30.0f;
    float barHeight = windowH * 0.6f;
    float barX = windowW - 40.0f;
    float barY = (windowH - barHeight) / 2.0f;

    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();

    float fill = (float)correctCount5 / (float)correctTarget5;
    if (fill > 1.0f) fill = 1.0f;
    float filledH = barHeight * fill;
    float r = 1.0f - fill;
    float g = 0.2f + 0.8f * fill;
    glColor3f(r, g, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + filledH);
    glVertex2f(barX, barY + filledH);
    glEnd();

    glColor3f(1,1,1);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    glLineWidth(1.0f);
}

void initPhase5() {
    srand((unsigned)time(NULL));
    windowW = windowWidth;
    windowH = windowHeight;
    // Ensure drawPlayer sees same window vars
    windowWidth_game = windowW;
    windowHeight_game = windowH;
    // Init player for panel drawing
    player.width = 200.0f;
    player.height = 90.0f;
    player.x = windowWidth_game / 2.0f;
    player.y = 0.0f;
    player.speed = 15.0f;
    player.health = 100;
    player.maxHealth = 100;
    player.isHit = false;
    player.hitTimer = 0;
    player.shakeOffsetX = 0.0f;
    player.shakeOffsetY = 0.0f;

    correctCount5 = 0;
    frameCounter5 = 0;
    generateQuestion();
    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawPhase5() {
    // Clear color and depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1) background: draw without affecting depth buffer
    glDisable(GL_DEPTH_TEST);
    drawParallaxBackground();

    // 2) ship panel under objects
    drawPlayer();

    // 3) draw 3D objects with depth testing so they render in front
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    float fx = (float)mouseX5;
    float fy = (float)(windowH - mouseY5);

    for (const auto &o : options) {
        float dx = fx - o.x;
        float dy = fy - o.y;
        float dist = sqrtf(dx*dx + dy*dy);
        bool lit = (dist <= 120.0f); // optional: used to color brighter when near cursor
        drawShapeAt(o, lit);
    }

    // 4) HUD overlays
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string goal = "Encontre: ";
    goal += shapeNames[targetShape];
    glRasterPos2f(windowW/2 - goal.size()*6, windowH - 30);
    for (char c : goal) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    drawProgressBar5();

    glutSwapBuffers();
}

void updatePhase5() {
    frameCounter5++;
}

void handlePhase5MouseMove(int x, int y) {
    mouseX5 = x;
    mouseY5 = y;
    glutPostRedisplay();
}

void handlePhase5MouseClick(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    float clickX = (float)x;
    float clickY = (float)(windowH - y);
    for (size_t i = 0; i < options.size(); ++i) {
        Option &o = options[i];
        float dx = clickX - o.x;
        float dy = clickY - o.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist <= o.size) {
            if (o.shape == targetShape) {
                correctCount5++;
                generateQuestion();
                if (correctCount5 >= correctTarget5) {
                    changeState(MAIN_MENU);
                    glutSetCursor(GLUT_CURSOR_INHERIT);
                }
            }
            break;
        }
    }
}

void handlePhase5Keyboard(unsigned char key) {
    if (key == 27) {
        changeState(MAIN_MENU);
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}

void handlePhase5KeyboardUp(unsigned char key) {
    (void)key;
}

void handlePhase5SpecialKey(int key, int x, int y) {
    (void)key; (void)x; (void)y;
}

void handlePhase5SpecialKeyUp(int key, int x, int y) {
    (void)key; (void)x; (void)y;
}
