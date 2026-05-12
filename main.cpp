#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>
#include <string>
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };
GameState currentState = MENU;

int score = 0;
float timeLeft = 60.0f;
int highScore = 0;

float basketX = WIDTH / 2 - 50;
const int BASKET_WIDTH = 100;
const int BASKET_HEIGHT = 25;

float chickenX[3] = {200, WIDTH/2, 600};
float chickenY = HEIGHT - 65;

struct Egg {
    float x, y;
    float speed;
    int type;
    bool active;
    int chickenId;
};

const int MAX_EGGS = 30;
Egg eggs[MAX_EGGS];

void initOpenGL();
void drawRect(float x, float y, float width, float height, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void drawEgg(float x, float y, int type);
void drawBasket();
void drawStick();
void drawSingleChicken(float x, float y);
void drawChicken();
void initEggs();
void spawnEgg();
void updateGame();
void timer(int value);
void display();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void drawText(float x, float y, string text);
void displayMenu();
void displayPlaying();
void displayPaused();
void displayGameOver();

void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(int i = 0; i <= 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x + radius * cos(rad), y + radius * sin(rad));
    }
    glEnd();
}

void drawEgg(float x, float y, int type) {
    if(type == 0) drawCircle(x, y, 12, 1.0f, 0.85f, 0.65f);
    else if(type == 1) drawCircle(x, y, 12, 1.0f, 0.84f, 0.0f);
    else if(type == 2) drawCircle(x, y, 12, 0.3f, 0.6f, 1.0f);
    else drawCircle(x, y, 12, 0.4f, 0.2f, 0.1f);
}

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

void drawStick() {
    drawRect(0, HEIGHT - 80, WIDTH, 10, 0.5f, 0.35f, 0.15f);
    for(int i = 0; i < 3; i++) {
        drawRect(chickenX[i] + 10, HEIGHT - 90, 8, 15, 0.4f, 0.3f, 0.1f);
    }
}

void drawSingleChicken(float x, float y) {
    drawCircle(x, y, 22, 1.0f, 1.0f, 0.0f);
    drawCircle(x + 30, y + 10, 13, 1.0f, 0.2f, 0.1f);
    drawCircle(x + 36, y + 15, 3, 0.0f, 0.0f, 0.0f);
}

void drawChicken() {
    for(int i = 0; i < 3; i++) {
        drawSingleChicken(chickenX[i], chickenY);
    }
}

void initEggs() {
    for(int i = 0; i < MAX_EGGS; i++) {
        eggs[i].active = false;
        eggs[i].chickenId = -1;
    }
}

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

void updateGame() {
    if(currentState != PLAYING) return;

    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            eggs[i].y -= eggs[i].speed;

            if(eggs[i].y + 15 < 55 && eggs[i].y + 15 > 30 &&
               eggs[i].x + 12 > basketX && eggs[i].x < basketX + BASKET_WIDTH) {

                if(eggs[i].type == 0) score += 1;
                else if(eggs[i].type == 1) score += 10;
                else if(eggs[i].type == 2) score += 5;
                else if(eggs[i].type == 3) score -= 10;

                eggs[i].active = false;
            }
            else if(eggs[i].y < 0) {
                eggs[i].active = false;
            }
        }
    }

    if(rand() % 35 == 0) spawnEgg();
}

