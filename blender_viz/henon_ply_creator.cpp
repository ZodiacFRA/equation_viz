#include <iostream>
#include <fstream>
#include <vector>

// Global Starting Conditions and Parameters 
// Classic parameters for chaotic behavior: a=1.76, b=0.1
double global_a = 1.76;
double global_b = 0.1;

// Initial coordinates
double global_x = 0.1;
double global_y = 0.1;
double global_z = 0.1;

// Number of iterations
const int ITERATIONS = 100000;

struct Point {
    double x, y, z;
};

int main() {
    std::vector<Point> points;
    points.reserve(ITERATIONS);

    double x = global_x;
    double y = global_y;
    double z = global_z;

    // Compute the map
    for (int i = 0; i < ITERATIONS; ++i) {
        double next_x = global_a - (y * y) - (global_b * z);
        double next_y = x;
        double next_z = y;

        x = next_x;
        y = next_y;
        z = next_z;
        points.push_back({x, y, z});
    }

    // Write to PLY file
    std::ofstream outFile("./renders/henon_3d.ply");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 1;
    }

    // PLY Header
    outFile << "ply\n";
    outFile << "format ascii 1.0\n";
    outFile << "element vertex " << points.size() << "\n";
    outFile << "property double x\n";
    outFile << "property double y\n";
    outFile << "property double z\n";
    outFile << "end_header\n";

    for (const auto& p : points) {
        outFile << p.x << " " << p.y << " " << p.z << "\n";
    }

    outFile.close();
    std::cout << "Successfully generated " << points.size() << " points to henon_3d.ply" << std::endl;

    return 0;
}