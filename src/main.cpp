#include "geometry.h"
#include "triangular.h"
#include "rectangular.h"
#include "renderer.h"
#include <random>

int main() {
    
    /*

    // ### RANDOM POINTS DELAUNAY TRIAGULATION SETUP ###

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 5);

    vector<Node> points;

    for (int i = 0; i < 6; i++) points.push_back({ float(dis(gen)), float(dis(gen)) });

    Triangular triGrid = Triangular(points);
    triGrid.delaunayTriangulate();

    Renderer renderer = Renderer(DrawMode::Triangles);
    renderer.setScaleInterval(1);
    renderer.setUnitsPerPixel(0.01f);

    triGrid.printInformation();
    renderer.setPoints(triGrid.getPoints());
    renderer.setTriangles(triGrid.getTriangles());

    renderer.run();
    */

    // ### RECTANGULAR GRID EXAMPLE ###

    Rectangular rectGrid = Rectangular(26, 6);

    Renderer renderer = Renderer(DrawMode::Rectangles);
    renderer.setScaleInterval(1);
    renderer.setUnitsPerPixel(0.01f);

    renderer.setPoints(rectGrid.getPoints());
    renderer.setRectangles(rectGrid.getRectangles());

    renderer.run();
    
    return 0;
}
