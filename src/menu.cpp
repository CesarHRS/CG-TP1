#include <GL/glut.h>
#include "menu.h"
#include "game.h"
#include "phase2.h"
#include "phase3.h"
#include "phase4.h"
#include "phase5.h"
#include "phase6.h"
#include "phase7.h"
#include "gameover.h"
#include "audio.h"
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
int menuMouseX = 400;
int menuMouseY = 300;

// Story overlay state
bool showPhaseStory = false;
int storyPhase = 0; 
int storyPage = 0; 

// ===================================================================
// BOTÕES DO MENU (Espaçamento corrigido)
// ===================================================================
// Altura total disponivel: 600.
// Começando em 500 e descendo de 60 em 60 pixels para distribuição uniforme.
Button startButton        = {300, 500, 200, 50, "Fase 1", false};
Button phase2Button       = {300, 440, 200, 50, "Fase 2", false};
Button phase3Button       = {300, 380, 200, 50, "Fase 3", false};
Button phase4Button       = {300, 320, 200, 50, "Fase 4", false};
Button phase5Button       = {300, 260, 200, 50, "Fase 5 (3D)", false};
Button phase6Button       = {300, 200, 200, 50, "Fase 6 (3D)", false};
Button phase7Button       = {300, 140, 200, 50, "Fase 7 (FPS 3D)", false};
Button instructionsButton = {300, 80,  200, 50, "Como Jogar", false};
Button exitButton         = {300, 20,  200, 50, "Sair", false};

// Botão de voltar (usado na tela de instruções)
Button backButton         = {300, 20,  200, 50, "Voltar para o Menu", false};

void handleKeyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (currentState == GAME_SCREEN) {
        handleGameKeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        handlePhase2KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE3_SCREEN) {
        handlePhase3KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE4_SCREEN) {
        handlePhase4KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE5_SCREEN) {
        handlePhase5KeyboardUp(key, x, y);
    } else if (currentState == PHASE6_SCREEN) {
        handlePhase6KeyboardUp(key, x, y);
        glutPostRedisplay();
    } else if (currentState == PHASE7_SCREEN) {
        handlePhase7KeyboardUp(key, x, y);
        glutPostRedisplay();
    }
}

void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawButton(Button &button) {
    float radius = button.height / 2.0f;
    int segments = 20;
    if (button.isHovered) {
        glColor3f(0.7f, 0.7f, 0.9f);
    } else {
        glColor3f(0.5f, 0.5f, 0.8f);
    }
    
    // Corpo central
    glBegin(GL_POLYGON);
    glVertex2f(button.x + radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y);
    glVertex2f(button.x + button.width - radius, button.y + button.height);
    glVertex2f(button.x + radius, button.y + button.height);
    glEnd();

    // Borda esquerda arredondada
    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = M_PI; 
        theta += (M_PI * i) / segments;
        glVertex2f(button.x + radius + radius * cos(theta), button.y + radius + radius * sin(theta));
    }
    glEnd();

    // Borda direita arredondada
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
    drawText(320, 560, "Jogo de navinha mais legal da sua vida");
    drawButton(startButton);
    drawButton(phase2Button);
    drawButton(phase3Button);
    drawButton(phase4Button);
    drawButton(phase5Button);
    drawButton(phase6Button);
    drawButton(phase7Button);
    drawButton(instructionsButton);
    drawButton(exitButton);
}

