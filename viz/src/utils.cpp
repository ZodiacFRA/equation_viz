#include "../inc/utils.h"
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <ctime>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"


void Camera::update(GLFWwindow* window, bool ctrl) {
    if (!ctrl) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  theta += 0.03f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) theta -= 0.03f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    phi -= 0.03f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  phi += 0.03f;
    }
}

void Camera::apply() {
    float x = radius * sin(phi) * cos(theta);
    float y = radius * cos(phi);
    float z = radius * sin(phi) * sin(theta);
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
}

void drawInfiniteGrid(float camTheta, float camPhi, float camRadius) {
    float camX = camRadius * sin(camPhi) * cos(camTheta);
    float camZ = camRadius * sin(camPhi) * sin(camTheta);
    float size = 20.0f, step = 0.5f;
    float offsetX = floor(camX / step) * step;
    float offsetZ = floor(camZ / step) * step;
    glBegin(GL_LINES);
    glColor4f(0.15f, 0.15f, 0.15f, 0.4f);
    for (float i = -size; i <= size; i += step) {
        glVertex3f(i + offsetX, 0, -size + offsetZ); glVertex3f(i + offsetX, 0, size + offsetZ);
        glVertex3f(-size + offsetX, 0, i + offsetZ); glVertex3f(size + offsetX, 0, i + offsetZ);
    }
    glEnd();
}

void drawText(float x, float y, std::string text) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 1280, 0, 720);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (unsigned char)c);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

void save_screenshot(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // 3 bytes per pixel (RGB)
    std::vector<unsigned char> pixels(3 * width * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // OpenGL images are upside down, so we flip the rows
    std::vector<unsigned char> flippedPixels(3 * width * height);
    for (int row = 0; row < height; row++) {
        memcpy(&flippedPixels[row * width * 3], 
               &pixels[(height - row - 1) * width * 3], 
               width * 3);
    }

    time_t now = time(0);
    char filename[100];
    strftime(filename, sizeof(filename), "renders/flow_map_%Y%m%d_%H%M%S.png", localtime(&now));

    if (stbi_write_png(filename, width, height, 3, flippedPixels.data(), width * 3)) {
        printf("Screenshot saved: %s\n", filename);
    } else {
        printf("Failed to save screenshot.\n");
    }
}

void printUsage(const char* progName) {
    printf("Usage: %s [map_name]\n", progName);
    printf("\nAvailable maps:\n");
    printf("  henon     - Henon map (default)\n");
    printf("  lorenz    - Lorenz attractor\n");
}