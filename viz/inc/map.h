#ifndef MAP_H
#define MAP_H

#include <cmath>
#include <array>
#include <string>

// Base class for iterated maps (x_{n+1} = f(x_n, y_n, z_n))
class IteratedMap {
public:
    virtual ~IteratedMap() = default;

    // Core iteration: compute next point given current point
    virtual void iterate(float& x, float& y, float& z) = 0;

    // Check if trajectory has escaped (prevents visual artifacts)
    virtual bool hasEscaped(float x, float y, float z) const {
        return std::abs(x) > 10.0f;
    }

    // Get scaling factor for visualization
    virtual float getScale() const = 0;

    // Get parameter by name (optional, for generic param handling)
    virtual float getParam(const std::string& name) const {
        return 0.0f;
    }

    // Set parameter by name (optional, for generic param handling)
    virtual void setParam(const std::string& name, float value) {}

    // Get name of this map
    virtual const char* getName() const = 0;

    // Visualization parameters (map-specific defaults)
    virtual int getDefaultResolution() const { return 10; }
    virtual int getDefaultIterations() const { return 15; }
};

#endif // MAP_H
