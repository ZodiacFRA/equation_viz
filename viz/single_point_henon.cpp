#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <string>

const int MAX_POINTS = 100;
float a = 1.4f;
float b = 0.1f;
float startX = 0.1f, startY = 0.1f, startZ = 0.1f;
bool needsUpdate = true;
bool isBroken = false;

float deltaPos = 0.000001f;

// Camera
float cameraTheta = 0.0f;    
float cameraPhi = 1.5f;      
float cameraRadius = 6.0f;   

struct Point { float x, y, z; };
std::vector<Point> points(MAX_POINTS);

// Scancode Helper (Checks physical key position)
bool isPressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void compute_henon_path() {
    float x = startX; float y = startY; float z = startZ;
    isBroken = false;
    for (int i = 0; i < MAX_POINTS; i++) {
        float next_x = a - (y * y) - (b * z);
        float next_y = x;
        float next_z = y;
        if (!std::isfinite(next_x) || std::abs(next_x) > 20.0f) {
            isBroken = true;
            return; 
        }
        x = next_x; y = next_y; z = next_z;
        points[i] = {x, y, z};
    }
    needsUpdate = false; 
}

void drawGrid(float size, int divisions) {
    float step = size / divisions;
    float halfSize = size / 2.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    glColor4f(0.2f, 0.2f, 0.2f, 0.4f); 
    for (int i = 0; i <= divisions; i++) {
        float pos = -halfSize + i * step;
        glVertex3f(pos, 0, -halfSize); glVertex3f(pos, 0, halfSize);
        glVertex3f(-halfSize, 0, pos); glVertex3f(halfSize, 0, pos);
        glVertex3f(pos, -halfSize, 0); glVertex3f(pos, halfSize, 0);
        glVertex3f(-halfSize, pos, 0); glVertex3f(halfSize, pos, 0);
        glVertex3f(0, pos, -halfSize); glVertex3f(0, pos, halfSize);
        glVertex3f(0, -halfSize, pos); glVertex3f(0, halfSize, pos);
    }
    glEnd();
}

void drawText(float x, float y, std::string text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 1280, 0, 720);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glColor3f(0.8f, 0.8f, 0.8f);
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Henon Explorer: AZERTY Fix", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        float oldA = a, oldB = b, oldX = startX, oldY = startY, oldZ = startZ;

        // Camera Orbit
        if (isPressed(window, GLFW_KEY_LEFT))  cameraTheta -= 0.04f;
        if (isPressed(window, GLFW_KEY_RIGHT)) cameraTheta += 0.04f;
        if (isPressed(window, GLFW_KEY_UP))    cameraPhi -= 0.04f;
        if (isPressed(window, GLFW_KEY_DOWN))  cameraPhi += 0.04f;

        // Params (U/I, J/K)
        if (isPressed(window, GLFW_KEY_U)) a -= 0.002f;
        if (isPressed(window, GLFW_KEY_I)) a += 0.002f;
        if (isPressed(window, GLFW_KEY_J)) b -= 0.002f;
        if (isPressed(window, GLFW_KEY_K)) b += 0.002f;

        // --- PHYSICAL MAPPING FOR AZERTY/QWERTY NEUTRALITY ---
        // Using A and W (AZERTY) / Q and Z (QWERTY)
        // X-Axis: A (top-left) and Z (bottom-leftish)
        if (isPressed(window, GLFW_KEY_T)) startX -= deltaPos;
        if (isPressed(window, GLFW_KEY_Y)) startX += deltaPos;
        
        // Y-Axis: Q (middle-left) and S (middle)
        if (isPressed(window, GLFW_KEY_G)) startY -= deltaPos;
        if (isPressed(window, GLFW_KEY_H)) startY += deltaPos;
        
        // Z-Axis: W (top-middle) and X (bottom-middle)
        if (isPressed(window, GLFW_KEY_B)) startZ -= deltaPos;
        if (isPressed(window, GLFW_KEY_N)) startZ += deltaPos;

        if (isPressed(window, GLFW_KEY_R)) {
            a = 1.4f; b = 0.1f; startX = 0.1f; startY = 0.1f; startZ = 0.1f;
            needsUpdate = true;
        }

        if (a != oldA || b != oldB || startX != oldX || startY != oldY || startZ != oldZ) 
            needsUpdate = true;

        if (needsUpdate) compute_henon_path();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)width/(double)height, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        float camX = cameraRadius * sin(cameraPhi) * cos(cameraTheta);
        float camY = cameraRadius * cos(cameraPhi);
        float camZ = cameraRadius * sin(cameraPhi) * sin(cameraTheta);
        gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

        drawGrid(5.0f, 10);

        if (!isBroken) {
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < MAX_POINTS; i++) {
                float t = (float)i / MAX_POINTS;
                glColor3f(t, t, t);
                glVertex3f(points[i].x, points[i].y, points[i].z);
            }
            glEnd();
        }

        drawText(20, 690, "a: " + std::to_string(a) + "  b: " + std::to_string(b));
        drawText(20, 670, "Seed: [" + std::to_string(startX) + ", " + std::to_string(startY) + ", " + std::to_string(startZ) + "]");
        drawText(20, 650, "A/Z (X)  Q/S (Y)  W/X (Z) - Layout Adjusted");

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}