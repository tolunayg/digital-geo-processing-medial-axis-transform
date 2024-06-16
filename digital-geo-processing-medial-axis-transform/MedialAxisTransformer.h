#pragma once

#include "Mesh.h"
#include "Painter.h"
#include <vector>

class MedialAxisTransformer {
public:
    MedialAxisTransformer(Mesh* mesh);
    std::vector<Vertex*> samplePoints();
    std::vector<Vertex*> computeIntersectionPoints(const std::vector<Vertex*>& sampledPoints);
    std::vector<Vertex*> computeMaximalBalls(const std::vector<Vertex*>& intersectionPoints, std::vector<float>& radii);
    SoSeparator* transform(Painter* painter);

private:
    Mesh* mesh;
};

Vertex* binarySearchMaximalBall(Vertex* p, Vertex* q, Mesh* mesh);
