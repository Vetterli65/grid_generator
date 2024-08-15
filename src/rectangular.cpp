#include "rectangular.h"

void Rectangular::createPoints() {
	for (int i = 0; i < N; i++) {
		points.push_back({ static_cast<float>(i % iMax), static_cast<float>(i / iMax) });
	}
}

void Rectangular::clusterPoints() {
	for (int i = 0; i < N; i++) {
		float xi = points[i].x / (iMax - 1);
		float eta = points[i].y / (jMax - 1);
		points[i] = { xi, eta };
	}
}

float Rectangular::y_S(float x) {
	if (x >= 2.0 && x <= 3.0) {
		return float(0.1 * sin((x - 2.0) * M_PI));
	}
	else {
		return 0.f;
	}
}

float Rectangular::y_N(float x) {
	if (x >= 2.0 && x <= 3.0) {
		return float(1 - 0.1 * sin((x - 2) * M_PI));
	}
	else {
		return 1.f;
	}
}

void Rectangular::transformXY(float x_W, float x_E) {
	for (int i = 0; i < points.size(); i++) {
		float x = x_W + points[i].x * (x_E - x_W);
		float y = y_S(x) + points[i].y * (y_N(x) - y_S(x));
		points[i] = { x,y };
	}
}

void Rectangular::writePointsTecplot(const std::string& filename) {
	std::ofstream outFile(filename);
	if (!outFile) {
		throw std::runtime_error("Failed to open file: " + filename);
	}

	outFile << "TITLE = \"2D Mesh Data\"\n";
	outFile << "VARIABLES = \"X\" \"Y\"\n";
	outFile << "ZONE T=\"2D Mesh\", I=" << iMax << ", J=" << jMax << ", DATAPACKING=POINT\n";

	for (const auto point : points) {
		outFile << point.x << " " << point.y << "\n";
	}

	outFile.close();
}

void Rectangular::enforceBCs(float dXi, float dEta) {
	for (int i = 0; i < N; i++) {
		if (i / iMax == 0 || i / iMax == jMax - 1) {
			if (points[i].x < 2.0 || points[i].x > 3.0) {
				if (i + iMax < N) {
					points[i].x = points[i + iMax].x;
				}
				else {
					points[i].x = points[i - iMax].x;
				}
			}

			if ((points[i].x < 3.0 && points[i].x > 2.0) || points[i].x == 2.0 || points[i].x == 3.0) {
				if (i + 2 * iMax < N) {
					points[i].x = float(points[i + 2 * iMax].x + 2 * (0.1 * M_PI * cos((points[i].x - 2) * M_PI)) * dEta);
					points[i].y = y_S(points[i].x);
				}
				else {
					points[i].x = float(points[i - 2 * iMax].x + 2 * (0.1 * M_PI * cos((points[i].x - 2) * M_PI)) * dEta);
					points[i].y = y_N(points[i].x);
				}
			}
		}
		if (i % iMax == 0 || i % iMax == iMax - 1) {
			if (i % jMax != 0 && i % jMax != jMax - 1) {
				if (i % iMax == 0) {
					points[i].y = points[i + 1].y;
				}
				else if (i % iMax == iMax - 1) {
					points[i].y = points[i - 1].y;
				}
			}
		}
	}

	int BLE = 0, BTE = 0, TLE = 0, TTE = 0;
	for (int i = 0; i < N; i++) {
		int ir = N - i - 1;
		if (i / iMax == 0 && i % iMax != 0 && i % iMax != iMax - 1) {
			if (2 - points[i].x > 0) BLE = i;
			if (3 - points[ir].x < 0) BTE = ir;
		}
		if (i / iMax == jMax - 1 && i % iMax != 0 && i % iMax != iMax - 1) {
			if (2 - points[i].x > 0) TLE = i;
			if (3 - points[ir].x < 0) TTE = ir;
		}
	}
	points[BLE].x = 2;
	points[BLE].y = 0;

	points[BTE].x = 3;
	points[BTE].y = 1;

	points[TLE].x = 2;
	points[TLE].y = 1;

	points[TTE].x = 3;
	points[TTE].y = 0;
}

Node Rectangular::dx(int i, float dXi, float dEta) {
	float newdXi = (points[i + 1].x - points[i - 1].x)/(2 * dXi);
	float newdEta = (points[i + iMax].x - points[i - iMax].x) / (2 * dEta);

	return { newdXi, newdEta };
}

Node Rectangular::dy(int i, float dXi, float dEta) {
	float newdXi = (points[i + 1].y - points[i - 1].y) / (2 * dXi);
	float newdEta = (points[i + iMax].y - points[i - iMax].y) / (2 * dEta);

	return { newdXi, newdEta };
}

float Rectangular::alpha(int i, float dXi, float dEta) {
	Node dX = dx(i, dXi, dEta);
	Node dY = dy(i, dXi, dEta);

	return float(pow(dX.y, 2) + pow(dY.y, 2));
}

float Rectangular::beta(int i, float dXi, float dEta) {
	Node dX = dx(i, dXi, dEta);
	Node dY = dy(i, dXi, dEta);
	return -(dX.x * dX.y + dY.x * dY.y);
}