static std::vector<std::string> getStoryParagraphs(int phase) {
    std::vector<std::string> p;
    if (phase == 1) {
        p.push_back("FASE 1");
        p.push_back("[ AMEACA DE IMPACTO ]");
        p.push_back("CONTROLE DE MISSAO: Controle para Comandante. Solicita-se confirmacao de escuta.");
        p.push_back("COMANDANTE: Confirmo a escuta.");
        p.push_back("CONTROLE DE MISSAO: Nossos sistemas de varredura profunda detectaram multiplos objetos de alta massa.");
        p.push_back("CONTROLE DE MISSAO: Diretriz: Decolagem imediata. Interceptar e neutralizar todas as ameacas.");
    } else if (phase == 2) {
        p.push_back("FASE 2");
        p.push_back("[ ANOMALIA DE SENSOR ]");
        p.push_back("CONTROLE DE MISSAO: Detectamos uma anomalia. Dados de telemetria corrompidos.");
        p.push_back("CONTROLE DE MISSAO: Desative o bloqueio automatico. Acione o sistema de mira manual.");
    } else if (phase == 3) {
        p.push_back("FASE 3");
        p.push_back("[ MUDANCA DE DIRETRIZ ]");
        p.push_back("CONTROLE DE MISSAO: O radar foi restabelecido. Detectamos um contato unico, nao identificado.");
        p.push_back("CONTROLE DE MISSAO: Nova diretriz: Reciclar municao e interceptar o contato hostil.");
    } else if (phase == 4) {
        p.push_back("FASE 4");
        p.push_back("[ CONFRONTO FINAL ]");
        p.push_back("CONTROLE DE MISSAO: Nave de guerra de origem desconhecida detectada.");
        p.push_back("CONTROLE DE MISSAO: Engajar o alvo hostil. Neutralizar a ameaca.");
    } else if (phase == 5) {
        p.push_back("FASE 5");
        p.push_back("[ O PLANETA DOS SOLIDOS GEOMETRICOS ]");
        p.push_back("CONTROLE DE MISSAO: Voce pousou em um planeta estranho. Colete os solidos geometricos solicitados.");
        p.push_back("CONTROLE DE MISSAO: Use W/A/S/D para mover e o Mouse para olhar.");
    } else if (phase == 6) {
        p.push_back("FASE 6");
        p.push_back("[ CINTURAO DE ASTEROIDES MAGNETICOS ]");
        p.push_back("CONTROLE DE MISSAO: Alerta. Campos magneticos ativos. Calcule a area correta (PI * r^2) para passar.");
        p.push_back("CONTROLE DE MISSAO: Atravesse apenas o portal seguro.");
    } else if (phase == 7) {
        p.push_back("FASE 7");
        p.push_back("[ ALERTA DE INTRUSAO ]");
        p.push_back("CONTROLE DE MISSAO: Comandante, apos desviar com sucesso dos campos magneticos, detectamos um estrondo no casco.");
        p.push_back("CONTROLE DE MISSAO: ALERTA VERMELHO! Os inimigos romperam a escotilha e adentraram sua nave. E um ataque desesperado final.");
        p.push_back("CONTROLE DE MISSAO: Eles estao armados e se espalharam pelos compartimentos. Voce precisa elimina-los antes que eles tomem o controle.");
        p.push_back("COMANDANTE: Entendido. Ninguem toma minha nave. Iniciando limpeza.");
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
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.06f); glVertex2f(0, 0);
    glColor3f(0.0f, 0.03f, 0.12f); glVertex2f(windowWidth, 0);
    glColor3f(0.0f, 0.01f, 0.04f); glVertex2f(windowWidth, windowHeight);
    glColor3f(0.01f, 0.01f, 0.03f); glVertex2f(0, windowHeight);
    glEnd();

    // Panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
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

    std::vector<std::string> paras = getStoryParagraphs(storyPhase);
    int pages = (paras.size() + 1) / 2;
    if (pages == 0) { showPhaseStory = false; return; }
    if (storyPage >= pages) storyPage = pages - 1;

    int startIdx = storyPage * 2;
    int maxChars = 70;

    std::vector<std::string> renderedLines;
    for (int i = 0; i < 2; ++i) {
        int idx = startIdx + i;
        if (idx >= (int)paras.size()) break;
        std::vector<std::string> wrapped = wrapText(paras[idx], maxChars);
        if (!renderedLines.empty()) renderedLines.push_back("");
        for (auto &ln : wrapped) renderedLines.push_back(ln);
    }

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

    std::string hint = (storyPage < pages - 1) ? "(Clique para mais...)" : "(Clique para iniciar)";
    float hintW = hint.length() * 9.0f;
    glColor3f(0.8f, 0.8f, 0.8f);
    glRasterPos2f(windowWidth / 2.0f - hintW / 2.0f, pad + 20.0f);
    for (char c : hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

void showStoryForPhase(int phase) {
    showPhaseStory = true;
    storyPhase = phase;
    storyPage = 0;
    currentState = MAIN_MENU; 
}

void drawInstructionsScreen() {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(330, 550, "COMO JOGAR");
    
    // Controles Gerais
    drawText(100, 500, "GERAL:");
    drawText(120, 480, "- ESC: Pausar / Voltar ao Menu");
    drawText(120, 460, "- Navegacao: Mouse");

    // Fases Iniciais
    drawText(100, 420, "FASE 1-3 (Nave 2D):");
    drawText(120, 400, "- A/D ou Setas: Mover Nave");
    drawText(120, 380, "- Mouse: Mirar e Clicar / Digitar resposta");
    
    // Fases de Exploração 3D
    drawText(100, 340, "FASE 5-6 (Exploracao 3D):");
    drawText(120, 320, "- W/A/S/D: Andar | Mouse: Olhar");
    drawText(120, 300, "- E: Interagir/Pegar Objeto");

    // Fase 7 (Destaque)
    glColor3f(0.2f, 1.0f, 0.2f); // Verde destaque
    drawText(100, 250, "FASE 7 (NOVO - Combate FPS):");
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(120, 230, "- W/A/S/D: Movimentacao Tatica (Frente/Lado)");
    drawText(120, 210, "- MOUSE: Mirar (360 graus)");
    drawText(120, 190, "- CLIQUE ESQ ou ESPACO: Disparar Laser");
    drawText(120, 170, "- Objetivo: Eliminar todos os 5 invasores da nave.");
    drawText(120, 150, "- Cuidado: Inimigos patrulham e atiram de volta.");
    
    drawButton(backButton);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpar Depth Buffer também

    switch (currentState) {
        case MAIN_MENU:
            if (showPhaseStory) drawStoryOverlay();
            else drawMainMenu();
            break;
        case INSTRUCTIONS_SCREEN:
            drawInstructionsScreen();
            break;
        case GAME_SCREEN: drawGame(); break;
        case PHASE2_SCREEN: drawPhase2(); break;
        case PHASE3_SCREEN: drawPhase3(); break;
        case PHASE4_SCREEN: drawPhase4(); break;
        case PHASE5_SCREEN: drawPhase5(windowWidth, windowHeight); break;
        case PHASE6_SCREEN: drawPhase6(windowWidth, windowHeight); break;
        case PHASE7_SCREEN: drawPhase7(); break;
    }
    glutSwapBuffers();
}

void updateScene() {
    if (currentState == GAME_SCREEN) { updateGame(); glutPostRedisplay(); }
    else if (currentState == PHASE2_SCREEN) { updatePhase2(); glutPostRedisplay(); }
    else if (currentState == PHASE3_SCREEN) { updatePhase3(); glutPostRedisplay(); }
    else if (currentState == PHASE4_SCREEN) { updatePhase4(); glutPostRedisplay(); }
    // Fases 3D usam seus proprios timers iniciados no init
}

void handleMouseClick(int button, int state, int x, int y) {
    if (currentState == GAME_SCREEN) { handleGameMouseClick(button, state, x, y); return; }
    if (currentState == PHASE2_SCREEN) { handlePhase2MouseClick(button, state, x, y); return; }
    if (currentState == PHASE3_SCREEN) { handlePhase3MouseClick(button, state, x, y); return; }
    if (currentState == PHASE4_SCREEN) { handlePhase4MouseClick(button, state, x, y); return; }
    if (currentState == PHASE5_SCREEN) { handlePhase5Mouse(button, state, x, y); return; }
    if (currentState == PHASE7_SCREEN) { handlePhase7MouseClick(button, state, x, y); return; }

    if (currentState == MAIN_MENU && (button == GLUT_LEFT_BUTTON) && state == GLUT_DOWN) {
        if (showPhaseStory && storyPhase != 0) {
            std::vector<std::string> paras = getStoryParagraphs(storyPhase);
            int pages = (paras.size() + 1) / 2;
            storyPage++;
            if (storyPage >= pages) {
                // Iniciar Fase
                if (storyPhase == 1) { currentState = GAME_SCREEN; initGame(); }
                else if (storyPhase == 2) { currentState = PHASE2_SCREEN; initPhase2(); }
                else if (storyPhase == 3) { currentState = PHASE3_SCREEN; initPhase3(); }
                else if (storyPhase == 4) { currentState = PHASE4_SCREEN; initPhase4(); }
                else if (storyPhase == 5) { currentState = PHASE5_SCREEN; initPhase5(); glutTimerFunc(16, updatePhase5, 0); }
                else if (storyPhase == 6) { currentState = PHASE6_SCREEN; initPhase6(); glutTimerFunc(16, updatePhase6, 0); }
                else if (storyPhase == 7) { currentState = PHASE7_SCREEN; initPhase7(); glutTimerFunc(16, updatePhase7, 0); }
                showPhaseStory = false; storyPhase = 0; storyPage = 0;
            }
            glutPostRedisplay();
            return;
        }

        if (isMouseOverButton(x, y, startButton)) showStoryForPhase(1);
        else if (isMouseOverButton(x, y, phase2Button)) showStoryForPhase(2);
        else if (isMouseOverButton(x, y, phase3Button)) showStoryForPhase(3);
        else if (isMouseOverButton(x, y, phase4Button)) showStoryForPhase(4);
        else if (isMouseOverButton(x, y, phase5Button)) showStoryForPhase(5);
        else if (isMouseOverButton(x, y, phase6Button)) showStoryForPhase(6);
        else if (isMouseOverButton(x, y, phase7Button)) showStoryForPhase(7);
        else if (isMouseOverButton(x, y, instructionsButton)) currentState = INSTRUCTIONS_SCREEN;
        else if (isMouseOverButton(x, y, exitButton)) exit(0);
    }
    else if (currentState == INSTRUCTIONS_SCREEN && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (isMouseOverButton(x, y, backButton)) currentState = MAIN_MENU;
    }
}

void handleMouseHover(int x, int y) {
    menuMouseX = x; menuMouseY = y;
    bool redraw = false;
    if (currentState == MAIN_MENU) {
        if (startButton.isHovered != isMouseOverButton(x, y, startButton)) { startButton.isHovered = !startButton.isHovered; redraw = true; }
        if (phase2Button.isHovered != isMouseOverButton(x, y, phase2Button)) { phase2Button.isHovered = !phase2Button.isHovered; redraw = true; }
        if (phase3Button.isHovered != isMouseOverButton(x, y, phase3Button)) { phase3Button.isHovered = !phase3Button.isHovered; redraw = true; }
        if (phase4Button.isHovered != isMouseOverButton(x, y, phase4Button)) { phase4Button.isHovered = !phase4Button.isHovered; redraw = true; }
        if (phase5Button.isHovered != isMouseOverButton(x, y, phase5Button)) { phase5Button.isHovered = !phase5Button.isHovered; redraw = true; }
        if (phase6Button.isHovered != isMouseOverButton(x, y, phase6Button)) { phase6Button.isHovered = !phase6Button.isHovered; redraw = true; }
        if (phase7Button.isHovered != isMouseOverButton(x, y, phase7Button)) { phase7Button.isHovered = !phase7Button.isHovered; redraw = true; }
        if (instructionsButton.isHovered != isMouseOverButton(x, y, instructionsButton)) { instructionsButton.isHovered = !instructionsButton.isHovered; redraw = true; }
        if (exitButton.isHovered != isMouseOverButton(x, y, exitButton)) { exitButton.isHovered = !exitButton.isHovered; redraw = true; }
    } else if (currentState == INSTRUCTIONS_SCREEN) {
        if (backButton.isHovered != isMouseOverButton(x, y, backButton)) { backButton.isHovered = !backButton.isHovered; redraw = true; }
    }
    if (redraw) glutPostRedisplay();
}

void handleKeyboard(unsigned char key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        if (key == 27) currentState = MAIN_MENU;
        else if (getGameOver()) handleGameOverKeyboard(key);
        else handleGameKeyboard(key);
    } 
    else if (currentState == PHASE7_SCREEN) {
        // Verifica variaveis exportadas de phase7.h
        if (phase7_gameOver || phase7_won) {
            if (key == 27 || key == ' ') {
                changeState(MAIN_MENU);
                glutSetCursor(GLUT_CURSOR_INHERIT);
            }
        } else {
            handlePhase7Keyboard(key, x, y);
        }
    }
    else if (currentState == PHASE2_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == PHASE3_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == PHASE4_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == PHASE5_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == PHASE6_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == INSTRUCTIONS_SCREEN && key == 27) currentState = MAIN_MENU;
    else if (currentState == MAIN_MENU && key == 27) exit(0);
    
    glutPostRedisplay();
}

void handleSpecialKey(int key, int x, int y) {
    if (currentState == GAME_SCREEN) handleGameSpecialKey(key, x, y);
    else if (currentState == PHASE2_SCREEN) handlePhase2SpecialKey(key, x, y);
    else if (currentState == PHASE4_SCREEN) handlePhase4SpecialKey(key, x, y);
    else if (currentState == PHASE6_SCREEN) handlePhase6Special(key, x, y);
    else if (currentState == PHASE7_SCREEN) handlePhase7SpecialKey(key, x, y);
    glutPostRedisplay();
}

void handleSpecialKeyUp(int key, int x, int y) {
    if (currentState == PHASE4_SCREEN) handlePhase4SpecialKeyUp(key, x, y);
    else if (currentState == PHASE6_SCREEN) handlePhase6SpecialUp(key, x, y);
    else if (currentState == PHASE7_SCREEN) handlePhase7SpecialKeyUp(key, x, y);
    glutPostRedisplay();
}

void setup() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glutSetCursor(GLUT_CURSOR_INHERIT);
    Audio::getInstance().init();
    Audio::getInstance().loadAll();
    atexit(audio_cleanup_at_exit);
}

void setCurrentPhase(int phase) {
    if (phase == 0) {
        currentState = MAIN_MENU;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        glutPassiveMotionFunc(handleMouseHover);
        glutPostRedisplay();
    }
}

void changeState(int newState) {
    currentState = (GameState)newState;
    if (newState == MAIN_MENU) {
        glutSetCursor(GLUT_CURSOR_INHERIT);
        glutPassiveMotionFunc(handleMouseHover);
    }
    glutPostRedisplay();
}
