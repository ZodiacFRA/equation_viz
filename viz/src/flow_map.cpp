#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

#include "../inc/utils.h"


class HenonField {
public:
    float a = 1.4f, b = 0.1f;
    int resolution = 10, iterations = 15;
    float range = 1.0f;
    float cx = 0, cy = 0, cz = 0;

    void draw() {
        float step = (range * 2.0f) / (float)(resolution > 1 ? resolution - 1 : 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                for (int k = 0; k < resolution; k++) {
                    float x = cx - range + (i * step);
                    float y = cy - range + (j * step);
                    float z = cz - range + (k * step);

                    glBegin(GL_LINE_STRIP);
                    for (int n = 0; n < iterations; n++) {
                        float nx = a - (y * y) - (b * z);
                        float ny = x;
                        float nz = y;
                        float dist = sqrt(pow(nx-x,2) + pow(ny-y,2) + pow(nz-z,2));
                        float t = std::min(dist / 1.5f, 1.0f);
                        glColor4f(1.0f - t, 0.2f, t, 0.6f);
                        glVertex3f(x, y, z);
                        x = nx; y = ny; z = nz;
                        if (std::abs(x) > 10.0f) break;
                    }
                    glEnd();
                }
            }
        }
    }
    void drawBox() {
        glPushMatrix();
        glTranslatef(cx, cy, cz);
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        glutWireCube(range * 2.0f);
        glPopMatrix();
    }
};



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Henon Viz", NULL, NULL);
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    Camera cam;
    HenonField field;
    double lastUpdate = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);

        cam.update(window, ctrl);

        // Zoom & Scale Logic
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
            if (ctrl) field.range += 0.02f; else cam.radius -= 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
            if (ctrl) field.range = std::max(0.01f, field.range - 0.02f); else cam.radius += 0.1f;
        }
        cam.radius = std::max(0.2f, cam.radius);

        // Axis-Locked Movement
        if (ctrl) {
            float speed = 0.05f;
            float x = cam.radius * sin(cam.phi) * cos(cam.theta);
            float y = cam.radius * cos(cam.phi);
            float z = cam.radius * sin(cam.phi) * sin(cam.theta);
            float ax = std::abs(x), ay = std::abs(y), az = std::abs(z);

            if (ax >= ay && ax >= az) {
                if (glfwGetKey(window, GLFW_KEY_UP)) field.cy += speed;
                if (glfwGetKey(window, GLFW_KEY_DOWN)) field.cy -= speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cz += speed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cz -= speed;
            } else if (ay >= ax && ay >= az) {
                if (glfwGetKey(window, GLFW_KEY_UP)) field.cx += speed;
                if (glfwGetKey(window, GLFW_KEY_DOWN)) field.cx -= speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cz += speed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cz -= speed;
            } else {
                if (glfwGetKey(window, GLFW_KEY_UP)) field.cy += speed;
                if (glfwGetKey(window, GLFW_KEY_DOWN)) field.cy -= speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cx += speed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cx -= speed;
            }
        }

        // Params Throttling
        if (now - lastUpdate > 0.1) {
            if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) { if(ctrl) field.resolution--; else field.resolution++; }
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { if(ctrl) field.iterations--; else field.iterations++; }
            if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { if(ctrl) field.a -= 0.01f; else field.a += 0.01f; }
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { if(ctrl) field.b -= 0.01f; else field.b += 0.01f; }
            field.resolution = std::clamp(field.resolution, 1, 40);
            field.iterations = std::clamp(field.iterations, 1, 300);
            lastUpdate = now;
        }

        // Screenshot (press Y)
        static bool yReleased = true;
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && yReleased) {
            save_screenshot(window);
            yReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) yReleased = true;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);
        int w, h; glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluPerspective(45.0, (double)w/h, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();

        cam.apply();
        drawInfiniteGrid(cam.theta, cam.phi, cam.radius);
        field.drawBox();
        field.draw();

        // HUD
        float sy = 690, ls = 20;
        drawText(20, sy, "Resolution: " + std::to_string(field.resolution));
        drawText(20, sy-ls, "Iterations: " + std::to_string(field.iterations));
        drawText(20, sy-2*ls, "Origin: [" + std::to_string(field.cx).substr(0,5) + "," + std::to_string(field.cy).substr(0,5) + "," + std::to_string(field.cz).substr(0,5) + "]");
        drawText(20, sy-3*ls, "Grid Size: " + std::to_string(field.range * 2.0f).substr(0,5));
        drawText(20, sy-4*ls, "Param A: " + std::to_string(field.a).substr(0,6));
        drawText(20, sy-5*ls, "Param B: " + std::to_string(field.b).substr(0,6));

        glfwSwapBuffers(window);
        glfwPollEvents();
        while (glfwGetTime() < now + 1.0/60.0);
    }
    glfwTerminate();
    return 0;
}