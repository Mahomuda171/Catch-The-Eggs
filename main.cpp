
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <math.h>
using namespace std;

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

// Egg structure
struct Egg {
    float x, y;
    float speed;
    int type;
    bool active;
    int chickenId;
};

const int MAX_EGGS = 30;
Egg eggs[MAX_EGGS];


// Function declarations
void display();
void initOpenGL();
void drawRect(float x, float y, float width, float height, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void initEggs();
void spawnEgg();
void keyboard(unsigned char key, int x, int y);
//void specialKeys(int key, int x, int y);


// Draw rectangle
void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Draw circle
void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    for(int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x + radius * cos(rad), y + radius * sin(rad));
    }
    glEnd();
}


// Initialize all eggs
void initEggs() {
    for(int i = 0; i < MAX_EGGS; i++) {
        eggs[i].active = false;
        eggs[i].chickenId = -1;
    }
}

// Spawn a new egg
void spawnEgg() {
    for(int i = 0; i < MAX_EGGS; i++) {
        if(!eggs[i].active) {
            int chickenChoice = rand() % 3;
            eggs[i].x = chickenX[chickenChoice] + 15;
            eggs[i].y = chickenY - 20;
            eggs[i].speed = 2.0f + (rand() % 5) / 2.0f;

            int r = rand() % 10;
            if(r <= 6) eggs[i].type = 0;
            else if(r == 7) eggs[i].type = 1;
            else if(r == 8) eggs[i].type = 2;
            else eggs[i].type = 3;

            eggs[i].active = true;
            eggs[i].chickenId = chickenChoice;
            break;
        }
    }
}

// Keyboard handler
void keyboard(unsigned char key, int x, int y) {
    if(currentState == PLAYING) {
        if(key == 'p' || key == 'P') {
            currentState = PAUSED;
        }
        else if(key == 27) {  // ESC
            currentState = MENU;
        }
        else if(key == 'a' || key == 'A') {
            basketX -= 30;
        }
        else if(key == 'd' || key == 'D') {
            basketX += 30;
        }
    }
    else if(currentState == PAUSED) {
        if(key == 'p' || key == 'P') {
            currentState = PLAYING;
        }
    }
    
    if(basketX < 0) basketX = 0;
    if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
    glutPostRedisplay();
}
 
// Update display
void display() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawStick();
    drawChicken();
    drawBasket();

    // Draw all active eggs
    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            drawEgg(eggs[i].x, eggs[i].y, eggs[i].type);
        }
    }

    glutSwapBuffers();
}


// Initialize OpenGL
void initOpenGL() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Catch The Eggs");

    initOpenGL();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
