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
    // Request a depth buffer so 3D GLUT solids render correctly
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 700);
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
    glutSpecialUpFunc(handleSpecialKeyUp); // Teclas especiais up
    glutIdleFunc(updateScene); 

    glutMainLoop(); 

    return 0;
}
