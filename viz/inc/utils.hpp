#pragma once

#include <string>

#include <GLFW/glfw3.h>

// Camera class: spherical coordinate camera for 3D visualization
class Camera {
public:
    float theta = 0.5f, phi = 1.2f, radius = 5.0f;
    
    void update(GLFWwindow* window, bool ctrl);
    void apply();
};

// Render an infinite grid centered on camera position
void drawInfiniteGrid(float camTheta, float camPhi, float camRadius);

// Draw text at screen coordinates (0-1280, 0-720 in normalized space)
void drawText(float x, float y, std::string text);

// Save framebuffer to PNG file in renders/ directory with timestamp
void save_screenshot(GLFWwindow* window);

// Print command-line usage instructions
void printUsage(const char* progName);