#include <GL/glut.h>
#include "menu.h"

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Menu do Jogo de navinha mais legal da sua vida");

    setup();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutMouseFunc(handleMouseClick);
    glutPassiveMotionFunc(handleMouseHover); 
    glutKeyboardFunc(handleKeyboard); 
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(handleSpecialKey); // Teclas especiais (setas)
    glutIdleFunc(updateScene); 

    glutMainLoop(); 

    return 0;
}
