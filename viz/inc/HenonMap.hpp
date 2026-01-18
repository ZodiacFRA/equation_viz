#pragma once
#include <string>

#include "IteratedMap.hpp"

// Henon map: x_{n+1} = a - y_n^2 - b*z_n, y_{n+1} = x_n, z_{n+1} = y_n
class HenonMap : public IteratedMap {
public:
    float a = 1.4f;
    float b = 0.1f;

    void iterate(float& x, float& y, float& z) override {
        float nx = a - (y * y) - (b * z);
        float ny = x;
        float nz = y;
        x = nx;
        y = ny;
        z = nz;
    }

    float getParam(const std::string& name) const override {
        if (name == "a") return a;
        if (name == "b") return b;
        return 0.0f;
    }

    void setParam(const std::string& name, float value) override {
        if (name == "a") a = value;
        if (name == "b") b = value;
    }

    const char* getName() const override {
        return "Henon Map";
    }

    float getScale() const override {
        return 1.0f;
    }

    int getDefaultResolution() const override { return 10; }
    int getDefaultIterations() const override { return 15; }
};
