#ifndef GAMEOVER_H
#define GAMEOVER_H

// Callback function types para permitir reiniciar fase específica
typedef void (*RestartCallback)();
typedef void (*MenuCallback)();

// Variáveis globais do Game Over
extern int windowWidth_gameover;
extern int windowHeight_gameover;
extern bool isGameOver;
extern RestartCallback onRestartCallback;
extern MenuCallback onMenuCallback;

// Funções do Game Over
void initGameOver(int windowWidth, int windowHeight);
void drawGameOver();
void handleGameOverKeyboard(unsigned char key);
void setGameOver(bool active);
bool getGameOver();

// Funções para registrar callbacks de cada fase
void registerRestartCallback(RestartCallback callback);
void registerMenuCallback(MenuCallback callback);

#endif // GAMEOVER_H
