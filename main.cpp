#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

const int WIDTH = 800, HEIGHT = 600;

// Game state
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };
GameState currentState = MENU;

// Game variables
float basketX = WIDTH/2 - 50;
const int BASKET_W = 100, BASKET_H = 25;
int score = 0;
float timeLeft = 60;
int highScore = 0;

// Chicken
struct Chicken {
    float x, y, speed;
    int dir;
    bool active;
};
Chicken chickens[3];
const float chickenY = HEIGHT - 70;

// Egg
struct Egg {
    float x, y, speed;
    int type;
    bool active;
};
Egg eggs[40];

// Function prototypes
void initGame();
void updateGame();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void timer(int);

// ============= MIDPOINT CIRCLE FUNCTIONS =============

// Filled circle using midpoint + scanline
void drawCircle(int cx, int cy, int r, float R, float G, float B) {
    glColor3f(R, G, B);
    for (int y = -r; y <= r; y++) {
        int x = sqrt(r*r - y*y);
        glBegin(GL_LINES);
        glVertex2f(cx - x, cy + y);
        glVertex2f(cx + x, cy + y);
        glEnd();
    }
}

// Egg shape (ellipse)
void drawEgg(int x, int y, int type) {
    float R, G, B;
    if (type == 0) { R=1.0; G=0.85; B=0.65; }      // Normal
    else if (type == 1) { R=1.0; G=0.84; B=0.0; }  // Golden
    else if (type == 2) { R=0.3; G=0.6; B=1.0; }   // Blue
    else { R=0.4; G=0.2; B=0.1; }                  // Poop

    glColor3f(R, G, B);
    for (int y1 = -16; y1 <= 16; y1++) {
        int x1 = 12 * sqrt(1 - (float)(y1*y1)/(16*16));
        glBegin(GL_LINES);
        glVertex2f(x - x1, y + y1);
        glVertex2f(x + x1, y + y1);
        glEnd();
    }
}

// ============= DRAWING FUNCTIONS =============

