#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include <memory>
#include <iostream>

#include "../inc/utils.h"
#include "../inc/map.h"
#include "../inc/henon_map.h"
#include "../inc/lorenz_map.h"


class FieldVisualizer {
public:
    std::unique_ptr<IteratedMap> map;
    int resolution = 10, iterations = 15;
    float range = 1.0f;
    float cx = 0, cy = 0, cz = 0;

    FieldVisualizer(std::unique_ptr<IteratedMap> m) : map(std::move(m)) {}

    void draw() {
        const float scale = map->getScale();
        float step = (range * 2.0f) / (float)(resolution > 1 ? resolution - 1 : 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                for (int k = 0; k < resolution; k++) {
                    // Initial point in visualization space, scaled to map space
                    float x = (cx - range + (i * step)) * scale;
                    float y = (cy - range + (j * step)) * scale;
                    float z = (cz - range + (k * step)) * scale;

                    glBegin(GL_LINE_STRIP);
                    for (int n = 0; n < iterations; n++) {
                        float px = x, py = y, pz = z;
                        map->iterate(x, y, z);

                        // dist is in map space, scale it back for color
                        float dist = sqrt(pow(x-px,2) + pow(y-py,2) + pow(z-pz,2));
                        float t = std::min((dist / scale) / 1.5f, 1.0f);
                        glColor4f(1.0f - t, 0.2f, t, 0.6f);

                        // draw vertex in visualization space
                        glVertex3f(px / scale, py / scale, pz / scale);
                        
                        if (map->hasEscaped(x, y, z)) break;
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Equation Viz", NULL, NULL);
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    Camera cam;
    
    // Parse command-line argument for map selection
    std::string mapName = "henon";  // Default
    if (argc > 1) {
        mapName = argv[1];
        if (mapName == "--help" || mapName == "-h") {
            printUsage(argv[0]);
            glfwTerminate();
            return 0;
        }
    }
    
    std::unique_ptr<IteratedMap> map;
    if (mapName == "henon" || mapName == "henon_map") {
        map = std::make_unique<HenonMap>();
    } else if (mapName == "lorenz" || mapName == "lorenz_map") {
        map = std::make_unique<LorenzMap>();
    } else {
        map = std::make_unique<HenonMap>();  // Default
    }
    
    FieldVisualizer field(std::move(map));
    // Initialize field parameters from map defaults
    field.resolution = field.map->getDefaultResolution();
    field.iterations = field.map->getDefaultIterations();
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
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cz += speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cz -= speed;
            } else if (ay >= ax && ay >= az) {
                if (glfwGetKey(window, GLFW_KEY_UP)) field.cx += speed;
                if (glfwGetKey(window, GLFW_KEY_DOWN)) field.cx -= speed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cz += speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cz -= speed;
            } else {
                if (glfwGetKey(window, GLFW_KEY_UP)) field.cy += speed;
                if (glfwGetKey(window, GLFW_KEY_DOWN)) field.cy -= speed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT)) field.cx += speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT)) field.cx -= speed;
            }
        }

        // Params Throttling
        if (now - lastUpdate > 0.1) {
            if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) { if(ctrl) field.resolution--; else field.resolution++; }
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { if(ctrl) field.iterations--; else field.iterations++; }
            // Map-specific parameters (cast to access HenonMap)
            if (auto* henonMap = dynamic_cast<HenonMap*>(field.map.get())) {
                if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { if(ctrl) henonMap->a -= 0.01f; else henonMap->a += 0.01f; }
                if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { if(ctrl) henonMap->b -= 0.01f; else henonMap->b += 0.01f; }
            }
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
        drawText(20, sy, "Map: " + std::string(field.map->getName()));
        drawText(20, sy-ls, "Resolution: " + std::to_string(field.resolution));
        drawText(20, sy-2*ls, "Iterations: " + std::to_string(field.iterations));
        drawText(20, sy-3*ls, "Origin: [" + std::to_string(field.cx).substr(0,5) + "," + std::to_string(field.cy).substr(0,5) + "," + std::to_string(field.cz).substr(0,5) + "]");
        drawText(20, sy-4*ls, "Grid Size: " + std::to_string(field.range * 2.0f).substr(0,5));
        
        // Display map-specific parameters
        if (auto* henonMap = dynamic_cast<HenonMap*>(field.map.get())) {
            drawText(20, sy-5*ls, "Param A: " + std::to_string(henonMap->a).substr(0,6));
            drawText(20, sy-6*ls, "Param B: " + std::to_string(henonMap->b).substr(0,6));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        while (glfwGetTime() < now + 1.0/60.0);
    }
    glfwTerminate();
    return 0;
}