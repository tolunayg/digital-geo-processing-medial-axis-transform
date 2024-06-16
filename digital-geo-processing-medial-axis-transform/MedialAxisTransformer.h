#pragma once

#include "Mesh.h"
#include "Painter.h"
#include <vector>

/**
 * Class for performing Medial Axis Transform on a 3D mesh.
 */
class MedialAxisTransformer {
public:
    /**
     * Constructor for MedialAxisTransformer.
     * @param mesh Pointer to the input mesh.
     */
    MedialAxisTransformer(Mesh* mesh);

    /**
     * Samples points on the surface of the mesh.
     * @return A vector of sampled vertices.
     */
    std::vector<Vertex*> samplePoints();

    /**
     * Computes the intersection points by moving the sampled points inward.
     * @param sampledPoints Vector of sampled points.
     * @return A vector of intersection vertices.
     */
    std::vector<Vertex*> computeIntersectionPoints(const std::vector<Vertex*>& sampledPoints);

    /**
     * Computes the maximal balls using binary search.
     * @param intersectionPoints Vector of intersection points.
     * @param radii Vector to store the radii of the maximal balls.
     * @return A vector of vertices representing the centers of the maximal balls.
     */
    std::vector<Vertex*> computeMaximalBalls(const std::vector<Vertex*>& intersectionPoints, std::vector<float>& radii);

    /**
     * Transforms the mesh and prepares the visual elements.
     * @param painter Pointer to the Painter object for rendering.
     * @return A separator node containing the visual elements.
     */
    SoSeparator* transform(Painter* painter);

private:
    Mesh* mesh; ///< Pointer to the input mesh.

    /**
     * Checks if a point is inside the mesh using ray casting.
     * @param point Pointer to the point vertex.
     * @param mesh Pointer to the mesh.
     * @return True if the point is inside the mesh, false otherwise.
     */
    bool isPointInsideMesh(Vertex* point, Mesh* mesh);

    /**
     * Performs ray-triangle intersection test.
     * @param orig Origin point of the ray.
     * @param v0 Vertex 0 of the triangle.
     * @param v1 Vertex 1 of the triangle.
     * @param v2 Vertex 2 of the triangle.
     * @return True if the ray intersects the triangle, false otherwise.
     */
    bool rayIntersectsTriangle(const float* orig, const float* v0, const float* v1, const float* v2);

    /**
     * Performs binary search to find the maximal ball.
     * @param p Pointer to the start vertex.
     * @param q Pointer to the end vertex.
     * @param mesh Pointer to the mesh.
     * @return Pointer to the center vertex of the maximal ball.
     */
    Vertex* binarySearchMaximalBall(Vertex* p, Vertex* q, Mesh* mesh);
};