void drawRect(float x, float y, float w, float h, float R, float G, float B) {
    glColor3f(R, G, B);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawBasket() {
    // Basket shadow
    drawRect(basketX + 5, 28, BASKET_W, BASKET_H, 0.3, 0.3, 0.3);
    // Basket body
    drawRect(basketX, 30, BASKET_W, BASKET_H, 0.8, 0.5, 0.2);
    // Basket weave pattern
    glColor3f(0.5, 0.3, 0.1);
    for(int i = 0; i < 3; i++) {
        glBegin(GL_LINES);
        glVertex2f(basketX + 5, 38 + i * 8);
        glVertex2f(basketX + BASKET_W - 5, 38 + i * 8);
        glEnd();
    }
    // Basket rim
    drawRect(basketX - 3, 52, BASKET_W + 6, 6, 0.7, 0.4, 0.1);
}

void drawStick() {
    drawRect(0, HEIGHT-80, WIDTH, 10, 0.5, 0.35, 0.15);
    // Bamboo joints
    glColor3f(0.4, 0.3, 0.1);
    for(int i = 1; i <= 8; i++) {
        glBegin(GL_LINES);
        glVertex2f(i * 100, HEIGHT-80);
        glVertex2f(i * 100, HEIGHT-70);
        glEnd();
    }
}

void drawChicken(float x, float y) {
    // Body
    drawCircle(x, y, 22, 1.0, 0.85, 0.2);
    drawCircle(x, y-3, 16, 1.0, 0.9, 0.4);
    // Wing
    glColor3f(0.9, 0.7, 0.2);
    glBegin(GL_TRIANGLE_FAN);
    for(int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x - 12 + 10 * cos(rad), y + 2 + 8 * sin(rad));
    }
    glEnd();
    // Head
    drawCircle(x+28, y+5, 14, 1.0, 0.8, 0.2);
    // Beak
    glColor3f(1.0, 0.5, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x+42, y+8); glVertex2f(x+52, y+5); glVertex2f(x+42, y+2);
    glEnd();
    // Eye
    drawCircle(x+38, y+8, 5, 1.0, 1.0, 1.0);
    drawCircle(x+40, y+9, 2.5, 0, 0, 0);
    drawCircle(x+41, y+10, 1, 1.0, 1.0, 1.0);
    // Cheek blush
    drawCircle(x+28, y+2, 4, 1.0, 0.5, 0.5);
    // Tail feathers
    glColor3f(0.8, 0.6, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(x-18, y+5); glVertex2f(x-28, y+12); glVertex2f(x-22, y-2);
    glVertex2f(x-18, y+3); glVertex2f(x-30, y+5); glVertex2f(x-22, y-5);
    glEnd();
    // Feet
    glColor3f(1.0, 0.6, 0);
    glBegin(GL_LINES);
    glVertex2f(x-8, y-20); glVertex2f(x-15, y-28);
    glVertex2f(x-8, y-20); glVertex2f(x-3, y-28);
    glVertex2f(x+5, y-20); glVertex2f(x-2, y-28);
    glVertex2f(x+5, y-20); glVertex2f(x+10, y-28);
    glEnd();
}

void drawBackground() {
    // Sky
    drawRect(0, 0, WIDTH, HEIGHT, 0.5, 0.7, 0.9);
    // Sun
    drawCircle(700, 500, 60, 1.0, 0.9, 0.2);
    drawCircle(700, 500, 40, 1.0, 0.85, 0.0);
    // Clouds
    drawCircle(150, HEIGHT-120, 25, 1,1,1);
    drawCircle(180, HEIGHT-110, 30, 1,1,1);
    drawCircle(210, HEIGHT-120, 25, 1,1,1);
    drawCircle(400, HEIGHT-90, 25, 1,1,1);
    drawCircle(430, HEIGHT-80, 30, 1,1,1);
    drawCircle(460, HEIGHT-90, 25, 1,1,1);
    drawCircle(620, HEIGHT-140, 25, 1,1,1);
    drawCircle(650, HEIGHT-130, 30, 1,1,1);
    drawCircle(680, HEIGHT-140, 25, 1,1,1);
    // Ground
    drawRect(0, 0, WIDTH, 80, 0.3, 0.6, 0.2);
}

void drawText(float x, float y, string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glColor3f(1,1,1);
    glRasterPos2f(x, y);
    for(char c : text) glutBitmapCharacter(font, c);
}

// ============= DISPLAY =============

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
    for(int i=0; i<3; i++)
        if(chickens[i].active) drawChicken(chickens[i].x, chickens[i].y);
    for(int i=0; i<40; i++)
        if(eggs[i].active) drawEgg(eggs[i].x, eggs[i].y, eggs[i].type);
    drawBasket();
    drawText(10, HEIGHT-30, "Score: " + to_string(score));
    drawText(WIDTH-150, HEIGHT-30, "Time: " + to_string((int)timeLeft));
}

