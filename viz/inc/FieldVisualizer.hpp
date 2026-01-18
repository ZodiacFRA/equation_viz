#pragma once

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

#include "../inc/IteratedMap.hpp"
#include "../inc/HenonMap.hpp"
#include "../inc/LorenzMap.hpp"


class FieldVisualizer {
public:
    std::unique_ptr<IteratedMap> map;
    int resolution = 10, iterations = 15;
    float range = 1.0f;
    float cx = 0, cy = 0, cz = 0;

    FieldVisualizer(std::unique_ptr<IteratedMap> m);

    void draw();
    void drawBox();
};
