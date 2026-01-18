#ifndef LORENZ_MAP_H
#define LORENZ_MAP_H

#include "map.h"
#include <string>

// Lorenz attractor: dx/dt = σ(y-x), dy/dt = x(ρ-z)-y, dz/dt = xy-βz
// Discretized iteration (Euler method with adaptive sub-stepping for accuracy)
class LorenzMap : public IteratedMap {
public:
    float sigma = 10.0f;
    float rho = 28.0f;
    float beta = 2.667f;
    float dt = 0.001f;      // Small timestep for accurate integration
    int substeps = 10;      // Number of sub-steps per iterate() call

    void iterate(float& x, float& y, float& z) override {
        // Sub-step integration for better accuracy
        for (int s = 0; s < substeps; s++) {
            float dx = sigma * (y - x);
            float dy = x * (rho - z) - y;
            float dz = x * y - beta * z;
            
            x += dt * dx;
            y += dt * dy;
            z += dt * dz;
        }
    }

    float getParam(const std::string& name) const override {
        if (name == "sigma") return sigma;
        if (name == "rho") return rho;
        if (name == "beta") return beta;
        return 0.0f;
    }

    void setParam(const std::string& name, float value) override {
        if (name == "sigma") sigma = value;
        if (name == "rho") rho = value;
        if (name == "beta") beta = value;
    }

    const char* getName() const override {
        return "Lorenz Attractor";
    }

    float getScale() const override {
        return 30.0f;
    }

    bool hasEscaped(float x, float y, float z) const override {
        // Lorenz attractor stays within a bounded region (~30 units in each dimension)
        return x*x + y*y + z*z > 3000.0f;
    }

    int getDefaultResolution() const override { return 8; }
    int getDefaultIterations() const override { return 100; }
};

#endif // LORENZ_MAP_H
