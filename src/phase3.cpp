#include <GL/glut.h>
#include "phase3.h"
#include "menu.h"
#include "gameover.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

int windowWidthP3 = 800;
int windowHeightP3 = 600;
int mouseXP3 = 400;
int mouseYP3 = 300;

int correctAnswersCountP3 = 0;
int correctAnswersTargetP3 = 10;

CalculatorP3 calcP3 = {"", 0, 0, false};
EquationP3 currentEquationP3 = {0,0,0,'+', false, true};

// keypad layout: 4 rows x 3 cols
const int KEYPAD_ROWS = 4;
const int KEYPAD_COLS = 3;
std::vector<std::string> keypadLabels = {
    "1","2","3",
    "4","5","6",
    "7","8","9",
    "0","DEL","ENT"
};

// Button geometry
// Center the keypad for an 800x600 window
// keypad width = (keyW+keyGap)*KEYPAD_COLS - keyGap = 234
// keypad total panel height = (keyH+keyGap)*KEYPAD_ROWS + 120 = 408
// center coords:
// keypadX = (800 - 234)/2 = 283
// keypadY = (600 - 408)/2 = 96
float keypadX = 283.0f;
float keypadY = 96.0f;
float keyW = 70.0f;
float keyH = 60.0f;
float keyGap = 12.0f;

// forward declarations
void createNewEquationP3();

void drawText(const std::string &s, float x, float y, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : s) glutBitmapCharacter(font, c);
}

