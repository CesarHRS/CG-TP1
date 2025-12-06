#ifndef PHASE7_H
#define PHASE7_H

void initPhase7();
void drawPhase7();
void updatePhase7();
void handlePhase7MouseClick(int button, int state, int x, int y);
void handlePhase7MouseMove(int x, int y);
void handlePhase7Keyboard(unsigned char key);
void handlePhase7KeyboardUp(unsigned char key);
void handlePhase7SpecialKey(int key, int x, int y);
void handlePhase7SpecialKeyUp(int key, int x, int y);

#endif // PHASE7_H
