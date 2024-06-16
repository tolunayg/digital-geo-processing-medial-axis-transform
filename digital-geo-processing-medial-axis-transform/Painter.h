#pragma once

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

#include "Mesh.h"

/**
 * Class for rendering different elements of the mesh and medial axis.
 */
class Painter {
public:
    /**
     * Returns a separator node for the mesh shape.
     * @param mesh Pointer to the mesh object.
     * @return A separator node containing the mesh shape.
     */
    SoSeparator* getShapeSep(Mesh* mesh);

    /**
     * Returns a separator node for the sampled points.
     * @param sampledPoints Vector of sampled vertices.
     * @return A separator node containing the sampled points.
     */
    SoSeparator* getSampledPointsSep(const std::vector<Vertex*>& sampledPoints);

    /**
     * Returns a separator node for the maximal balls.
     * @param centers Vector of vertices representing the centers of the maximal balls.
     * @param radii Vector of radii of the maximal balls.
     * @return A separator node containing the maximal balls.
     */
    SoSeparator* getMaximalBallsSep(const std::vector<Vertex*>& centers, const std::vector<float>& radii);

    /**
     * Returns a separator node for the medial axis lines.
     * @param centers Vector of vertices representing the centers of the maximal balls.
     * @return A separator node containing the medial axis lines.
     */
    SoSeparator* getMedialAxisLinesSep(const std::vector<Vertex*>& centers);
};