void drawCalculatorPanel() {
    // panel background (spaceship panel look)
    glColor3f(0.05f, 0.1f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2f(keypadX - 40, keypadY - 40);
    glVertex2f(keypadX + (keyW+keyGap)*KEYPAD_COLS + 40 - keyGap, keypadY - 40);
    glVertex2f(keypadX + (keyW+keyGap)*KEYPAD_COLS + 40 - keyGap, keypadY + (keyH+keyGap)*KEYPAD_ROWS + 120);
    glVertex2f(keypadX - 40, keypadY + (keyH+keyGap)*KEYPAD_ROWS + 120);
    glEnd();

    // metallic border
    glColor3f(0.2f, 0.25f, 0.28f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(keypadX - 40, keypadY - 40);
    glVertex2f(keypadX + (keyW+keyGap)*KEYPAD_COLS + 40 - keyGap, keypadY - 40);
    glVertex2f(keypadX + (keyW+keyGap)*KEYPAD_COLS + 40 - keyGap, keypadY + (keyH+keyGap)*KEYPAD_ROWS + 120);
    glVertex2f(keypadX - 40, keypadY + (keyH+keyGap)*KEYPAD_ROWS + 120);
    glEnd();
    glLineWidth(1.0f);

    // display area
    float dispW = (keyW+keyGap)*KEYPAD_COLS - keyGap;
    float dispH = 60.0f;
    float dispX = keypadX;
    float dispY = keypadY + (keyH+keyGap)*KEYPAD_ROWS + 40;

    // display background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(dispX, dispY);
    glVertex2f(dispX + dispW, dispY);
    glVertex2f(dispX + dispW, dispY + dispH);
    glVertex2f(dispX, dispY + dispH);
    glEnd();
    glDisable(GL_BLEND);

    // display border
    glColor3f(0.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(dispX, dispY);
    glVertex2f(dispX + dispW, dispY);
    glVertex2f(dispX + dispW, dispY + dispH);
    glVertex2f(dispX, dispY + dispH);
    glEnd();
    glLineWidth(1.0f);

    // display text (right aligned)
    std::string text = calcP3.display.empty() ? "" : calcP3.display;
    if (text.empty()) text = "";
    glColor3f(0.9f, 0.9f, 0.9f);
    float tx = dispX + 10.0f;
    float ty = dispY + dispH/2.0f - 6.0f;
    drawText(text, tx, ty);

    // keypad buttons
    for (int r = 0; r < KEYPAD_ROWS; ++r) {
        for (int c = 0; c < KEYPAD_COLS; ++c) {
            int idx = r * KEYPAD_COLS + c;
            float bx = keypadX + c * (keyW + keyGap);
            float by = keypadY + (KEYPAD_ROWS - 1 - r) * (keyH + keyGap);

            // button background
            glColor3f(0.12f, 0.18f, 0.2f);
            glBegin(GL_QUADS);
            glVertex2f(bx, by);
            glVertex2f(bx + keyW, by);
            glVertex2f(bx + keyW, by + keyH);
            glVertex2f(bx, by + keyH);
            glEnd();

            // border
            glColor3f(0.0f, 1.0f, 0.8f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(bx, by);
            glVertex2f(bx + keyW, by);
            glVertex2f(bx + keyW, by + keyH);
            glVertex2f(bx, by + keyH);
            glEnd();
            glLineWidth(1.0f);

            // label
            glColor3f(1.0f, 1.0f, 1.0f);
            float lx = bx + keyW/2.0f - keypadLabels[idx].size()*4.5f;
            float ly = by + keyH/2.0f - 6.0f;
            drawText(keypadLabels[idx], lx, ly);
        }
    }

    // decorative detail removed to keep cleaner calculator look
}

// utility: check which key was clicked, or -1 if none
int hitTestKey(int mx, int my) {
    for (int r = 0; r < KEYPAD_ROWS; ++r) {
        for (int c = 0; c < KEYPAD_COLS; ++c) {
            int idx = r * KEYPAD_COLS + c;
            float bx = keypadX + c * (keyW + keyGap);
            float by = keypadY + (KEYPAD_ROWS - 1 - r) * (keyH + keyGap);
            if (mx >= bx && mx <= bx + keyW && my >= by && my <= by + keyH) return idx;
        }
    }
    return -1;
}

void createNewEquationP3() {
    // random op among + - * /
    int opIdx = rand() % 4;
    char op = '+';
    if (opIdx == 0) op = '+';
    else if (opIdx == 1) op = '-';
    else if (opIdx == 2) op = '*';
    else op = '/';

    int a=0,b=0,res=0;

    if (op == '+') {
        a = rand() % 50 + 1;
        b = rand() % 50 + 1;
        res = a + b;
    } else if (op == '-') {
        a = rand() % 50 + 1;
        b = rand() % 50 + 1;
        if (a < b) std::swap(a,b);
        res = a - b;
    } else if (op == '*') {
        a = rand() % 12 + 1;
        b = rand() % 12 + 1;
        res = a * b;
    } else { // division, ensure integer result
        b = rand() % 12 + 1; // divisor
        int q = rand() % 12 + 1; // quotient
        a = b * q;
        res = q;
    }

    // decide which is variable: 0 => x is variable, 1 => y is variable
    bool xVar = (rand()%2)==0;

    if (xVar) {
        currentEquationP3.xIsVariable = true;
        currentEquationP3.x = -1; // variable
        currentEquationP3.y = b;
    } else {
        currentEquationP3.xIsVariable = false;
        currentEquationP3.x = a;
        currentEquationP3.y = -1;
    }
    currentEquationP3.op = op;
    currentEquationP3.result = res;
    currentEquationP3.active = true;

    calcP3.display = "";
}

void initPhase3() {
    srand(time(0));
    windowWidthP3 = 800;
    windowHeightP3 = 600;
    mouseXP3 = windowWidthP3/2;
    mouseYP3 = windowHeightP3/2;
    calcP3.display = "";
    calcP3.enterPressed = false;
    createNewEquationP3();
    setGameOver(false);
    correctAnswersCountP3 = 0;

    // register game over callbacks
    initGameOver(windowWidthP3, windowHeightP3);
    registerRestartCallback(restartPhase3);
    registerMenuCallback(returnToMenuFromPhase3);

    glutSetCursor(GLUT_CURSOR_NONE);
}

void drawEquationP3() {
    if (!currentEquationP3.active) return;

    // Draw the equation at top-left of the calculator display area
    float eqX = keypadX + 10.0f;
    float eqY = keypadY + (keyH+keyGap)*KEYPAD_ROWS + 110.0f;

    std::ostringstream oss;
    if (currentEquationP3.xIsVariable) {
        oss << "X " << currentEquationP3.op << " " << currentEquationP3.y << " = " << currentEquationP3.result;
    } else {
        oss << currentEquationP3.x << " " << currentEquationP3.op << " Y = " << currentEquationP3.result;
    }

    glColor3f(1.0f,1.0f,1.0f);
    drawText(oss.str(), eqX, eqY);
}

void drawCongratulations() {
    glColor3f(0.0f,1.0f,0.0f);
    drawText("Parabens! Conta correta.", keypadX, keypadY - 80.0f, GLUT_BITMAP_HELVETICA_18);
}



void drawProgressBarP3() {
    // position to the right of the display
    float dispW = (keyW+keyGap)*KEYPAD_COLS - keyGap;
    float barX = keypadX + dispW + 30.0f;
    float barY = keypadY + (keyH+keyGap)*KEYPAD_ROWS + 60.0f;
    float barW = 140.0f;
    float barH = 20.0f;

    // background
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barW, barY);
    glVertex2f(barX + barW, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();

    // filled portion
    float fraction = 0.0f;
    if (correctAnswersTargetP3 > 0) fraction = (float)correctAnswersCountP3 / (float)correctAnswersTargetP3;
    if (fraction > 1.0f) fraction = 1.0f;

    // color from red to green
    float r = 1.0f - fraction;
    float g = 0.2f + 0.8f * fraction;
    glColor3f(r, g, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barW * fraction, barY);
    glVertex2f(barX + barW * fraction, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();

    // border
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barW, barY);
    glVertex2f(barX + barW, barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();
    glLineWidth(1.0f);

    // text
    std::stringstream ss;
    ss << correctAnswersCountP3 << "/" << correctAnswersTargetP3;
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(ss.str(), barX + barW/2 - 12.0f, barY + barH/2 + 5.0f, GLUT_BITMAP_HELVETICA_12);
}

void drawPhase3() {
    // background
    glColor3f(0.02f, 0.02f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(0,0);
    glVertex2f(windowWidthP3,0);
    glVertex2f(windowWidthP3,windowHeightP3);
    glVertex2f(0,windowHeightP3);
    glEnd();

    drawCalculatorPanel();
    drawEquationP3();

    // draw custom pointer centered on current mouse
    // convert mouse coords (mouseXP3, mouseYP3) to screen coords used by drawSpacePointer in menu
    // mouseYP3 is already window coords with origin top-left in handlers, so reuse logic similar to menu
    // We'll draw a simplified pointer here
    float px = (float)mouseXP3;
    float py = (float)(windowHeightP3 - mouseYP3);
    // glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 5; i >= 1; --i) {
        float a = 0.06f * i;
        glColor4f(0.0f, 0.7f, 1.0f, a);
        float r = (float)(i * 3 + 6);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(px, py);
        for (int s = 0; s <= 20; ++s) {
            float aang = (float)s / 20.0f * 2.0f * M_PI;
            glVertex2f(px + cos(aang) * r, py + sin(aang) * r);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
    // triangular pointer
    float size = 10.0f;
    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(px, py);
    glVertex2f(px + size, py - size * 0.6f);
    glVertex2f(px + size * 0.2f, py + size * 0.2f);
    glEnd();

    // draw progress (visual bar only)
    drawProgressBarP3();

    if (getGameOver() && isVictory) {
        drawCongratulations();
    }

    drawGameOver();
}

void updatePhase3() {
    // no continuous updates needed
}

void handlePhase3MouseMove(int x, int y) {
    mouseXP3 = x;
    mouseYP3 = y;
    glutPostRedisplay();
}

void handlePhase3MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !getGameOver()) {
        // convert y
        int my = windowHeightP3 - y;
        int mx = x;
        int hit = hitTestKey(mx, my);
        if (hit >= 0) {
            std::string label = keypadLabels[hit];
            if (label == "DEL") {
                if (!calcP3.display.empty()) calcP3.display.pop_back();
            } else if (label == "ENT") {
                // process enter
                if (!calcP3.display.empty()) {
                    int val = atoi(calcP3.display.c_str());
                    int expected = 0;
                    if (currentEquationP3.xIsVariable) {
                        expected = currentEquationP3.x; // placeholder
                        // compute the unknown X from equation: X op y = result
                        int yv = currentEquationP3.y;
                        char op = currentEquationP3.op;
                        if (op == '+') expected = currentEquationP3.result - yv;
                        else if (op == '-') expected = currentEquationP3.result + yv; // X - y = res => X = res + y
                        else if (op == '*') expected = currentEquationP3.result / yv;
                        else if (op == '/') expected = currentEquationP3.result * yv;
                    } else {
                        int xv = currentEquationP3.x;
                        char op = currentEquationP3.op;
                        if (op == '+') expected = currentEquationP3.result - xv;
                        else if (op == '-') expected = xv - currentEquationP3.result; // x - Y = res => Y = x - res
                        else if (op == '*') expected = currentEquationP3.result / xv;
                        else if (op == '/') expected = xv / currentEquationP3.result;
                    }

                    if (val == expected) {
                        // correct: increment counter and check target
                        correctAnswersCountP3++;
                        if (correctAnswersCountP3 >= correctAnswersTargetP3) {
                            setGameOver(true);
                            setVictory(true);
                        } else {
                            // next equation
                            createNewEquationP3();
                        }
                    } else {
                        // incorrect -> create new equation
                        createNewEquationP3();
                    }
                }
            } else {
                // digit
                if (calcP3.display.size() < 6) {
                    calcP3.display += label;
                }
            }
        }

        glutPostRedisplay();
    }
}

void handlePhase3Keyboard(unsigned char key) {
    if (getGameOver()) {
        // forward to gameover handler
        handleGameOverKeyboard(key);
        return;
    }

    if (key >= '0' && key <= '9') {
        if (calcP3.display.size() < 6) calcP3.display.push_back(key);
        glutPostRedisplay();
    } else if (key == 8 || key == 127) {
        if (!calcP3.display.empty()) calcP3.display.pop_back();
        glutPostRedisplay();
    } else if (key == 13 || key == '\r') {
        // enter
        if (!calcP3.display.empty()) {
            int val = atoi(calcP3.display.c_str());
            int expected = 0;
            if (currentEquationP3.xIsVariable) {
                int yv = currentEquationP3.y;
                char op = currentEquationP3.op;
                if (op == '+') expected = currentEquationP3.result - yv;
                else if (op == '-') expected = currentEquationP3.result + yv;
                else if (op == '*') expected = currentEquationP3.result / yv;
                else if (op == '/') expected = currentEquationP3.result * yv;
            } else {
                int xv = currentEquationP3.x;
                char op = currentEquationP3.op;
                if (op == '+') expected = currentEquationP3.result - xv;
                else if (op == '-') expected = xv - currentEquationP3.result;
                else if (op == '*') expected = currentEquationP3.result / xv;
                else if (op == '/') expected = xv / currentEquationP3.result;
            }

            if (val == expected) {
                correctAnswersCountP3++;
                if (correctAnswersCountP3 >= correctAnswersTargetP3) {
                    setGameOver(true);
                    setVictory(true);
                } else {
                    createNewEquationP3();
                }
            } else {
                createNewEquationP3();
            }
        }
        glutPostRedisplay();
    } else if (key == 27) { // ESC => back to menu
        returnToMenuFromPhase3();
        glutPostRedisplay();
    }
}

void handlePhase3KeyboardUp(unsigned char key) {
    // not used currently
}

void restartPhase3() {
    initPhase3();
}

void returnToMenuFromPhase3() {
    glutSetCursor(GLUT_CURSOR_INHERIT);
    glutPassiveMotionFunc(handleMouseHover);
    currentState = MAIN_MENU;
}
