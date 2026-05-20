#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

// ============= GAME LOGIC ===================================

enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };
GameState currentState = MENU;

// Game variables
float basketX = WIDTH / 2 - 50;
const int BASKET_WIDTH = 100;
const int BASKET_HEIGHT = 25;
int score = 0;
float timeLeft = 60.0f;
int highScore = 0;

// Moving chickens
struct Chicken {
    float x;
    float y;
    float speed;
    int direction;  // 1 = right, -1 = left
    bool active;
};

Chicken chickens[3];
float chickenY = HEIGHT - 70;

// Egg
struct Egg {
    float x, y;
    float speed;
    int type;
    bool active;
    int chickenId;
};

const int MAX_EGGS = 40;
Egg eggs[MAX_EGGS];

// Function
void initChickens();
void initEggs();
void spawnEgg();
void updateGame();
void updateChickens();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void timer(int value);

// ============= DRAWING FUNCTIONS ============================

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

// Draw egg
void drawEgg(float x, float y, int type) {
    if (type == 0) glColor3f(1.0f, 0.85f, 0.65f);  // Normal
    else if (type == 1) glColor3f(1.0f, 0.84f, 0.0f);  // Golden
    else if (type == 2) glColor3f(0.3f, 0.6f, 1.0f);   // Blue
    else glColor3f(0.4f, 0.2f, 0.1f);  // Poop

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i += 20) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x + 12 * cos(rad), y + 15 * sin(rad));
    }
    glEnd();
}

// Draw basket
void drawBasket() {
    // Basket shadow
    drawRect(basketX + 5, 28, BASKET_WIDTH, BASKET_HEIGHT, 0.3f, 0.3f, 0.3f);

    // Basket body
    drawRect(basketX, 30, BASKET_WIDTH, BASKET_HEIGHT, 0.8f, 0.5f, 0.2f);

    // Basket weave pattern
    glColor3f(0.5f, 0.3f, 0.1f);
    for(int i = 0; i < 3; i++) {
        glBegin(GL_LINES);
        glVertex2f(basketX + 5, 38 + i * 8);
        glVertex2f(basketX + BASKET_WIDTH - 5, 38 + i * 8);
        glEnd();
    }

    // Basket rim
    drawRect(basketX - 3, 52, BASKET_WIDTH + 6, 6, 0.7f, 0.4f, 0.1f);
}

// Draw stick
void drawStick() {
    // Main stick
    drawRect(0, HEIGHT - 80, WIDTH, 10, 0.5f, 0.35f, 0.15f);

    // Bamboo joints
    glColor3f(0.4f, 0.3f, 0.1f);
    for(int i = 1; i <= 8; i++) {
        glBegin(GL_LINES);
        glVertex2f(i * 100, HEIGHT - 80);
        glVertex2f(i * 100, HEIGHT - 70);
        glEnd();
    }
}

// Draw single chicken
void drawSingleChicken(float x, float y, int chickenNum) {
    // Body (yellow)
    drawCircle(x, y, 22, 1.0f, 0.85f, 0.2f);

    // Belly (lighter)
    drawCircle(x, y - 3, 16, 1.0f, 0.9f, 0.4f);

    // Wing
    glColor3f(0.9f, 0.7f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
    for(int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x - 12 + 10 * cos(rad), y + 2 + 8 * sin(rad));
    }
    glEnd();

    // Head
    drawCircle(x + 28, y + 5, 14, 1.0f, 0.8f, 0.2f);

    // Beak
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 42, y + 8);
    glVertex2f(x + 52, y + 5);
    glVertex2f(x + 42, y + 2);
    glEnd();

    // Eye (white)
    drawCircle(x + 38, y + 8, 5, 1.0f, 1.0f, 1.0f);
    // Eye pupil (black)
    drawCircle(x + 40, y + 9, 2.5f, 0.0f, 0.0f, 0.0f);
    // Eye highlight
    drawCircle(x + 41, y + 10, 1, 1.0f, 1.0f, 1.0f);
    // Cheek blush
    drawCircle(x + 28, y + 2, 4, 1.0f, 0.5f, 0.5f);

    // Tail feathers
    glColor3f(0.8f, 0.6f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 18, y + 5);
    glVertex2f(x - 28, y + 12);
    glVertex2f(x - 22, y - 2);
    glVertex2f(x - 18, y + 3);
    glVertex2f(x - 30, y + 5);
    glVertex2f(x - 22, y - 5);
    glEnd();

    // Feet (simple lines - no green triangle)
    glColor3f(1.0f, 0.6f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(x - 8, y - 20);
    glVertex2f(x - 15, y - 28);
    glVertex2f(x - 8, y - 20);
    glVertex2f(x - 3, y - 28);
    glVertex2f(x + 5, y - 20);
    glVertex2f(x - 2, y - 28);
    glVertex2f(x + 5, y - 20);
    glVertex2f(x + 10, y - 28);
    glEnd();

}

