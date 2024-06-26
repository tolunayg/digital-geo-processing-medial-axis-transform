#include "MedialAxisTransformer.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

/**
 * Constructor for MedialAxisTransformer.
 * Initializes the mesh and sets the random seed.
 * @param mesh Pointer to the input mesh.
 */
MedialAxisTransformer::MedialAxisTransformer(Mesh* mesh) : mesh(mesh) {
    // Initialize random seed
    std::srand(static_cast<unsigned>(std::time(0)));
}

/**
 * Calculates the area of a triangle given its three vertices.
 * @param v1 Pointer to the first vertex.
 * @param v2 Pointer to the second vertex.
 * @param v3 Pointer to the third vertex.
 * @return The area of the triangle.
 */
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

/**
 * Samples points on the surface of the mesh.
 * Points are sampled based on the area of each triangle in the mesh.
 * @return A vector of sampled vertices.
 */
std::vector<Vertex*> MedialAxisTransformer::samplePoints() {
    std::vector<Vertex*> sampledPoints;
    // Calculate the total surface area of the mesh
    float totalArea = 0.0f;
    for (int i = 0; i < mesh->tris.size(); ++i) {
        Triangle* tri = mesh->tris[i];
        totalArea += calculateTriangleArea(mesh->verts[tri->v1i], mesh->verts[tri->v2i], mesh->verts[tri->v3i]);
    }

    // Number of points to sample
    int numSamples = mesh->verts.size() / 4;


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

/**
 * Checks if a point is inside the mesh using ray casting.
 * @param point Pointer to the point vertex.
 * @param mesh Pointer to the mesh.
 * @return True if the point is inside the mesh, false otherwise.
 */
bool MedialAxisTransformer::isPointInsideMesh(Vertex* point, Mesh* mesh) {
    int intersections = 0;
    for (size_t i = 0; i < mesh->tris.size(); ++i) {
        Triangle* tri = mesh->tris[i];
        Vertex* v1 = mesh->verts[tri->v1i];
        Vertex* v2 = mesh->verts[tri->v2i];
        Vertex* v3 = mesh->verts[tri->v3i];

        // Ray casting along the x-axis
        if (rayIntersectsTriangle(point->coords, v1->coords, v2->coords, v3->coords)) {
            intersections++;
        }
    }
    return (intersections % 2) == 1; // Point is inside if intersections count is odd
}

/**
 * Performs ray-triangle intersection test.
 * @param orig Origin point of the ray.
 * @param v0 Vertex 0 of the triangle.
 * @param v1 Vertex 1 of the triangle.
 * @param v2 Vertex 2 of the triangle.
 * @return True if the ray intersects the triangle, false otherwise.
 */
bool MedialAxisTransformer::rayIntersectsTriangle(const float* orig, const float* v0, const float* v1, const float* v2) {
    const float EPSILON = 0.0000001f;
    float edge1[3], edge2[3], h[3], s[3], q[3];
    float a, f, u, v;
    for (int i = 0; i < 3; ++i) {
        edge1[i] = v1[i] - v0[i];
        edge2[i] = v2[i] - v0[i];
    }
    h[0] = orig[1] * edge2[2] - orig[2] * edge2[1];
    h[1] = orig[2] * edge2[0] - orig[0] * edge2[2];
    h[2] = orig[0] * edge2[1] - orig[1] * edge2[0];
    a = edge1[0] * h[0] + edge1[1] * h[1] + edge1[2] * h[2];
    if (a > -EPSILON && a < EPSILON) {
        return false; // Ray is parallel to the triangle
    }
    f = 1.0f / a;
    for (int i = 0; i < 3; ++i) {
        s[i] = orig[i] - v0[i];
    }
    u = f * (s[0] * h[0] + s[1] * h[1] + s[2] * h[2]);
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    q[0] = s[1] * edge1[2] - s[2] * edge1[1];
    q[1] = s[2] * edge1[0] - s[0] * edge1[2];
    q[2] = s[0] * edge1[1] - s[1] * edge1[0];
    v = f * (orig[0] * q[0] + orig[1] * q[1] + orig[2] * q[2]);
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    float t = f * (edge2[0] * q[0] + edge2[1] * q[1] + edge2[2] * q[2]);
    return t > EPSILON; // Intersection with the triangle
}

/**
 * Computes the intersection points by moving the sampled points inward.
 * @param sampledPoints Vector of sampled points.
 * @return A vector of intersection vertices.
 */
std::vector<Vertex*> MedialAxisTransformer::computeIntersectionPoints(const std::vector<Vertex*>& sampledPoints) {
    std::vector<Vertex*> intersectionPoints;
    // Move points along the inward normal (z-axis in this case)
    for (size_t i = 0; i < sampledPoints.size(); ++i) {
        float* coords = new float[3];
        coords[0] = sampledPoints[i]->coords[0];
        coords[1] = sampledPoints[i]->coords[1];
        coords[2] = sampledPoints[i]->coords[2] - 0.05; // Move inward along z-axis
        Vertex* intersectionPoint = new Vertex(sampledPoints[i]->idx, coords);
        intersectionPoints.push_back(intersectionPoint);
    }
    return intersectionPoints;
}

/**
 * Performs binary search to find the maximal ball.
 * @param p Pointer to the start vertex.
 * @param q Pointer to the end vertex.
 * @param mesh Pointer to the mesh.
 * @return Pointer to the center vertex of the maximal ball.
 */
Vertex* MedialAxisTransformer::binarySearchMaximalBall(Vertex* p, Vertex* q, Mesh* mesh) {
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

/**
 * Computes the maximal balls using binary search.
 * @param intersectionPoints Vector of intersection points.
 * @param radii Vector to store the radii of the maximal balls.
 * @return A vector of vertices representing the centers of the maximal balls.
 */
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
        radii.push_back(radius * 0.5); // Reduce the radius to fit within the mesh
    }
    return maximalBalls;
}

/**
 * Transforms the mesh and prepares the visual elements.
 * @param painter Pointer to the Painter object for rendering.
 * @return A separator node containing the visual elements.
 */
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