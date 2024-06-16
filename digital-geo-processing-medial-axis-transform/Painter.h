#pragma once

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoIndexedLineSet.h> // Use SoIndexedLineSet

#include "Mesh.h"

class Painter {
public:
    SoSeparator* getShapeSep(Mesh* mesh);
    SoSeparator* getSampledPointsSep(const std::vector<Vertex*>& sampledPoints);
    SoSeparator* getMaximalBallsSep(const std::vector<Vertex*>& centers, const std::vector<float>& radii);
    SoSeparator* getMedialAxisLinesSep(const std::vector<Vertex*>& centers); // Add this method
};