// Draw all chickens
void drawChicken() {
    for(int i = 0; i < 3; i++) {
        if(chickens[i].active) {
            drawSingleChicken(chickens[i].x, chickens[i].y, i);
        }
    }
}

// Draw background
void drawBackground() {
    // Sky
    drawRect(0, 0, WIDTH, HEIGHT, 0.5f, 0.7f, 0.9f);
    // Sun
    drawCircle(700, 500, 60, 1.0f, 0.9f, 0.2f); // glow
    drawCircle(700, 500, 40, 1.0f, 0.85f, 0.0f); // core
    // Clouds
    // Clouds (clean and correct)

glColor3f(1.0f, 1.0f, 1.0f);
// Clouds (moved a bit higher)

glColor3f(1.0f, 1.0f, 1.0f);

// Cloud 1 (higher)
// Clouds (professional clean version)
drawCircle(150, HEIGHT - 120, 25, 1, 1, 1);
drawCircle(180, HEIGHT - 110, 30, 1, 1, 1);
drawCircle(210, HEIGHT - 120, 25, 1, 1, 1);

drawCircle(400, HEIGHT - 90, 25, 1, 1, 1);
drawCircle(430, HEIGHT - 80, 30, 1, 1, 1);
drawCircle(460, HEIGHT - 90, 25, 1, 1, 1);

drawCircle(620, HEIGHT - 140, 25, 1, 1, 1);
drawCircle(650, HEIGHT - 130, 30, 1, 1, 1);
drawCircle(680, HEIGHT - 140, 25, 1, 1, 1);
    // Ground
    drawRect(0, 0, WIDTH, 80, 0.3f, 0.6f, 0.2f);
}

// Draw text
void drawText(float x, float y, string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for(char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// ============= DISPLAY FUNCTIONS ============================

void displayMenu() {
    drawBackground();




    drawText(WIDTH/2 - 70, HEIGHT - 150, "CATCH THE EGGS!", GLUT_BITMAP_TIMES_ROMAN_24);
    drawText(WIDTH/2 - 60, HEIGHT - 220, "1. Start Game");
    drawText(WIDTH/2 - 60, HEIGHT - 260, "2. High Score: " + to_string(highScore));
    drawText(WIDTH/2 - 60, HEIGHT - 300, "3. Exit");

    drawText(WIDTH/2 - 175, HEIGHT - 360, "Controls: Arrow Keys or Mouse to move basket");
    drawText(WIDTH/2 - 145, HEIGHT - 390, "Press P to pause, ESC to exit during game");
    drawText(WIDTH/2 - 125, HEIGHT - 420, "Catch: Golden(10) Blue(5) Normal(1)");
    drawText(WIDTH/2 - 110, HEIGHT - 450, "Avoid Poop - Brown Color(-10)");
    drawText(WIDTH/2 - 145, HEIGHT - 480, "3 Chickens are walking and laying eggs!");
}

void displayPlaying() {
    drawBackground();
    drawStick();
    drawChicken();

    // Draw eggs
    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            drawEgg(eggs[i].x, eggs[i].y, eggs[i].type);
        }
    }
    drawBasket();

    // HUD
    drawText(10, HEIGHT - 30, "Score: " + to_string(score));
    drawText(WIDTH - 150, HEIGHT - 30, "Time: " + to_string((int)timeLeft));
}

void displayPaused() {
    displayPlaying();
    drawRect(WIDTH/2 - 120, HEIGHT/2 - 50, 240, 100, 0.0f, 0.0f, 0.0f);
    drawText(20, HEIGHT - 30, "SCORE: " + to_string(score));
    drawText(300, HEIGHT - 30, "TIME: " + to_string((int)timeLeft));
    drawText(550, HEIGHT - 30, "HIGH SCORE: " + to_string(highScore));
    drawText(WIDTH/2 - 35, HEIGHT/2 + 20, "PAUSED");
    drawText(WIDTH/2 - 55, HEIGHT/2 - 10, "Press P to resume");
}

