#include <GL/glut.h>
#include "menu.h"
#include "game.h"
#include "phase2.h"
#include "phase3.h"
#include "phase4.h"
#include "phase5.h"
#include "phase6.h"
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
// menu uses system cursor; custom pointer is drawn only in phase3
int menuMouseX = 400;
int menuMouseY = 300;

// Story overlay state: when true, a space-background overlay with text is shown
bool showPhaseStory = false;
int storyPhase = 0; // 1,2,3
int storyPage = 0; // pagination index (0-based, two paragraphs per page)


// Arrange menu buttons with uniform vertical spacing
// Start near the top and step down by 60 px between buttons
// Arrange 9 main buttons symmetrically (7 phases + instructions + exit)
Button startButton = {300, 460, 200, 50, "Fase 1", false};
Button phase2Button = {300, 400, 200, 50, "Fase 2", false};
Button phase3Button = {300, 340, 200, 50, "Fase 3", false};
Button phase4Button = {300, 280, 200, 50, "Fase 4", false};
Button phase5Button = {300, 220, 200, 50, "Fase 5 (3D)", false};
Button phase6Button = {300, 160, 200, 50, "Fase 6 (3D)", false};
// instructions and exit below
Button instructionsButton = {300, 100, 200, 50, "Como Jogar", false};
Button exitButton = {300, 40, 200, 50, "Sair", false};
Button backButton = {300, 10, 200, 50, "Voltar para o Menu", false};

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
    } else if (currentState == PHASE4_SCREEN) {
        handlePhase4KeyboardUp(key);
        glutPostRedisplay();
    } else if (currentState == PHASE5_SCREEN) {
        handlePhase5KeyboardUp(key, x, y);
    } else if (currentState == PHASE6_SCREEN) {
        handlePhase6KeyboardUp(key, x, y);
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
    drawText(320, 540, "Jogo de navinha mais legal da sua vida");
    drawButton(startButton);
    drawButton(phase2Button);
    drawButton(phase3Button);
    drawButton(phase4Button);
    drawButton(phase5Button);
    drawButton(phase6Button);
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
        p.push_back("CONTROLE de MISSAO: Analise de telemetria confirma: Assinatura de asteroides em trajetoria de colisao direta. Janela de interceptacao estimada: T-menos 15 minutos.");
        p.push_back("CONTROLE DE MISSAO: Diretriz: Decolagem imediata da nave Papa Alpha Romeo Gamma. Objetivo primario: Interceptar e neutralizar todas as ameacas antes da reentrada atmosferica.");
        p.push_back("COMANDANTE: Confirmado, interceptar e neutralizar todas as ameacas antes da reentrada atmosferica.");
    } else if (phase == 2) {
        p.push_back("FASE 2");
        p.push_back("[ ANOMALIA DE SENSOR ]");
        p.push_back("CONTROLE DE MISSAO: Papa Alpha Romeo Gamma, mantenha posicao.");
        p.push_back("CONTROLE DE MISSAO: Detectamos uma anomalia. O fluxo de asteroides persiste, porem os dados de telemetria dos alvos estao corrompidos. Suspeita de interferencia externa.");
        p.push_back("CONTROLE DE MISSAO: O sistema de aquisicao automatica de alvos esta inoperante.");
        p.push_back("CONTROLE DE MISSAO: Diretriz: Desative o bloqueio automatico. Acione o sistema de mira manual. Utilize o visor de aquisicao direta para calcular os vetores de interceptacao. Prossiga com a missao.");
        p.push_back("COMANDANTE: Confirmado, desabilitando sistema automatico, e engajando modo manual.");
    } else if (phase == 3) {
        p.push_back("FASE 3");
        p.push_back("[ MUDANCA DE DIRETRIZ ]");
        p.push_back("CONTROLE DE MISSAO: Papa Alpha Romeo Gamma, recebemos seu pedido de retorno a base para reabastecimento. Aguarde autorizacao.");
        p.push_back("CONTROLE DE MISSAO: ...Espere. Alerta. O radar foi restabelecido. A fonte da interferencia foi localizada.");
        p.push_back("CONTROLE DE MISSAO: Detectamos um contato unico, nao identificado, em alta velocidade. Assinatura energetica desconhecida. O contato viola o perimetro de defesa orbital.");
        p.push_back("CONTROLE DE MISSAO: Analise confirma: Este alvo e a fonte da anomalia e o ponto de origem dos asteroides.");
        p.push_back("CONTROLE DE MISSAO: Pedido de retorno a base negado. Nova diretriz: Reciclar a municao previamente gasta, e interceptar e identificar o contato hostil. Prioridade maxima.");
    } else if (phase == 4) {
        p.push_back("FASE 4");
        p.push_back("[ CONFRONTO FINAL ]");
        p.push_back("CONTROLE DE MISSAO: Papa Alpha Romeo Gamma, confirmamos identificacao visual do contato hostil.");
        p.push_back("CONTROLE DE MISSAO: Classificacao: Nave de guerra de origem desconhecida. Dimensoes compativeis com classe de cruzador pesado. Armamento ativo detectado.");
        p.push_back("CONTROLE DE MISSAO: O contato esta em formacao de combate e disparando projeteis em sua direcao. Analise tatica: Este e o comandante da frota invasora.");
        p.push_back("CONTROLE DE MISSAO: Diretriz final: Engajar o alvo hostil. Neutralizar a ameaca com extremo prejuizo. Esta e a unica chance de defender o perimetro.");
        p.push_back("CONTROLE DE MISSAO: Boa sorte, Comandante. A humanidade conta com voce.");
        p.push_back("COMANDANTE: Confirmado. Iniciando engajamento hostil.");
    }else if (phase == 5) {
        p.push_back("FASE 5");
        p.push_back("[ ASAS QUEBRADAS ]");
        p.push_back("APOS O CONFRONTO FINAL, A NAVE DO COMANDANTE SOFREU DANOS.");
        p.push_back("CONTROLE DE MISSAO: Controle para Comandante. Solicita-se confirmacao de escuta");
        p.push_back("COMANDANTE: Confirmo a escuta.");
        p.push_back("CONTROLE DE MISSAO: Sistemas de varredura indicam danos estruturais severos nas asas e na iluminação.");
        p.push_back("CONTROLE DE MISSAO: Iniciar sequencia de reparos o mais rapido possivel.");
        p.push_back("COMANDANTE: Confirmado, iniciando sequencia de reparos.");
    } else if (phase == 5) {
        p.push_back("FASE 5");
        p.push_back("[ O PLANETA DOS SOLIDOS GEOMETRICOS ]");
        p.push_back("CONTROLE DE MISSAO: Comandante, entrando em modo de exploracao tridimensional.");
        p.push_back("COMANDANTE: Confirmado. Ativando sistemas de navegacao livre e sensores de curto alcance.");
        p.push_back("CONTROLE DE MISSAO: Voce pousou em um planeta com solidos geometricos espalhados.");
        p.push_back("COMANDANTE: Sua missao e identificar e coletar os objetos geometricos especificos solicitados.");
        p.push_back("CONTROLE DE MISSAO: Use W/A/S/D para movimentacao e mouse para visao. Cuidado com objetos errados!");
        p.push_back("COMANDANTE: Entendido. Iniciando varredura do planeta.");
    } else if (phase == 6) {
        p.push_back("FASE 6");
        p.push_back("[ CINTURAO DE ASTEROIDES MAGNETICOS ]");
        p.push_back("CONTROLE DE MISSAO: Comandante, decolagem bem-sucedida. Proximo destino: retorno a base.");
        p.push_back("COMANDANTE: Confirmado. Ajustando rota para Terra.");
        p.push_back("CONTROLE DE MISSAO: Alerta. Sensores detectam campo de forca no setor. Multiplos campos magneticos ativos.");
        p.push_back("CONTROLE DE MISSAO: Os campos geram portoes de energia com diferentes intensidades. Atravessar o campo errado causara dano a nave.");
        p.push_back("CONTROLE DE MISSAO: Calculo de areas dos campos magneticos sera necessario para identificar o portao seguro.");
        p.push_back("CONTROLE DE MISSAO: Use a formula: Area = PI * raio ao quadrado. Aproxime PI = 3 para calculos rapidos.");
        p.push_back("COMANDANTE: Entendido. Iniciando navegacao pelo cinturao magnetico.");
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
    if (pages == 0) {
        // No story available for this phase: close overlay to avoid invalid indices
        showPhaseStory = false;
        storyPhase = 0;
        storyPage = 0;
        return;
    }
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
    if (storyPage < pages - 1) hint = "(Clique para mostrar mais)";
    else hint = "(Clique para iniciar a fase)";
    float hintW = hint.length() * 9.0f;
    glColor3f(0.8f, 0.8f, 0.8f);
    glRasterPos2f(windowWidth / 2.0f - hintW / 2.0f, pad + 20.0f);
    for (char c : hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

void showStoryForPhase(int phase) {
    showPhaseStory = true;
    storyPhase = phase;
    storyPage = 0;
    currentState = MAIN_MENU; // Voltar para o menu para exibir a história
}

void drawInstructionsScreen() {
    drawText(330, 500, "Como Jogar");
    
    // Controles gerais
    drawText(150, 460, "CONTROLES GERAIS:");
    drawText(150, 440, "- Pressione 'A' e 'D' (ou setas) para mover a nave horizontalmente.");
    drawText(150, 420, "- Na Fase 4, use 'W' e 'S' para mover verticalmente tambem.");
    drawText(150, 400, "- Pressione 'ESC' durante o jogo para voltar ao menu.");
    
    // Fase 1
    drawText(150, 370, "FASE 1 - Destrua os Asteroides:");
    drawText(150, 350, "- Clique nos asteroides com o botao esquerdo do mouse.");
    drawText(150, 330, "- Digite o resultado da conta matematica que aparecer no asteroide.");
    drawText(150, 310, "- Destrua todos os asteroides antes que atinjam sua nave.");
    
    // Fase 2
    drawText(150, 280, "FASE 2 - Mira Manual:");
    drawText(150, 260, "- Mova o mouse para mirar nos asteroides.");
    drawText(150, 240, "- Clique para disparar e acertar o asteroide correto (10 acertos).");
    
    // Fase 3
    drawText(150, 210, "FASE 3 - Reciclagem de Municao:");
    drawText(150, 190, "- Destrua asteroides para coletar municao (triangulos vermelhos).");
    drawText(150, 170, "- Use espaco para atirar no alvo hostil central.");
    
    // Fase 4
    drawText(150, 140, "FASE 4 - Boss Final:");
    drawText(150, 120, "- Desvie dos projeteis do boss e dos asteroides.");
    drawText(150, 100, "- Clique para disparar lasers no boss (munição limitada).");
    
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
        case PHASE4_SCREEN:
            drawPhase4();
            break;
        case PHASE5_SCREEN:
            drawPhase5(windowWidth, windowHeight);
            break;
        case PHASE6_SCREEN:
            drawPhase6(windowWidth, windowHeight);
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
    } else if (currentState == PHASE4_SCREEN) {
        updatePhase4();
        glutPostRedisplay();
    } else if (currentState == PHASE5_SCREEN) {
        // Fase 5 (3D) usa glutTimerFunc próprio, não precisa de update aqui
    } else if (currentState == PHASE6_SCREEN) {
        // Fase 6 (3D) usa glutTimerFunc próprio, não precisa de update aqui
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
    
    // Se estiver na Fase 4, passar clique para o handler da Fase 4
    if (currentState == PHASE4_SCREEN) {
        handlePhase4MouseClick(button, state, x, y);
        return;
    }
    if (currentState == PHASE5_SCREEN) {
        handlePhase5Mouse(button, state, x, y);
        return;
    }
    
    // Sistema de história e fases - Qualquer clique (esquerdo ou direito)
    if (currentState == MAIN_MENU && (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) && state == GLUT_DOWN) {
        if (showPhaseStory && storyPhase != 0) {
            // Se a história está sendo exibida, avançar para a próxima página
            std::vector<std::string> paras = getStoryParagraphs(storyPhase);
            int pages = (paras.size() + 1) / 2;
            storyPage++;
            if (storyPage >= pages) {
                // Chegou ao fim da história -> iniciar a fase
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
                } else if (storyPhase == 4) {
                    currentState = PHASE4_SCREEN;
                    initPhase4();
                    glutPassiveMotionFunc(handlePhase4MouseMove);
                    glutMotionFunc(handlePhase4MouseMove);
                } else if (storyPhase == 5) {
                    currentState = PHASE5_SCREEN;
                    initPhase5();
                    glutPassiveMotionFunc(handlePhase5PassiveMotion);
                    glutMotionFunc(handlePhase5PassiveMotion);
                    glutTimerFunc(16, updatePhase5, 0);
                } else if (storyPhase == 6) {
                    currentState = PHASE6_SCREEN;
                    initPhase6();
                    glutTimerFunc(16, updatePhase6, 0);
                }
                // Resetar estado da história
                showPhaseStory = false;
                storyPhase = 0;
                storyPage = 0;
            }
            glutPostRedisplay();
            return;
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (currentState) {
            case MAIN_MENU:
                // Clicar nas fases mostra a história
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
                } else if (isMouseOverButton(x, y, phase4Button)) {
                    showPhaseStory = true;
                    storyPhase = 4;
                    storyPage = 0;
                } else if (isMouseOverButton(x, y, phase5Button)) {
                    showPhaseStory = true;
                    storyPhase = 5;
                    storyPage = 0;
                } else if (isMouseOverButton(x, y, phase6Button)) {
                    currentState = PHASE6_SCREEN;
                    initPhase6();
                    glutTimerFunc(16, updatePhase6, 0);
                } else if (isMouseOverButton(x, y, instructionsButton)) {
                    currentState = INSTRUCTIONS_SCREEN;
                } else if (isMouseOverButton(x, y, exitButton)) {      
                    exit(0);
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
            case PHASE4_SCREEN:
            case PHASE5_SCREEN:
            case PHASE6_SCREEN:
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
            if (phase4Button.isHovered != isMouseOverButton(x, y, phase4Button)) {
                phase4Button.isHovered = !phase4Button.isHovered;
                needsRedraw = true;
            }
            if (phase5Button.isHovered != isMouseOverButton(x, y, phase5Button)) {
                phase5Button.isHovered = !phase5Button.isHovered;
            }
            if (phase6Button.isHovered != isMouseOverButton(x, y, phase6Button)) {
                phase6Button.isHovered = !phase6Button.isHovered;
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
        case PHASE4_SCREEN:
        case PHASE5_SCREEN:
        case PHASE6_SCREEN:
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
    } else if (currentState == PHASE4_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase4Keyboard(key);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE5_SCREEN) {
        if (key == 27) {
            currentState = MAIN_MENU;
            // Restaurar cursor normal e callback de hover do menu
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPassiveMotionFunc(handleMouseHover);
        } else {
            if (getGameOver()) {
                handleGameOverKeyboard(key);
            } else {
                handlePhase5Keyboard(key, x, y);
            }
        }
        glutPostRedisplay();
    } else if (currentState == PHASE6_SCREEN) {
        if (getGameOver()) {
            handleGameOverKeyboard(key);
        } else {
            handlePhase6Keyboard(key, x, y);
        }
        glutPostRedisplay();
    }
}

void handleSpecialKey(int key, int x, int y) {
    if (currentState == GAME_SCREEN) {
        handleGameSpecialKey(key, x, y);
        glutPostRedisplay();
    } else if (currentState == PHASE2_SCREEN) {
        handlePhase2SpecialKey(key, x, y);
        glutPostRedisplay();
    } else if (currentState == PHASE4_SCREEN) {
        handlePhase4SpecialKey(key, x, y);
        glutPostRedisplay();
    } else if (currentState == PHASE6_SCREEN) {
        handlePhase6Special(key, x, y);
        glutPostRedisplay();
    }
}

void handleSpecialKeyUp(int key, int x, int y) {
    if (currentState == PHASE4_SCREEN) {
        handlePhase4SpecialKeyUp(key, x, y);
        glutPostRedisplay();
    } else if (currentState == PHASE6_SCREEN) {
        handlePhase6SpecialUp(key, x, y);
        glutPostRedisplay();
    }
    (void)key;
    (void)x;
    (void)y;
}

void setup() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    // Menu usa cursor padrão do sistema
    glutSetCursor(GLUT_CURSOR_INHERIT);

    // Initialize audio system (optional). Will print warnings if SDL or mixers aren't available.
    Audio::getInstance().init();
    Audio::getInstance().loadAll();
    // Ensure cleanup on exit
    atexit(audio_cleanup_at_exit);
}

void setCurrentPhase(int phase) {
    if (phase == 0) {
        currentState = MAIN_MENU;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        glutPassiveMotionFunc(handleMouseHover);
        glutPostRedisplay();
    } else if (phase == 6) {
        showPhaseStory = true;
        storyPhase = 6;
        storyPage = 0;
        glutPostRedisplay();
    }
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
