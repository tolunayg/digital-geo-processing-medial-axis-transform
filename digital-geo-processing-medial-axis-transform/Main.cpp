#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include "Mesh.h"
#include "Painter.h"
#include "MedialAxisTransformer.h"

int main(int, char** argv)
{
    HWND window = SoWin::init(argv[0]);

    SoWinExaminerViewer* viewer = new SoWinExaminerViewer(window);

    SoSeparator* root = new SoSeparator;
    root->ref();

    // Load and draw the mesh
    Mesh* mesh = new Mesh();
    Painter* painter = new Painter();
    char filename[] = "0.off";
    mesh->loadOff(filename);
    root->addChild(painter->getShapeSep(mesh));

    // Initialize the Medial Axis Transformer and apply transformations
    MedialAxisTransformer transformer(mesh);
    root->addChild(transformer.transform(painter));

    viewer->setSize(SbVec2s(640, 480));
    viewer->setSceneGraph(root);
    viewer->show();

    SoWin::show(window);
    SoWin::mainLoop();

    delete viewer;
    root->unref();
    return 0;
}