void displayGameOver() {
    drawBackground();
    drawRect(WIDTH/2 - 150, HEIGHT/2 - 80, 300, 160, 0.0f, 0.0f, 0.0f);
    drawText(WIDTH/2 - 50, HEIGHT/2 + 50, "GAME OVER!", GLUT_BITMAP_TIMES_ROMAN_24);
    drawText(WIDTH/2 - 40, HEIGHT/2 + 20, "Score: " + to_string(score));
    drawText(WIDTH/2 - 55, HEIGHT/2 - 10, "High Score: " + to_string(highScore));
    drawText(WIDTH/2 - 60, HEIGHT/2 - 50, "Press M for Menu");
}

void display() {
    switch(currentState) {
        case MENU: displayMenu(); break;
        case PLAYING: displayPlaying(); break;
        case PAUSED: displayPaused(); break;
        case GAMEOVER: displayGameOver(); break;
    }
    glutSwapBuffers();
}

// ============= GAME LOGIC IMPLEMENTATION ====================

void initChickens() {
    chickens[0] = {150, chickenY, 1.5f, 1, true};
    chickens[1] = {WIDTH/2, chickenY, 1.2f, -1, true};
    chickens[2] = {650, chickenY, 1.8f, 1, true};
}

void initEggs() {
    for(int i = 0; i < MAX_EGGS; i++) {
        eggs[i].active = false;
        eggs[i].chickenId = -1;
    }
}

void updateChickens() {
    for(int i = 0; i < 3; i++) {
        if(chickens[i].active) {
            chickens[i].x += chickens[i].speed * chickens[i].direction;

// Up-down floating animation
            chickens[i].y = chickenY + sin(glutGet(GLUT_ELAPSED_TIME) * 0.005) * 5;

            if(chickens[i].x < 80) {
                chickens[i].x = 80;
                chickens[i].direction = 1;
            }
            if(chickens[i].x > WIDTH - 80) {
                chickens[i].x = WIDTH - 80;
                chickens[i].direction = -1;
            }
        }
    }
}

void spawnEgg() {
    for(int i = 0; i < MAX_EGGS; i++) {
        if(!eggs[i].active) {
            int chickenChoice = rand() % 3;
            if(chickens[chickenChoice].active) {
                eggs[i].x = chickens[chickenChoice].x + 5;
                eggs[i].y = chickens[chickenChoice].y - 15;
                eggs[i].speed = 2.5f + (rand() % 50) / 20.0f;

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
}

void updateGame() {
    updateChickens();

    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            eggs[i].y -= eggs[i].speed * 0.7f;

            // Collision with basket
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

    if(rand() % 30 == 0) spawnEgg();
}

// ============= INPUT HANDLERS ===============================

void keyboard(unsigned char key, int x, int y) {
    switch(currentState) {
        case MENU:
            if(key == '1') {
                currentState = PLAYING;
                score = 0;
                timeLeft = 60;
                initChickens();
                initEggs();
            }
            else if(key == '3' || key == 27) exit(0);
            break;

        case PLAYING:
            if(key == 'p' || key == 'P') currentState = PAUSED;
            else if(key == 27) currentState = MENU;
            else if(key == 'a' || key == 'A') basketX -= 35;
            else if(key == 'd' || key == 'D') basketX += 35;
            break;

        case PAUSED:
            if(key == 'p' || key == 'P') currentState = PLAYING;
            else if(key == 27) currentState = MENU;
            break;

        case GAMEOVER:
            if(key == 'm' || key == 'M') {
                if(score > highScore) highScore = score;
                currentState = MENU;
            }
            break;
    }

    if(basketX < 0) basketX = 0;
    if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if(currentState == PLAYING) {
        if(key == GLUT_KEY_LEFT) basketX -= 35;
        else if(key == GLUT_KEY_RIGHT) basketX += 35;

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
            initChickens();
            initEggs();
        }
        else if(screenY > 300 && screenY < 340) {
            exit(0);
        }
    }
}

void timer(int value) {
    if(currentState == PLAYING) {
        timeLeft -= 0.016f;
        updateGame();

        if(timeLeft <= 0) {
            if(score > highScore) highScore = score;
            currentState = GAMEOVER;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void initOpenGL() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

// ============= MAIN FUNCTION ================================

int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Catch The Eggs - Moving Chickens!");

    initOpenGL();
    initChickens();
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