float Rectangular::gamma(int i, float dXi, float dEta) {
	Node dX = dx(i, dXi, dEta);
	Node dY = dy(i, dXi, dEta);
	return float(pow(dX.x, 2) + pow(dY.y, 2));
}

vector<float> Rectangular::getCoeff(int i, float dXi, float dEta) {
	float a = alpha(i, dXi, dEta);
	float b = beta(i, dXi, dEta);
	float g = gamma(i, dXi, dEta);

	float mult = 1 / (2 * (a / (dXi * dXi)) + 2 * (g / (dEta * dEta)));
	float a1 = -(mult * b) / (2 * dXi * dEta);
	float a2 = (mult * g) / (dEta * dEta);
	float a3 = (mult * b) / (2 * dXi * dEta);
	float a4 = (mult * a) / (dXi * dXi);
	float a5 = a4;
	float a6 = a3;
	float a7 = a2;
	float a8 = a1;
	return { a1, a2, a3, a4, a5, a6, a7, a8 };
}

void Rectangular::enforceICs(float dXi, float dEta, float& maxChange) {
	for (int i = 0; i < N; i++) {
		if ((i % iMax != 0 && i % iMax != iMax - 1) && (i / iMax != 0 && i / iMax != jMax - 1)) {
			vector<float> coeff = getCoeff(i, dXi, dEta);

			float newX = coeff[0] * points[i - 1 - iMax].x
						+ coeff[1] * points[i - iMax].x
						+ coeff[2] * points[i + 1 - iMax].x
						+ coeff[3] * points[i - 1].x
						+ coeff[4] * points[i + 1].x
						+ coeff[5] * points[i - 1 + iMax].x
						+ coeff[6] * points[i + iMax].x
						+ coeff[7] * points[i + 1 + iMax].x;

			float newY = coeff[0] * points[i - 1 - iMax].y
						+ coeff[1] * points[i - iMax].y
						+ coeff[2] * points[i + 1 - iMax].y
						+ coeff[3] * points[i - 1].y
						+ coeff[4] * points[i + 1].y
						+ coeff[5] * points[i - 1 + iMax].y
						+ coeff[6] * points[i + iMax].y
						+ coeff[7] * points[i + 1 + iMax].y;

			maxChange = max(maxChange, (float) max(fabs(newX - points[i].x), fabs(newY - points[i].y)));

			points[i] = { newX, newY };
		}
	}
}

void Rectangular::gaussSeibel(int maxIterations) {
	bool converged = false;
	int iteration = 0;

	float dXi = 1.f / float(iMax - 1);
	float dEta = 1.f / float(jMax - 1);

	while (!converged) {
		float maxChange = 0.0;

		enforceBCs(dXi, dEta);
		enforceICs(dXi, dEta, maxChange);

		converged = maxChange < 9e-7 || iteration > maxIterations;
		iteration++;
		if (iteration % 5 == 0) {
			cout << "Iteration: " << iteration << ", Max Change: " << maxChange << endl;
			// writePointsTecplot("OG/Orthogonal_Grid_" + std::to_string(iteration) + ".dat");
		}
	}
	writePointsTecplot("Orthogonal_Grid.dat");
}

void Rectangular::createRectangles() {
	for (int i = 0; i < jMax - 1; i++) {
		for (int j = 0; j < iMax - 1; j++) {
			Rectangle newRect;
			newRect.vertices = { i * iMax + j, (i + 1) * iMax + j, (i + 1) * iMax + (j + 1), i * iMax + (j + 1) };
			rectangles.push_back(newRect);
		}
	}

	for (int i = 0; i < rectangles.size(); i++) {
		rectangles[i].edges = { i + 1, i + 1 + (iMax - 1) , i - 1, i - 1 - (iMax - 1) };

		if (i % (iMax - 1) == iMax - 2) rectangles[i].edges[0] = -1;
		if (i / (iMax - 1) == jMax - 2) rectangles[i].edges[1] = -1;
		if (i % (iMax - 1) == 0) rectangles[i].edges[2] = -1;
		if (i / (iMax - 1) == 0) rectangles[i].edges[3] = -1;
	}
}

void Rectangular::printRectangles() {
	int width = static_cast<int>(log10(max(rectangles.size() * 4, points.size())) + 1);

	cout << "Rectangle Vertices: " << endl;
	for (int i = 0; i < rectangles.size(); i++) {
		cout << setw(width) << i << " | "
			<< setw(width) << rectangles[i].vertices[0] << " "
			<< setw(width) << rectangles[i].vertices[1] << " "
			<< setw(width) << rectangles[i].vertices[2] << " "
			<< setw(width) << rectangles[i].vertices[3] << endl;
	}
	cout << endl;

	cout << "Rectangle Adjacencies: " << endl;
	for (int i = 0; i < rectangles.size(); i++) {
		cout << setw(width) << i << " | "
			<< setw(width) << rectangles[i].edges[0] << " "
			<< setw(width) << rectangles[i].edges[1] << " "
			<< setw(width) << rectangles[i].edges[2] << " "
			<< setw(width) << rectangles[i].edges[3] << endl;
	}
	cout << endl;

	cout << "Points: " << endl;
	for (int i = 0; i < points.size(); i++) {
		cout << setw(width) << i << " | "
			<< setw(8) << points[i].x << " "
			<< setw(8) << points[i].y << endl;
	}
	cout << endl;
}