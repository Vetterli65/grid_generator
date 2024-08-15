#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <string>

using std::vector, std::cout, std::endl, std::setw, std::max, std::min;

struct Node {
    float x, y;

    Node operator+(const Node& other) const {
        return { x + other.x, y + other.y };
    }

    Node operator-(const Node& other) const {
        return { x - other.x, y - other.y };
    }

    float dot(const Node& other) const {
        return (x * other.x) + (y * other.y);
    }

    float cross(const Node& other) const {
        return (x * other.y) - (y * other.x);
    }
};

struct Polygon {
    vector<int> vertices;
    vector<int> edges;

    Polygon(int n, int val){
        vertices.resize(n);
        edges.resize(n, val);
    }

};

class Geometry {
public:	
	virtual ~Geometry() = default;
};

#endif // GEOMETRY_H
