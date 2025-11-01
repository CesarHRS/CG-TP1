#ifndef GAMEOVER_H
#define GAMEOVER_H

// Callback function types para permitir reiniciar fase específica
typedef void (*RestartCallback)();
typedef void (*MenuCallback)();
typedef void (*NextPhaseCallback)(); // Callback para próxima fase

// Variáveis globais do Game Over
extern int windowWidth_gameover;
extern int windowHeight_gameover;
extern bool isGameOver;
extern bool isVictory; // Se é vitória ou derrota
extern RestartCallback onRestartCallback;
extern MenuCallback onMenuCallback;
extern NextPhaseCallback onNextPhaseCallback; // Callback para próxima fase
extern int victoryPhase; // Identificador da fase que causou a vitória (1..n)

// Funções do Game Over
void initGameOver(int windowWidth, int windowHeight);
void drawGameOver();
void handleGameOverKeyboard(unsigned char key);
void setGameOver(bool active);
void setVictory(bool victory); // Definir se é vitória
void setVictoryPhase(int phase); // Definir a fase da vitória
bool getGameOver();

// Funções para registrar callbacks de cada fase
void registerRestartCallback(RestartCallback callback);
void registerMenuCallback(MenuCallback callback);
void registerNextPhaseCallback(NextPhaseCallback callback); // Registrar callback de próxima fase

#endif // GAMEOVER_H
