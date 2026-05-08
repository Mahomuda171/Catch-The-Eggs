#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>
using namespace std;

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

// Game states
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };
GameState currentState = MENU;

// Game variables
int score = 0;
float timeLeft = 60.0f;
int highScore = 0;

// Basket variables
float basketX = WIDTH / 2 - 50;
const int BASKET_WIDTH = 100;
const int BASKET_HEIGHT = 25;

// Chicken positions (3 chickens)
float chickenX[3] = {200, WIDTH/2, 600};
float chickenY = HEIGHT - 65;

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
void displayMenu();
void displayGameOver();

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
    glVertex2f(x, y);
    for(int i = 0; i <= 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(x + radius * cos(rad), y + radius * sin(rad));
    }
    glEnd();
}

// Draw egg based on type
void drawEgg(float x, float y, int type) {
    if(type == 0) drawCircle(x, y, 12, 1.0f, 0.85f, 0.65f);  // Normal egg
    else if(type == 1) drawCircle(x, y, 12, 1.0f, 0.84f, 0.0f);  // Golden egg
    else if(type == 2) drawCircle(x, y, 12, 0.3f, 0.6f, 1.0f);   // Blue egg
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

// Draw stick/perch
void drawStick() {
    drawRect(0, HEIGHT - 80, WIDTH, 10, 0.5f, 0.35f, 0.15f);
    for(int i = 0; i < 3; i++) {
        drawRect(chickenX[i] + 10, HEIGHT - 90, 8, 15, 0.4f, 0.3f, 0.1f);
    }
}

// Draw single chicken
void drawSingleChicken(float x, float y) {
    // Body
    drawCircle(x, y, 22, 1.0f, 1.0f, 0.0f);
    // Head
    drawCircle(x + 30, y + 10, 13, 1.0f, 0.2f, 0.1f);
    // Eye
    drawCircle(x + 36, y + 15, 3, 0.0f, 0.0f, 0.0f);
}

// Draw all chickens
void drawChicken() {
    for(int i = 0; i < 3; i++) {
        drawSingleChicken(chickenX[i], chickenY);
    }
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
            if(r <= 6) eggs[i].type = 0;      // 70% normal egg
            else if(r == 7) eggs[i].type = 1;  // 10% golden egg
            else if(r == 8) eggs[i].type = 2;  // 10% blue egg
            else eggs[i].type = 3;              // 10% poop
            
            eggs[i].active = true;
            eggs[i].chickenId = chickenChoice;
            break;
        }
    }
}

// Update game logic
void updateGame() {
    if(currentState != PLAYING) return;
    
    // Update egg positions and check collisions
    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            eggs[i].y -= eggs[i].speed;
            
            // Collision with basket
            if(eggs[i].y + 15 < 55 && eggs[i].y + 15 > 30 &&
               eggs[i].x + 12 > basketX && eggs[i].x < basketX + BASKET_WIDTH) {
                
                if(eggs[i].type == 0) score += 1;
                else if(eggs[i].type == 1) score += 10;
                else if(eggs[i].type == 2) score += 5;
                else if(eggs[i].type == 3) score -= 10;
                
                eggs[i].active = false;
            }
            // Remove if off screen
            else if(eggs[i].y < 0) {
                eggs[i].active = false;
            }
        }
    }
    
    // Randomly spawn eggs
    if(rand() % 35 == 0) spawnEgg();
    
    // Update timer
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
}

