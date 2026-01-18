#include "../inc/FieldVisualizer.hpp"

FieldVisualizer::FieldVisualizer(std::unique_ptr<IteratedMap> m) : map(std::move(m)) {}

void FieldVisualizer::draw() {
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

void FieldVisualizer::drawBox() {
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glutWireCube(range * 2.0f);
    glPopMatrix();
}
