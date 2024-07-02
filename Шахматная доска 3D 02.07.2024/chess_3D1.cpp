#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/glut.h"

typedef struct
{
    GLint       x;
    GLint       y;
} GLintPoint;

float angleX = 0.0f;
float angleY = 0.0f;
int lastMouseX, lastMouseY;

void chessboard ()
{
    //GLintPoint          T = {199, 99};

    /*for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {

          if ((i + j) & 1)
          {
              glColor3f (0.820f, 0.508f, 0.200f);
          }
          else
          {
              glColor3f (0.68f, 0.384f, 0.392f);
          }

          glRecti (T.x, T.y, T.x + 50, T.y + 50);
          T.x += 50;



        }

        T.x = 199;
        T.y += 50;
    }*/
    glPushMatrix();

    // Apply rotation



    glRotatef(26.0, 1.0, 0.0, 0.0);
    glRotatef(38.0, 0.0, 1.0, 0.0);  // Rotate 45 degrees around the Y axis

    glColor3f (0.68f, 0.384f, 0.392f);
    glutWireCube(1.0f);
    glPopMatrix();
}

void myDisplay ()
{
    glClear (GL_COLOR_BUFFER_BIT);
    glTranslatef(0.0f,0.0f,-5.0f);
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    chessboard();
    glFlush ();
}

// Функция для обработки событий перемещения мыши
void mouseMotion(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    angleX += deltaY * 0.5f;
    angleY += deltaX * 0.5f;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

// Функция для обработки событий нажатия кнопок мыши
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
}

void myInit ()
{
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glColor3f (0.0f, 0.0f, 0.0f);
    glPointSize (2.0);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (-1.0, 1.0, -1.0, 1.0, 1.0,100000.0);
}

int main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (200, 150);
    glutCreateWindow ("chess");
    glutDisplayFunc (myDisplay);
    myInit();
    glutMainLoop();
    return 0;
}
