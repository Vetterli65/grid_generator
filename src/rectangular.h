#ifndef RECTANGULAR_H
#define RECTANGULAR_H

#include "geometry.h"

# define M_PI           3.14159265358979323846

using std::cout, std::endl, std::pair, std::vector, std::pow, std::max;

struct Rectangle : Polygon {
	Rectangle() : Polygon(4, -1) {}

};

class Rectangular : public Geometry {
public:
	Rectangular(int iMax, int jMax) : iMax(iMax), jMax(jMax), N(iMax * jMax) {
		createPoints();
		clusterPoints();
		transformXY(0.f, 5.f);
		gaussSeibel(15000);
		createRectangles();
	};

	vector<Node> getPoints() { return points; }
	vector<Rectangle> getRectangles() { return rectangles; }

	void gaussSeibel(int maxIterations);
	void createRectangles();
	void printRectangles();

	void writePointsTecplot(const std::string& filename);

private:
	int iMax, jMax;
	int N;

	vector<Node> points;
	vector<Rectangle> rectangles;

	void createPoints();
	void clusterPoints();
	void transformXY(float x_W, float x_E);

	float y_S(float x);
	float y_N(float x);

	void enforceBCs(float dXi, float dEta);
    Node dx(int i, float dXi, float dEta);
	Node dy(int i, float dXi, float dEta);
	float alpha(int i, float dXi, float dEta);
	float beta(int i, float dXi, float dEta);
	float gamma(int i, float dXi, float dEta);
	vector<float> getCoeff(int i, float dXi, float dEta);
	void enforceICs(float dXi, float dEta, float& maxChange);

};

#endif // RECTANGULAR_H
