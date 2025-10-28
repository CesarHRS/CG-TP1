#include <GL/glut.h>
#include "menu.h"
#include "game.h"
#include "phase2.h"
#include "phase3.h"
#include "gameover.h"
#include <string.h>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif




GameState currentState = MAIN_MENU;
int windowWidth = 800;
int windowHeight = 600;
// menu uses system cursor; custom pointer is drawn only in phase3
int menuMouseX = 400;
int menuMouseY = 300;

// Story overlay state: when true, a space-background overlay with text is shown
bool showPhaseStory = false;
int storyPhase = 0; // 1,2,3
int storyPage = 0; // pagination index (0-based, two paragraphs per page)

Button startButton = {300, 390, 200, 50, "Fase 1", false};
Button phase2Button = {300, 320, 200, 50, "Fase 2", false};
Button phase3Button = {300, 250, 200, 50, "Fase 3", false};
Button instructionsButton = {300, 190, 200, 50, "Como Jogar", false};
Button exitButton = {300, 130, 200, 50, "Sair", false};
Button backButton = {300, 80, 200, 50, "Voltar para o Menu", false};

void handleKeyboardUp(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    if (currentState == GAME_SCREEN) {
        handleGameKeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        handlePhase2KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        handlePhase3KeyboardUp(key);
        glutPostRedisplay();
    }
}


// custom pointer removed from menu; phase3 draws its own pointer

void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawMenuPointer(int mx, int my) {
    float px = (float)mx;
    float py = (float)(windowHeight - my);
    float size = 10.0f;
    // glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.0f, 0.7f, 1.0f, 0.12f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(px, py);
    for (int i = 0; i <= 20; ++i) {
        float a = (float)i/20.0f * 2.0f * M_PI;
        glVertex2f(px + cos(a) * (size+6.0f), py + sin(a) * (size+6.0f));
    }
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(px, py);
    glVertex2f(px + size, py - size * 0.6f);
    glVertex2f(px + size * 0.2f, py + size * 0.2f);
    glEnd();
}

void drawButton(Button &button) {
    float radius = button.height / 2.0f;
    int segments = 20;
    if (button.isHovered) {
        glColor3f(0.7f, 0.7f, 0.9f);
    } else {
        glColor3f(0.5f, 0.5f, 0.8f);
    }
    glBegin(GL_POLYGON);
    glVertex2f(button.x + radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y + button.height);
    glVertex2f(button.x + radius, button.y + button.height);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = M_PI; // 180 graus
        theta += (M_PI * i) / segments;
        glVertex2f(button.x + radius + radius * cos(theta), button.y + radius + radius * sin(theta));
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = 0.0f;
        theta += (M_PI * i) / segments;
        glVertex2f(button.x + button.width - radius + radius * cos(theta), button.y + radius + radius * sin(theta));
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    float textX = button.x + (button.width - strlen(button.label) * 9) / 2.0f;
    float textY = button.y + (button.height / 2.0f) + 5;
    drawText(textX, textY, button.label);
}

bool isMouseOverButton(int x, int y, Button &button) {
    int invertedY = windowHeight - y;
    return (x >= button.x && x <= button.x + button.width &&
            invertedY >= button.y && invertedY <= button.y + button.height);
}


// --- Funções de Renderização das Telas ---

void drawMainMenu() {
    drawText(320, 500, "Jogo de navinha mais legal da sua vida");
    drawButton(startButton);
    drawButton(phase2Button);
    drawButton(phase3Button);
    drawButton(instructionsButton);
    drawButton(exitButton);
}

