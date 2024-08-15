#ifndef TRIANGULAR_H
#define TRIANGULAR_H

#include "geometry.h"

struct Triangle : Polygon {
    Triangle() : Polygon(3, -1) {}
};

class Triangular : public Geometry {
public:

    Triangular() : points({}) {};
    Triangular(vector<Node> points) : points(points) {};
    Triangular(std::string filename) {
        readTecplotFile(filename);
    }

    void delaunayTriangulate();
    void printInformation();

    vector<Node> getPoints() { return points; }
    vector<Triangle> getTriangles() { return triangles; }

    void writeTecplotFile(const std::string& filename);
    void readTecplotFile(const std::string& filename);

private:
    vector<Node> points;
    vector<Triangle> triangles;

    // prep points 
    void normalizePoints(float& xmin, float& ymin, float& dmax);
    void denormalizePoints(float& xmin, float& ymin, float& dmax);
    void binSort();

    // delaunay helper functions
    int edge(int L, int K);
    bool delanayCondition(Node P, Node v1, Node v2, Node v3);
    bool pointWithinTriangle(float S1, float S2, float S3);
    int searchForTriangle(int i, int j);
    void createNewTriangles(int ntri, int T, int i, int& triOnStack, vector<int>& triStack);
    void emptyStack(int& tos, int i, vector<int>& triStack);
    void removeSuperTriangle(int& ntri);

};

#endif // TRIANGULAR_H