void timer(int value) {
    if(currentState == PLAYING) {
        static int lastTime = 0;
        int currentTime = glutGet(GLUT_ELAPSED_TIME);

        if(lastTime == 0) lastTime = currentTime;

        if(currentTime - lastTime >= 1000) {
            timeLeft -= 1;
            lastTime = currentTime;

            if(timeLeft <= 0) {
                currentState = GAMEOVER;
                if(score > highScore) highScore = score;
            }
        }

        updateGame();
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void drawText(float x, float y, string text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for(char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawSmallText(float x, float y, string text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for(char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void displayMenu() {
    drawRect(0, 0, WIDTH, HEIGHT, 0.1f, 0.1f, 0.2f);

    drawText(WIDTH/2 - 70, HEIGHT - 150, "CATCH THE EGGS!");
    drawText(WIDTH/2 - 60, HEIGHT - 220, "1. Start Game");
    drawText(WIDTH/2 - 60, HEIGHT - 260, "2. High Score: " + to_string(highScore));
    drawText(WIDTH/2 - 60, HEIGHT - 300, "3. Exit");

    drawSmallText(WIDTH/2 - 120, HEIGHT - 370, "Controls: Arrow Keys or Mouse to move basket");
    drawSmallText(WIDTH/2 - 100, HEIGHT - 400, "Press P to pause, ESC to exit during game");
    drawSmallText(WIDTH/2 - 130, HEIGHT - 430, "Catch: Golden(10) Blue(5) Normal(1) | Avoid Poop(-10)");
    drawSmallText(WIDTH/2 - 100, HEIGHT - 460, "3 Chickens are laying eggs from different positions!");
}

void displayPlaying() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawStick();
    drawChicken();

    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) drawEgg(eggs[i].x, eggs[i].y, eggs[i].type);
    }

    drawBasket();
    drawText(10, HEIGHT - 30, "Score: " + to_string(score));
    drawText(WIDTH - 150, HEIGHT - 30, "Time: " + to_string((int)timeLeft));
}

void displayPaused() {
    displayPlaying();
    drawRect(WIDTH/2 - 120, HEIGHT/2 - 50, 240, 100, 0.0f, 0.0f, 0.0f);
    drawText(WIDTH/2 - 35, HEIGHT/2 + 20, "PAUSED");
    drawText(WIDTH/2 - 55, HEIGHT/2 - 10, "Press P to resume");
}

void displayGameOver() {
    drawRect(0, 0, WIDTH, HEIGHT, 0.1f, 0.1f, 0.2f);
    drawText(WIDTH/2 - 50, HEIGHT/2 + 50, "GAME OVER!");
    drawText(WIDTH/2 - 40, HEIGHT/2 + 20, "Score: " + to_string(score));
    drawText(WIDTH/2 - 55, HEIGHT/2 - 10, "High Score: " + to_string(highScore));
    drawText(WIDTH/2 - 60, HEIGHT/2 - 50, "Press M for Menu");
    drawText(WIDTH/2 - 70, HEIGHT/2 - 90, "Press ESC to Exit");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch(currentState) {
        case MENU: displayMenu(); break;
        case PLAYING: displayPlaying(); break;
        case PAUSED: displayPaused(); break;
        case GAMEOVER: displayGameOver(); break;
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch(currentState) {
        case MENU:
            if(key == '1') {
                currentState = PLAYING;
                score = 0;
                timeLeft = 60;
                initEggs();
            }
            else if(key == '3' || key == 27) {
                exit(0);
            }
            break;

        case PLAYING:
            if(key == 'p' || key == 'P') currentState = PAUSED;
            else if(key == 27) currentState = MENU;
            else if(key == 'a' || key == 'A') basketX -= 30;
            else if(key == 'd' || key == 'D') basketX += 30;
            break;

        case PAUSED:
            if(key == 'p' || key == 'P') currentState = PLAYING;
            else if(key == 27) currentState = MENU;
            break;

        case GAMEOVER:
            if(key == 'm' || key == 'M') {
                currentState = MENU;
            }
            else if(key == 27) exit(0);
            break;
    }

    if(basketX < 0) basketX = 0;
    if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if(currentState == PLAYING) {
        if(key == GLUT_KEY_LEFT) basketX -= 30;
        else if(key == GLUT_KEY_RIGHT) basketX += 30;

        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        glutPostRedisplay();
    }
}

void mouseMotion(int x, int y) {
    if(currentState == PLAYING) {
        basketX = x - BASKET_WIDTH/2;
        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        glutPostRedisplay();
    }
}

void mouseClick(int button, int state, int x, int y) {
    if(currentState == MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int screenY = HEIGHT - y;
        if(screenY > 380 && screenY < 420) {
            currentState = PLAYING;
            score = 0;
            timeLeft = 60;
            initEggs();
        }
        else if(screenY > 300 && screenY < 340) {
            exit(0);
        }
    }
}

void initOpenGL() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Catch The Eggs");

    initOpenGL();
    initEggs();

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}