static std::vector<std::string> getStoryParagraphs(int phase) {
    std::vector<std::string> p;
    if (phase == 1) {
        p.push_back("FASE 1");
        p.push_back("[ AMEAÇA DE IMPACTO ]");
        p.push_back("CONTROLE DE MISSÃO: Controle para Comandante. Solicita-se confirmação de escuta.");
        p.push_back("COMANDANTE: Confirmo a escuta.");
        p.push_back("CONTROLE DE MISSÃO: Nossos sistemas de varredura profunda detectaram múltiplos objetos de alta massa.");
        p.push_back("CONTROLE de MISSÃO: Análise de telemetria confirma: Assinatura de asteroides em trajetória de colisão direta. Janela de interceptação estimada: T-menos 15 minutos.");
        p.push_back("CONTROLE DE MISSÃO: Diretriz: Decolagem imediata da nave Papa Alpha Romeo Gamma. Objetivo primário: Interceptar e neutralizar todas as ameaças antes da reentrada atmosférica.");
        p.push_back("COMANDANTE: Confirmado, interceptar e neutralizar todas as ameaças antes da reentrada atmosférica.");
    } else if (phase == 2) {
        p.push_back("FASE 2");
        p.push_back("[ ANOMALIA DE SENSOR ]");
        p.push_back("CONTROLE DE MISSÃO: Papa Alpha Romeo Gamma, mantenha posição.");
        p.push_back("CONTROLE DE MISSÃO: Detectamos uma anomalia. O fluxo de asteroides persiste, porém os dados de telemetria dos alvos estão corrompidos. Suspeita de interferência externa.");
        p.push_back("CONTROLE DE MISSÃO: O sistema de aquisição automática de alvos está inoperante.");
        p.push_back("CONTROLE DE MISSÃO: Diretriz: Desative o bloqueio automático. Acione o sistema de mira manual. Utilize o visor de aquisição direta para calcular os vetores de interceptação. Prossiga com a missão.");
        p.push_back("COMANDANTE: Confirmado, desabilitando sistema automático, e engajando modo manual.");
    } else if (phase == 3) {
        p.push_back("FASE 3");
        p.push_back("[ MUDANÇA DE DIRETRIZ ]");
        p.push_back("CONTROLE DE MISSÃO: Papa Alpha Romeo Gamma, recebemos seu pedido de retorno à base para reabastecimento. Aguarde autorização.");
        p.push_back("CONTROLE DE MISSÃO: ...Espere. Alerta. O radar foi restabelecido. A fonte da interferência foi localizada.");
        p.push_back("CONTROLE DE MISSÃO: Detectamos um contato único, não identificado, em alta velocidade. Assinatura energética desconhecida. O contato viola o perímetro de defesa orbital.");
        p.push_back("CONTROLE DE MISSÃO: Análise confirma: Este alvo é a fonte da anomalia e o ponto de origem dos asteroides.");
        p.push_back("CONTROLE DE MISSÃO: Pedido de retorno à base negado. Nova diretriz: Reciclar a munição previamente gasta, e interceptar e identificar o contato hostil. Prioridade máxima.");
    }
    return p;
}

static std::vector<std::string> wrapText(const std::string &text, int maxChars) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word;
    std::string line;
    while (iss >> word) {
        if (line.empty()) line = word;
        else if ((int)line.length() + 1 + (int)word.length() <= maxChars) line += " " + word;
        else {
            lines.push_back(line);
            line = word;
        }
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}