// Timer function
void timer(int value) {
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Display menu
void displayMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Title
    drawRect(0, HEIGHT/2 + 100, WIDTH, 50, 0.2f, 0.3f, 0.5f);
    glColor3f(1, 1, 1);
    glRasterPos2f(WIDTH/2 - 80, HEIGHT/2 + 130);
    for(char c : "CATCH THE EGGS") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    // Menu options
    glColor3f(1, 1, 0);
    glRasterPos2f(WIDTH/2 - 50, HEIGHT/2 + 50);
    for(char c : "Press S to Start") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    glColor3f(1, 1, 1);
    glRasterPos2f(WIDTH/2 - 70, HEIGHT/2 + 20);
    for(char c : "High Score: ") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    char scoreStr[10];
    sprintf(scoreStr, "%d", highScore);
    glRasterPos2f(WIDTH/2 + 20, HEIGHT/2 + 20);
    for(int i = 0; scoreStr[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    
    glutSwapBuffers();
}

// Display game over screen
void displayGameOver() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawRect(0, HEIGHT/2 + 100, WIDTH, 50, 0.5f, 0.1f, 0.1f);
    glColor3f(1, 1, 1);
    glRasterPos2f(WIDTH/2 - 60, HEIGHT/2 + 130);
    for(char c : "GAME OVER") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    glColor3f(1, 1, 0);
    glRasterPos2f(WIDTH/2 - 50, HEIGHT/2 + 50);
    char scoreStr[30];
    sprintf(scoreStr, "Your Score: %d", score);
    for(int i = 0; scoreStr[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    
    glColor3f(1, 1, 1);
    glRasterPos2f(WIDTH/2 - 80, HEIGHT/2 + 20);
    for(char c : "Press R to Restart") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    glRasterPos2f(WIDTH/2 - 60, HEIGHT/2 - 10);
    for(char c : "Press M for Menu") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    glutSwapBuffers();
}

// Display game
void display() {
    if(currentState == MENU) {
        displayMenu();
    }
    else if(currentState == GAMEOVER) {
        displayGameOver();
    }
    else {
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
        
        // Draw score and time
        glColor3f(1, 1, 1);
        char info[50];
        sprintf(info, "Score: %d  Time: %.0f", score, timeLeft);
        glRasterPos2f(10, HEIGHT - 20);
        for(int i = 0; info[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[i]);
        
        if(currentState == PAUSED) {
            glColor3f(1, 1, 0);
            glRasterPos2f(WIDTH/2 - 30, HEIGHT/2);
            for(char c : "PAUSED") glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        
        glutSwapBuffers();
    }
}

// Keyboard handler
void keyboard(unsigned char key, int x, int y) {
    if(currentState == MENU) {
        if(key == 's' || key == 'S') {
            currentState = PLAYING;
            score = 0;
            timeLeft = 60;
            initEggs();
        }
        else if(key == 27) exit(0); // ESC to exit
    }
    else if(currentState == PLAYING) {
        if(key == 'p' || key == 'P') {
            currentState = PAUSED;
        }
        else if(key == 27) { // ESC
            currentState = MENU;
        }
        else if(key == 'a' || key == 'A') {
            basketX -= 30;
            if(basketX < 0) basketX = 0;
            if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        }
        else if(key == 'd' || key == 'D') {
            basketX += 30;
            if(basketX < 0) basketX = 0;
            if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        }
    }
    else if(currentState == PAUSED) {
        if(key == 'p' || key == 'P') {
            currentState = PLAYING;
        }
        else if(key == 27) {
            currentState = MENU;
        }
    }
    else if(currentState == GAMEOVER) {
        if(key == 'r' || key == 'R') {
            currentState = PLAYING;
            score = 0;
            timeLeft = 60;
            initEggs();
        }
        else if(key == 'm' || key == 'M') {
            currentState = MENU;
        }
    }
    
    glutPostRedisplay();
}

// Special keys (Arrow keys)
void specialKeys(int key, int x, int y) {
    if(currentState == PLAYING) {
        if(key == GLUT_KEY_LEFT) basketX -= 30;
        else if(key == GLUT_KEY_RIGHT) basketX += 30;
        
        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        glutPostRedisplay();
    }
}


// Mouse motion for basket control
void mouseMotion(int x, int y) {
    if(currentState == PLAYING) {
        basketX = x - BASKET_WIDTH/2;
        if(basketX < 0) basketX = 0;
        if(basketX > WIDTH - BASKET_WIDTH) basketX = WIDTH - BASKET_WIDTH;
        glutPostRedisplay();
    }
}

// Initialize OpenGL
void initOpenGL() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
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
    
    srand(time(NULL));
    initOpenGL();
    initEggs();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    return 0;
<<<<<<< HEAD
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


//////////Marwa //////

// Game states
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };
GameState currentState = PLAYING;  // Temporary set to PLAYING for testing

// Game variables
int score = 0;
float timeLeft = 60.0f;
int highScore = 0;

// Function declarations
void updateGame();
void timer(int value);

// Update game logic
void updateGame() {
    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) {
            eggs[i].y -= eggs[i].speed;

            // Collision with basket
            if(eggs[i].y + 15 < 55 && eggs[i].y + 15 > 30 &&
               eggs[i].x + 12 > basketX && eggs[i].x < basketX + BASKET_WIDTH) {

                if(eggs[i].type == 0) score += 1;
                else if(eggs[i].type == 1) score += 10;
                else if(eggs[i].type == 2) score += 5;
                else if(eggs[i].type == 3) score -= 10;

                eggs[i].active = false;
            }
            // Remove if off screen
            else if(eggs[i].y < 0) {
                eggs[i].active = false;
            }
        }
    }

    // Randomly spawn eggs
    if(rand() % 35 == 0) spawnEgg();
}

// Timer function for animation
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

// Draw text function
void drawText(float x, float y, string text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for(char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Update display to show score and time
void display() {
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawStick();
    drawChicken();

    for(int i = 0; i < MAX_EGGS; i++) {
        if(eggs[i].active) drawEgg(eggs[i].x, eggs[i].y, eggs[i].type);
    }

    drawBasket();

    // Show score and time
    drawText(10, HEIGHT - 30, "Score: " + to_string(score));
    drawText(WIDTH - 150, HEIGHT - 30, "Time: " + to_string((int)timeLeft));

    glutSwapBuffers();
}
=======
}
>>>>>>> 78f5641b0d1da422796f8017be18970f36a5f763
