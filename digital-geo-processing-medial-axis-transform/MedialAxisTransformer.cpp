#include "MedialAxisTransformer.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

MedialAxisTransformer::MedialAxisTransformer(Mesh* mesh) : mesh(mesh) {
    // Initialize random seed
    std::srand(static_cast<unsigned>(std::time(0)));
}

float calculateTriangleArea(Vertex* v1, Vertex* v2, Vertex* v3) {
    float a[3] = { v2->coords[0] - v1->coords[0], v2->coords[1] - v1->coords[1], v2->coords[2] - v1->coords[2] };
    float b[3] = { v3->coords[0] - v1->coords[0], v3->coords[1] - v1->coords[1], v3->coords[2] - v1->coords[2] };
    float cross[3] = {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
    return 0.5f * std::sqrt(cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2]);
}

std::vector<Vertex*> MedialAxisTransformer::samplePoints() {
    std::vector<Vertex*> sampledPoints;
    // Calculate the total surface area of the mesh
    float totalArea = 0.0f;
    for (int i = 0; i < mesh->tris.size(); ++i) {
        Triangle* tri = mesh->tris[i];
        totalArea += calculateTriangleArea(mesh->verts[tri->v1i], mesh->verts[tri->v2i], mesh->verts[tri->v3i]);
    }

    // Number of points to sample
    int numSamples = mesh->verts.size() / 10;

    // Sample points based on triangle areas
    for (int i = 0; i < numSamples; ++i) {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * totalArea;
        float accumulatedArea = 0.0f;
        Triangle* selectedTriangle = nullptr;
        for (int j = 0; j < mesh->tris.size(); ++j) {
            Triangle* tri = mesh->tris[j];
            accumulatedArea += calculateTriangleArea(mesh->verts[tri->v1i], mesh->verts[tri->v2i], mesh->verts[tri->v3i]);
            if (accumulatedArea >= r) {
                selectedTriangle = tri;
                break;
            }
        }

        if (selectedTriangle != nullptr) {
            // Barycentric coordinates to sample a point inside the selected triangle
            float u = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            float v = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            if (u + v > 1.0f) {
                u = 1.0f - u;
                v = 1.0f - v;
            }
            float w = 1.0f - u - v;
            float sampledPointCoords[3] = {
                u * mesh->verts[selectedTriangle->v1i]->coords[0] +
                v * mesh->verts[selectedTriangle->v2i]->coords[0] +
                w * mesh->verts[selectedTriangle->v3i]->coords[0],

                u * mesh->verts[selectedTriangle->v1i]->coords[1] +
                v * mesh->verts[selectedTriangle->v2i]->coords[1] +
                w * mesh->verts[selectedTriangle->v3i]->coords[1],

                u * mesh->verts[selectedTriangle->v1i]->coords[2] +
                v * mesh->verts[selectedTriangle->v2i]->coords[2] +
                w * mesh->verts[selectedTriangle->v3i]->coords[2]
            };

            int idx = mesh->verts.size() + sampledPoints.size();
            float* coords = new float[3];
            coords[0] = sampledPointCoords[0];
            coords[1] = sampledPointCoords[1];
            coords[2] = sampledPointCoords[2];
            Vertex* sampledVertex = new Vertex(idx, coords);
            sampledPoints.push_back(sampledVertex);
        }
    }
    return sampledPoints;
}

bool isPointInsideMesh(Vertex* point, Mesh* mesh) {
    // Dummy implementation: always return true for now
    // We'll replace this with proper intersection tests later
    return true;
}

std::vector<Vertex*> MedialAxisTransformer::computeIntersectionPoints(const std::vector<Vertex*>& sampledPoints) {
    std::vector<Vertex*> intersectionPoints;
    // Dummy implementation: simply move the points along the z-axis inward normal
    for (size_t i = 0; i < sampledPoints.size(); ++i) {
        float* coords = new float[3];
        coords[0] = sampledPoints[i]->coords[0];
        coords[1] = sampledPoints[i]->coords[1];
        coords[2] = sampledPoints[i]->coords[2] - 0.05f; // Move inward along z-axis
        Vertex* intersectionPoint = new Vertex(sampledPoints[i]->idx, coords);
        intersectionPoints.push_back(intersectionPoint);
    }
    return intersectionPoints;
}

Vertex* binarySearchMaximalBall(Vertex* p, Vertex* q, Mesh* mesh) {
    Vertex* mid = nullptr;
    float* coords = new float[3];
    while (true) {
        coords[0] = (p->coords[0] + q->coords[0]) / 2.0f;
        coords[1] = (p->coords[1] + q->coords[1]) / 2.0f;
        coords[2] = (p->coords[2] + q->coords[2]) / 2.0f;
        mid = new Vertex(-1, coords); // -1 is temporary index, not used
        if (isPointInsideMesh(mid, mesh)) {
            p = mid;
        }
        else {
            q = mid;
        }
        // Check for convergence
        if (std::sqrt(
            (p->coords[0] - q->coords[0]) * (p->coords[0] - q->coords[0]) +
            (p->coords[1] - q->coords[1]) * (p->coords[1] - q->coords[1]) +
            (p->coords[2] - q->coords[2]) * (p->coords[2] - q->coords[2])
        ) < 0.001f) { // Convergence threshold
            break;
        }
    }
    return p;
}

std::vector<Vertex*> MedialAxisTransformer::computeMaximalBalls(const std::vector<Vertex*>& intersectionPoints, std::vector<float>& radii) {
    std::vector<Vertex*> maximalBalls;
    for (size_t i = 0; i < intersectionPoints.size(); ++i) {
        Vertex* p = intersectionPoints[i];
        float* coords = new float[3];
        coords[0] = p->coords[0];
        coords[1] = p->coords[1];
        coords[2] = p->coords[2] - 1.0f; // Move far inward
        Vertex* q = new Vertex(p->idx, coords);
        Vertex* center = binarySearchMaximalBall(p, q, mesh);
        maximalBalls.push_back(center);
        float radius = std::sqrt(
            (p->coords[0] - center->coords[0]) * (p->coords[0] - center->coords[0]) +
            (p->coords[1] - center->coords[1]) * (p->coords[1] - center->coords[1]) +
            (p->coords[2] - center->coords[2]) * (p->coords[2] - center->coords[2])
        );
        radii.push_back(radius);
    }
    return maximalBalls;
}

SoSeparator* MedialAxisTransformer::transform(Painter* painter) {
    SoSeparator* res = new SoSeparator;

    // Step 1: Sample points on the mesh
    std::vector<Vertex*> sampledPoints = samplePoints();
    res->addChild(painter->getSampledPointsSep(sampledPoints));

    // Step 2: Compute intersection points
    std::vector<Vertex*> intersectionPoints = computeIntersectionPoints(sampledPoints);
    res->addChild(painter->getSampledPointsSep(intersectionPoints));

    // Step 3: Compute maximal balls
    std::vector<float> radii;
    std::vector<Vertex*> maximalBalls = computeMaximalBalls(intersectionPoints, radii);
    //res->addChild(painter->getMaximalBallsSep(maximalBalls, radii));

    // Step 4: Visualize the medial axis
    res->addChild(painter->getMedialAxisLinesSep(maximalBalls));

    return res;
}