void drawStoryOverlay() {
    // Draw a dark space-like background (gradient)
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.06f);
    glVertex2f(0, 0);
    glColor3f(0.0f, 0.03f, 0.12f);
    glVertex2f(windowWidth, 0);
    glColor3f(0.0f, 0.01f, 0.04f);
    glVertex2f(windowWidth, windowHeight);
    glColor3f(0.01f, 0.01f, 0.03f);
    glVertex2f(0, windowHeight);
    glEnd();

    // Stars
    srand(1000 + storyPhase);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 120; ++i) {
        float sx = (float)(rand() % windowWidth);
        float sy = (float)(rand() % windowHeight);
        float bright = (rand() % 100) / 200.0f + 0.5f;
        glColor3f(bright, bright, bright);
        glVertex2f(sx, sy);
    }
    glEnd();

    // Panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    float pad = 40.0f;
    float panelW = windowWidth - pad * 2;
    float panelH = windowHeight - pad * 2;
    glBegin(GL_QUADS);
    glVertex2f(pad, pad);
    glVertex2f(pad + panelW, pad);
    glVertex2f(pad + panelW, pad + panelH);
    glVertex2f(pad, pad + panelH);
    glEnd();
    glDisable(GL_BLEND);

    // Paragraphs and pagination
    std::vector<std::string> paras = getStoryParagraphs(storyPhase);
    int pages = (paras.size() + 1) / 2;
    if (storyPage < 0) storyPage = 0;
    if (storyPage >= pages) storyPage = pages - 1;

    int startIdx = storyPage * 2;
    int maxChars = 70;

    // Render the two paragraphs for this page, centered vertically
    std::vector<std::string> renderedLines;
    for (int i = 0; i < 2; ++i) {
        int idx = startIdx + i;
        if (idx >= (int)paras.size()) break;
        std::vector<std::string> wrapped = wrapText(paras[idx], maxChars);
        // Add a blank line between paragraphs
        if (!renderedLines.empty()) renderedLines.push_back("");
        for (auto &ln : wrapped) renderedLines.push_back(ln);
    }

    // Calculate starting Y to center the block
    float lineHeight = 22.0f;
    float totalH = renderedLines.size() * lineHeight;
    float startY = windowHeight / 2.0f + totalH / 2.0f - 10.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < renderedLines.size(); ++i) {
        const std::string &s = renderedLines[i];
        float textWidth = s.length() * 9.0f;
        float x = windowWidth / 2.0f - textWidth / 2.0f;
        float y = startY - (float)i * lineHeight;
        glRasterPos2f(x, y);
        for (char c : s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Hint depending on whether there are more pages
    std::string hint;
    if (storyPage < pages - 1) hint = "(Clique com o botão direito para mostrar mais)";
    else hint = "(Clique com o botão direito para iniciar a fase)";
    float hintW = hint.length() * 9.0f;
    glColor3f(0.8f, 0.8f, 0.8f);
    glRasterPos2f(windowWidth / 2.0f - hintW / 2.0f, pad + 20.0f);
    for (char c : hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}
void drawInstructionsScreen() {
    drawText(330, 560, "Como Jogar");
    
    // Instruções gerais
    drawText(50, 520, "CONTROLES GERAIS:");
    drawText(70, 500, "- Pressione ESC para pausar o jogo e acessar o menu de pausa");
    drawText(70, 480, "- Use as setas para navegar nos menus");
    
    // Fase 1
    drawText(50, 450, "FASE 1 - Defesa Espacial:");
    drawText(70, 430, "- Use A/D para mover a nave para esquerda/direita");
    drawText(70, 410, "- Clique nos asteroides para resolver a equacao");
    drawText(70, 390, "- Digite a resposta e pressione ENTER");
    drawText(70, 370, "- Complete 10 acertos para avancar para a Fase 2");
    
    // Fase 2
    drawText(50, 340, "FASE 2 - Tiro Certeiro:");
    drawText(70, 320, "- Use o mouse para mirar nos asteroides");
    drawText(70, 300, "- Clique para atirar (5 tiros por rodada)");
    drawText(70, 280, "- Acerte o asteroide com a resposta correta da equacao");
    drawText(70, 260, "- Complete 10 acertos para avancar para a Fase 3");
    
    // Fase 3
    drawText(50, 230, "FASE 3 - Bomba Instavel:");
    drawText(70, 210, "- Use a calculadora clicavel para resolver a equacao da bomba");
    drawText(70, 190, "- Voce tem 20 segundos antes da bomba explodir");
    drawText(70, 170, "- Complete 10 bombas desarmadas para vencer o jogo!");
    
    drawButton(backButton);
}

// --- Funções Principais (Callbacks) ---

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (currentState) {
        case MAIN_MENU:
            if (showPhaseStory) {
                drawStoryOverlay();
            } else {
                drawMainMenu();
            }
            break;
        case INSTRUCTIONS_SCREEN:
            drawInstructionsScreen();
            break;
        case GAME_SCREEN:
            drawGame(); 
            break;
        case PHASE2_SCREEN:
            drawPhase2();
            break;
        case PHASE3_SCREEN:
            drawPhase3();
            break;
            break;
    }

    glutSwapBuffers();
}

void updateScene() {
    if (currentState == GAME_SCREEN) {
        updateGame();
        glutPostRedisplay(); 
    } else if (currentState == PHASE2_SCREEN) {
        updatePhase2();
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        updatePhase3();
        glutPostRedisplay();
    }
}


void handleMouseClick(int button, int state, int x, int y) {
    // Se estiver no jogo, passar clique para o handler do jogo
    if (currentState == GAME_SCREEN) {
        handleGameMouseClick(button, state, x, y);
        return;
    }
    
    // Se estiver na Fase 2, passar clique para o handler da Fase 2
    if (currentState == PHASE2_SCREEN) {
        handlePhase2MouseClick(button, state, x, y);
        return;
    }

    // Se estiver na Fase 3, passar clique para o handler da Fase 3
    if (currentState == PHASE3_SCREEN) {
        handlePhase3MouseClick(button, state, x, y);
        return;
    }
    
    // Right click: if story overlay is shown, proceed to the pending phase; otherwise
    // right-clicking the phase buttons proceeds immediately to the phase.
    if (currentState == MAIN_MENU && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (showPhaseStory && storyPhase != 0) {
            // advance pagination: two paragraphs per page
            std::vector<std::string> paras = getStoryParagraphs(storyPhase);
            int pages = (paras.size() + 1) / 2;
            storyPage++;
            if (storyPage >= pages) {
                // reached the end -> start the pending phase
                if (storyPhase == 1) {
                    currentState = GAME_SCREEN;
                    initGame();
                    glutPassiveMotionFunc(handleGameMouseMove);
                    glutMotionFunc(handleGameMouseMove);
                } else if (storyPhase == 2) {
                    currentState = PHASE2_SCREEN;
                    initPhase2();
                    glutPassiveMotionFunc(handlePhase2MouseMove);
                    glutMotionFunc(handlePhase2MouseMove);
                } else if (storyPhase == 3) {
                    currentState = PHASE3_SCREEN;
                    initPhase3();
                    glutPassiveMotionFunc(handlePhase3MouseMove);
                    glutMotionFunc(handlePhase3MouseMove);
                }
                // reset overlay state
                showPhaseStory = false;
                storyPhase = 0;
                storyPage = 0;
            }
            glutPostRedisplay();
            return;
        }

        // No overlay active — right-click on buttons to enter immediately
        if (isMouseOverButton(x, y, startButton)) {
            currentState = GAME_SCREEN;
            initGame();
            glutPassiveMotionFunc(handleGameMouseMove);
            glutMotionFunc(handleGameMouseMove);
            glutPostRedisplay();
            return;
        } else if (isMouseOverButton(x, y, phase2Button)) {
            currentState = PHASE2_SCREEN;
            initPhase2();
            glutPassiveMotionFunc(handlePhase2MouseMove);
            glutMotionFunc(handlePhase2MouseMove);
            glutPostRedisplay();
            return;
        } else if (isMouseOverButton(x, y, phase3Button)) {
            currentState = PHASE3_SCREEN;
            initPhase3();
            glutPassiveMotionFunc(handlePhase3MouseMove);
            glutMotionFunc(handlePhase3MouseMove);
            glutPostRedisplay();
            return;
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (currentState) {
            case MAIN_MENU:
                // Left-click shows the story overlay for the clicked phase
                if (isMouseOverButton(x, y, startButton)) {
                    showPhaseStory = true;
                    storyPhase = 1;
                    storyPage = 0;
                } else if (isMouseOverButton(x, y, phase2Button)) {
                    showPhaseStory = true;
                    storyPhase = 2;
                    storyPage = 0;
                } else if (isMouseOverButton(x, y, phase3Button)) {
                    showPhaseStory = true;
                    storyPhase = 3;
                    storyPage = 0;
                } else if (isMouseOverButton(x, y, instructionsButton)) {
                    currentState = INSTRUCTIONS_SCREEN;
                } else if (isMouseOverButton(x, y, exitButton)) {
                    exit(0); // Sair do jogo
                }
                break;
            case INSTRUCTIONS_SCREEN:
                if (isMouseOverButton(x, y, backButton)) {
                    currentState = MAIN_MENU;
                    glutSetCursor(GLUT_CURSOR_INHERIT);
                    glutPassiveMotionFunc(handleMouseHover);
                }
                break;
            case GAME_SCREEN:
            case PHASE2_SCREEN:
            case PHASE3_SCREEN:
                break;
        }
        glutPostRedisplay();
    }
}

void handleMouseHover(int x, int y) {
    // update menu pointer
    menuMouseX = x;
    menuMouseY = y;
    bool needsRedraw = false;
    
    switch (currentState) {
        case MAIN_MENU:
            if (startButton.isHovered != isMouseOverButton(x, y, startButton)) {
                startButton.isHovered = !startButton.isHovered;
                needsRedraw = true;
            }
            if (phase2Button.isHovered != isMouseOverButton(x, y, phase2Button)) {
                phase2Button.isHovered = !phase2Button.isHovered;
                needsRedraw = true;
            }
            if (phase3Button.isHovered != isMouseOverButton(x, y, phase3Button)) {
                phase3Button.isHovered = !phase3Button.isHovered;
                needsRedraw = true;
            }
            if (instructionsButton.isHovered != isMouseOverButton(x, y, instructionsButton)) {
                instructionsButton.isHovered = !instructionsButton.isHovered;
                needsRedraw = true;
            }
            if (exitButton.isHovered != isMouseOverButton(x, y, exitButton)) {
                exitButton.isHovered = !exitButton.isHovered;
                needsRedraw = true;
            }
            break;
        case INSTRUCTIONS_SCREEN:
            if (backButton.isHovered != isMouseOverButton(x, y, backButton)) {
                backButton.isHovered = !backButton.isHovered;
                needsRedraw = true;
            }
            break;
        case GAME_SCREEN:
        case PHASE2_SCREEN:
        case PHASE3_SCREEN:
            break;
    }

    if (needsRedraw) {
        glutPostRedisplay();
    }
}



void handleKeyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    if (currentState == GAME_SCREEN) {
        if (key == 27) { 
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            // Se está em Game Over, usar handleGameOverKeyboard
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handleGameKeyboard(key);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            // Se está em Game Over, usar handleGameOverKeyboard
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase2Keyboard(key);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase3Keyboard(key);
            }
        }
        glutPostRedisplay();
    }
}

void handleSpecialKey(int key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        handleGameSpecialKey(key, x, y);
        glutPostRedisplay();
    }
    // Adicionar para outras fases conforme necessário
}

void setup() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    // Menu usa cursor padrão do sistema
    glutSetCursor(GLUT_CURSOR_INHERIT);
}

void changeState(int newState) {
    currentState = (GameState)newState;
    
    // Se voltando ao menu, restaurar cursor e callbacks
    if (newState == MAIN_MENU) {
        glutSetCursor(GLUT_CURSOR_INHERIT);
        glutPassiveMotionFunc(handleMouseHover);
    }
    
    glutPostRedisplay();
}