void displayPaused() {
    displayPlaying();
    drawRect(WIDTH/2 - 120, HEIGHT/2 - 50, 240, 100, 0,0,0);
    drawText(WIDTH/2 - 35, HEIGHT/2 + 20, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
    drawText(WIDTH/2 - 55, HEIGHT/2 - 10, "Press P to resume");
}

void displayGameOver() {
    drawBackground();
    drawRect(WIDTH/2 - 150, HEIGHT/2 - 80, 300, 160, 0,0,0);
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

// ============= GAME LOGIC =============

void initGame() {
    chickens[0] = {150, chickenY, 1.5, 1, true};
    chickens[1] = {WIDTH/2, chickenY, 1.2, -1, true};
    chickens[2] = {650, chickenY, 1.8, 1, true};
    for(int i=0; i<40; i++) eggs[i].active = false;
    score = 0;
    timeLeft = 60;
}

void updateGame() {
    // Move chickens (only left-right)
    for(int i=0; i<3; i++) {
        if(chickens[i].active) {
            chickens[i].x += chickens[i].speed * chickens[i].dir;
            if(chickens[i].x < 80) { chickens[i].x = 80; chickens[i].dir = 1; }
            if(chickens[i].x > WIDTH-80) { chickens[i].x = WIDTH-80; chickens[i].dir = -1; }
        }
    }

    // Spawn eggs randomly
    if(rand() % 30 == 0) {
        for(int i=0; i<40; i++) {
            if(!eggs[i].active) {
                int ch = rand() % 3;
                if(chickens[ch].active) {
                    eggs[i].x = chickens[ch].x + 5;
                    eggs[i].y = chickens[ch].y - 20;
                    eggs[i].speed = 2.5 + (rand()%50)/20.0;
                    int r = rand() % 10;
                    if(r <= 6) eggs[i].type = 0;
                    else if(r == 7) eggs[i].type = 1;
                    else if(r == 8) eggs[i].type = 2;
                    else eggs[i].type = 3;
                    eggs[i].active = true;
                    break;
                }
            }
        }
    }

    // Update eggs
    for(int i=0; i<40; i++) {
        if(eggs[i].active) {
            eggs[i].y -= eggs[i].speed * 0.7;

            // Collision with basket
            if(eggs[i].y + 16 < 55 && eggs[i].y + 16 > 30 &&
               eggs[i].x + 12 > basketX && eggs[i].x - 12 < basketX + BASKET_W) {
                if(eggs[i].type == 0) score += 1;
                else if(eggs[i].type == 1) score += 10;
                else if(eggs[i].type == 2) score += 5;
                else score -= 10;
                eggs[i].active = false;
            }
            else if(eggs[i].y - 16 < 0) eggs[i].active = false;
        }
    }
}

// ============= INPUT HANDLERS =============

void keyboard(unsigned char key, int x, int y) {
    if(currentState == MENU) {
        if(key == '1') { currentState = PLAYING; initGame(); }
        if(key == '3' || key == 27) exit(0);
    }
    else if(currentState == PLAYING) {
        if(key == 'p' || key == 'P') currentState = PAUSED;
        if(key == 27) currentState = MENU;
        if(key == 'a' || key == 'A') basketX -= 35;
        if(key == 'd' || key == 'D') basketX += 35;
    }
    else if(currentState == PAUSED) {
        if(key == 'p' || key == 'P') currentState = PLAYING;
        if(key == 27) currentState = MENU;
    }
    else if(currentState == GAMEOVER) {
        if(key == 'm' || key == 'M') {
            if(score > highScore) highScore = score;
            currentState = MENU;
        }
    }
    if(basketX < 0) basketX = 0;
    if(basketX > WIDTH - BASKET_W) basketX = WIDTH - BASKET_W;
    glutPostRedisplay();
}


void specialKeys(int key, int x, int y) {
    if(currentState == PLAYING) {
        if(key == GLUT_KEY_LEFT) basketX -= 35;
        if(key == GLUT_KEY_RIGHT) basketX += 35;
        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_W) basketX = WIDTH - BASKET_W;
        glutPostRedisplay();
    }
}

void mouseMotion(int x, int y) {
    if(currentState == PLAYING) {
        basketX = x - BASKET_W/2;
        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_W) basketX = WIDTH - BASKET_W;
        glutPostRedisplay();
    }
}

void mouseClick(int button, int state, int x, int y) {
    if(currentState == MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int screenY = HEIGHT - y;
        if(screenY > 380 && screenY < 420) {
            currentState = PLAYING;
            initGame();
        }
        else if(screenY > 300 && screenY < 340) {
            exit(0);
        }
    }
}

void timer(int) {
    if(currentState == PLAYING) {
        timeLeft -= 0.016;
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
    glClearColor(0.5, 0.7, 0.9, 1);
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
    glutCreateWindow("Catch The Eggs - Midpoint Circle");
    initOpenGL();
    initGame();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);
    glutMainLoop();
    return 0;
}
