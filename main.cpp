
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

//////////Marwa////////////

// Global variables
float basketX = WIDTH / 2 - 50;
const int BASKET_WIDTH = 100;
const int BASKET_HEIGHT = 25;

// Chicken positions (3 chickens)
float chickenX[3] = {200, WIDTH/2, 600};
float chickenY = HEIGHT - 65;

// Function declarations
void drawEgg(float x, float y, int type);
void drawBasket();
void drawStick();
void drawSingleChicken(float x, float y, int chickenNum);
void drawChicken();

// Draw egg based on type
void drawEgg(float x, float y, int type) {
    if(type == 0) drawCircle(x, y, 12, 1.0f, 0.85f, 0.65f);  // Normal
    else if(type == 1) drawCircle(x, y, 12, 1.0f, 0.84f, 0.0f);  // Golden
    else if(type == 2) drawCircle(x, y, 12, 0.3f, 0.6f, 1.0f);   // Blue
    else drawCircle(x, y, 12, 0.4f, 0.2f, 0.1f);  // Poop
}

// Draw basket
void drawBasket() {
    drawRect(basketX, 30, BASKET_WIDTH, BASKET_HEIGHT, 0.65f, 0.41f, 0.16f);
    
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(basketX, 30);
    glVertex2f(basketX + BASKET_WIDTH, 30);
    glVertex2f(basketX + BASKET_WIDTH, 55);
    glVertex2f(basketX, 55);
    glEnd();
}

// Draw stick
void drawStick() {
    drawRect(0, HEIGHT - 80, WIDTH, 10, 0.5f, 0.35f, 0.15f);
    for(int i = 0; i < 3; i++) {
        drawRect(chickenX[i] + 10, HEIGHT - 90, 8, 15, 0.4f, 0.3f, 0.1f);
    }
}

// Draw single chicken
void drawSingleChicken(float x, float y, int chickenNum) {
    drawCircle(x, y, 22, 1.0f, 1.0f, 0.0f);
    drawCircle(x + 30, y + 10, 13, 1.0f, 0.2f, 0.1f);
    drawCircle(x + 36, y + 15, 3, 0.0f, 0.0f, 0.0f);
}

// Draw all chickens
void drawChicken() {
    for(int i = 0; i < 3; i++) {
        drawSingleChicken(chickenX[i], chickenY, i);
    }
}

// Updated display function
void display() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawStick();
    drawChicken();
    drawBasket();
    
    // Test eggs
    drawEgg(200, 400, 0);
    drawEgg(400, 350, 1);
    drawEgg(600, 300, 2);
    
    glutSwapBuffers();
}
