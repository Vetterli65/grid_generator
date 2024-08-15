#include "triangular.h"

void Triangular::normalizePoints(float& xmin, float& ymin, float& dmax) {
    int numPoints = points.size();
    // Find min and max boundaries of point cloud
    float xmax = 0, ymax = 0;
    for (int i = 0; i < numPoints; i++) {
        xmax = max(xmax, points[i].x);
        xmin = min(xmin, points[i].x);
        ymax = max(ymax, points[i].y);
        ymin = min(ymin, points[i].y);
    }

    // Remap everything (preserving the aspect ratio) to between (0,0) - (1,1)
    dmax = max(xmax - xmin, ymax - ymin); // dmax = largest dimension
    for (int i = 0; i < numPoints; i++) {
        points[i].x = (points[i].x - xmin) / dmax;
        points[i].y = (points[i].y - ymin) / dmax;
    }
}

void Triangular::denormalizePoints(float& xmin, float& ymin, float& dmax) {
    int numPoints = points.size();
    for (int i = 0; i < numPoints; i++) {
        points[i].x = points[i].x * dmax + xmin;
        points[i].y = points[i].y * dmax + ymin;
    }
}

void Triangular::binSort() {
    // Sort points by proximity
    int NbinRows = static_cast<int>(std::ceil(std::pow(points.size(), 0.25)));
    vector<int> bins(points.size());
    for (int i = 0; i < points.size(); i++) {
        int p = static_cast<int>(points[i].y * NbinRows * 0.999); // bin row
        int q = static_cast<int>(points[i].x * NbinRows * 0.999); // bin column
        if (p % 2)
            bins[i] = (p + 1) * NbinRows - q;
        else
            bins[i] = p * NbinRows + q + 1;
    }

    // Insertion sort
    for (int i = 1; i < points.size(); i++) {
        int key = bins[i];
        Node tempF = points[i];
        int j = i - 1;
        while (j >= 0 && (bins[j] > key)) {
            bins[j + 1] = bins[j];
            points[j + 1] = points[j];
            j--;
        }
        bins[j + 1] = key;
        points[j + 1] = tempF;
    }
}

int Triangular::edge(int L, int K) {
    for (int m = 0; m < 3; m++){
        if (triangles[L].edges[m] == K) {
            return m;
        }
    }
    return -1;
}

bool Triangular::delanayCondition(Node P, Node v1, Node v2, Node v3) {
    float cosa = (v1 - v3).dot(v2 - v3);
    float cosb = (v2 - P).dot(v1 - P);
    if (cosa >= 0.0 && cosb >= 0.0) return false;
    else if (cosa < 0.0 && cosb < 0.0) return true;
    else {
        float sina = (v1 - v3).cross(v2 - v3);;
        float sinb = (v2 - P).cross(v1 - P);
        if (sina * cosb + sinb * cosa < 0.0) return true;
        else return false;
    }
}

bool Triangular::pointWithinTriangle(float S1, float S2, float S3) {
    if ((S1 < 0 && S2 < 0 && S3 < 0) ||
        (S1 < 1E-10 && S2 < 0 && S3 < 0) ||
        (S2 < 1E-10 && S1 < 0 && S3 < 0) ||
        (S3 < 1E-10 && S1 < 0 && S2 < 0)) { // inside triangle
        return true;
    }
    else {
        return false;
    }
}

int Triangular::searchForTriangle(int i, int j) {
    float S1, S2, S3;
    do {
        // Vertices of a given triangle
        int V1 = triangles[j].vertices[0];
        int V2 = triangles[j].vertices[1];
        int V3 = triangles[j].vertices[2];

        // Vectors of the edges
        Node AB = points[V2] - points[V1];
        Node BC = points[V3] - points[V2];
        Node CA = points[V1] - points[V3];

        Node AP = points[i] - points[V1];
        Node BP = points[i] - points[V2];
        Node CP = points[i] - points[V3];

        // Normals of the edges
        Node N1 = { AB.y, -AB.x };
        Node N2 = { BC.y, -BC.x };
        Node N3 = { CA.y, -CA.x };

        S1 = AP.dot(N1);
        S2 = BP.dot(N2);
        S3 = CP.dot(N3);

        // Adjust j in the direction of target point ii
        if ((S1 > 0) && (S1 >= S2) && (S1 >= S3)) {
            j = triangles[j].edges[0];
        }
        else if ((S2 > 0) && (S2 >= S1) && (S2 >= S3)) {
            j = triangles[j].edges[1];
        }
        else if ((S3 > 0) && (S3 >= S1) && (S3 >= S2)) {
            j = triangles[j].edges[2];
        }
    } while (!pointWithinTriangle(S1, S2, S3));
    return j;
}

void Triangular::createNewTriangles(int ntri, int T, int i, int& triOnStack, vector<int>& triStack) {
    // Delete triangle T and replace it with three sub-triangles touching P
    triangles.resize(ntri);
    // Vertices of new triangles
    triangles[ntri - 2].vertices = { i, triangles[T].vertices[1], triangles[T].vertices[2] };
    triangles[ntri - 1].vertices = { i, triangles[T].vertices[2], triangles[T].vertices[0] };
    // Update adjacencies of triangles surrounding the old triangle
    int A = triangles[T].edges[0];
    int B = triangles[T].edges[1];
    int C = triangles[T].edges[2];
    if (A >= 0) triangles[A].edges[edge(A, T)] = T;
    if (B >= 0) triangles[B].edges[edge(B, T)] = ntri - 2;
    if (C >= 0) triangles[C].edges[edge(C, T)] = ntri - 1;
    // Adjacencies of new triangles    
    triangles[ntri - 2].edges = { T, triangles[T].edges[1], ntri - 1 };
    triangles[ntri - 1].edges = { ntri - 2, triangles[T].edges[2], T };
    // Replace v3 of containing triangle with P and rotate to v1
    triangles[T].vertices = { i, triangles[T].vertices[0], triangles[T].vertices[1] };
    // Replace 1st and 3rd adjacencies of containing triangle with new triangles
    triangles[T].edges = { ntri - 1, triangles[T].edges[0], ntri - 2 };
    // Place each triangle containing P onto a stack, if the edge opposite P has an adjacent triangle
    if (triangles[T].edges[1] >= 0) triStack[++triOnStack] = T;
    if (triangles[ntri - 2].edges[1] >= 0) triStack[++triOnStack] = ntri - 2;
    if (triangles[ntri - 1].edges[1] >= 0) triStack[++triOnStack] = ntri - 1;
}

void Triangular::emptyStack(int& tos, int i, vector<int>& triStack) {
    while (tos >= 0) { // looping thru the stack
        int L = triStack[tos--];
        Node v1 = points[triangles[L].vertices[2]];
        Node v2 = points[triangles[L].vertices[1]];
        int oppVert = -1;
        int oppVertID = -1;
        for (int k = 0; k < 3; k++) {
            if ((triangles[triangles[L].edges[1]].vertices[k] != triangles[L].vertices[1])
                && (triangles[triangles[L].edges[1]].vertices[k] != triangles[L].vertices[2])) {
                oppVert = triangles[triangles[L].edges[1]].vertices[k];
                oppVertID = k;
                break;
            }
        }
        Node v3 = points[oppVert];
        Node P = points[i];

        if (delanayCondition(P, v1, v2, v3)) {
            // Swap diagonal, and redo triangles L, R, A, C
            // Initial state:
            int R = triangles[L].edges[1];
            int C = triangles[L].edges[2];
            int A = triangles[R].edges[(oppVertID + 2) % 3];
            // Fix adjacency of A
            if (A >= 0) triangles[A].edges[edge(A, R)] = L;
            // Fix adjacency of C
            if (C >= 0) triangles[C].edges[edge(C, L)] = R;
            // Fix vertices and adjacency of R
            for (int m = 0; m < 3; m++) {
                if (triangles[R].vertices[m] == oppVert) {
                    triangles[R].vertices[(m + 2) % 3] = i;
                    break;
                }
            }
            triangles[R].edges[edge(R, L)] = C;
            triangles[R].edges[edge(R, A)] = L;
            for (int m = 0; m < 3; m++) {
                if (triangles[R].vertices[0] != i) {
                    triangles[R].vertices = { triangles[R].vertices[1], triangles[R].vertices[2], triangles[R].vertices[0] };
                    triangles[R].edges = { triangles[R].edges[1], triangles[R].edges[2], triangles[R].edges[0] };
                }
            }

            // Fix vertices and adjacency of L
            triangles[L].vertices[2] = oppVert;
            triangles[L].edges[edge(L, C)] = R;
            triangles[L].edges[edge(L, R)] = A;
            // Add L and R to stack if they have triangles opposite P;
            if (triangles[L].edges[1] >= 0) triStack[++tos] = L;
            if (triangles[R].edges[1] >= 0) triStack[++tos] = R;
        }
    }
}

void Triangular::removeSuperTriangle(int& ntri) {
    // Count how many triangles we have that don't involve supertriangle vertices
    int nT_final = ntri;
    int numPoints = points.size();
    vector<int> renumberAdj(ntri);
    vector<bool> removeEdge(ntri, false);
    for (int i = 0; i < ntri; i++)
        if ((triangles[i].vertices[0] >= numPoints - 3) || (triangles[i].vertices[1] >= numPoints - 3) || (triangles[i].vertices[2] >= numPoints - 3)) {
            removeEdge[i] = true;
            renumberAdj[i] = ntri - (nT_final--);
        }
        else {
            renumberAdj[i] = ntri - nT_final;
        }

    // Delete any triangles that contain the supertriangle vertices
    vector<Triangle> trianglesFinal(nT_final);
    int index = 0;
    for (int i = 0; i < ntri; i++)
        if ((triangles[i].vertices[0] < numPoints - 3) && (triangles[i].vertices[1] < numPoints - 3) && (triangles[i].vertices[2] < numPoints - 3)) {
            trianglesFinal[index].vertices = triangles[i].vertices;
            trianglesFinal[index].edges = {
                (1 - removeEdge[triangles[i].edges[0]]) * triangles[i].edges[0] - removeEdge[triangles[i].edges[0]],
                (1 - removeEdge[triangles[i].edges[1]]) * triangles[i].edges[1] - removeEdge[triangles[i].edges[1]],
                (1 - removeEdge[triangles[i].edges[2]]) * triangles[i].edges[2] - removeEdge[triangles[i].edges[2]]
            };
            index++;
        }
    for (int i = 0; i < nT_final; i++) {
        if (trianglesFinal[i].edges[0] >= 0) trianglesFinal[i].edges[0] -= renumberAdj[trianglesFinal[i].edges[0]];
        if (trianglesFinal[i].edges[1] >= 0) trianglesFinal[i].edges[1] -= renumberAdj[trianglesFinal[i].edges[1]];
        if (trianglesFinal[i].edges[2] >= 0) trianglesFinal[i].edges[2] -= renumberAdj[trianglesFinal[i].edges[2]];
    }

    points.erase(points.end() - 3, points.end());
    triangles = trianglesFinal;
    ntri = nT_final;
}

void Triangular::printInformation() {
    int width = static_cast<int>(log10(max(triangles.size() * 3, points.size())) + 1);

    cout << "Triangle Vertices: "<< endl;
    for (int i = 0; i < triangles.size(); i++) {
        cout << setw(width) << i << " | " 
             << setw(width) << triangles[i].vertices[0] << " "
             << setw(width) << triangles[i].vertices[1] << " "
             << setw(width) << triangles[i].vertices[2] << endl;
    }
    cout << endl;

    cout << "Triangle Adjacencies: "<< endl;
    for (int i = 0; i < triangles.size(); i++) {
        cout << setw(width) << i << " | " 
             << setw(width) << triangles[i].edges[0] << " "
             << setw(width) << triangles[i].edges[1] << " "
             << setw(width) << triangles[i].edges[2] << endl;
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

void Triangular::writeTecplotFile(const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile) {
        std::cerr << "Unable to open file";
        return;
    }

    // Write Tecplot header
    outFile << "TITLE = \"Delaunay Triangulation\"\n";
    outFile << "VARIABLES = \"X\", \"Y\"\n";
    outFile << "ZONE NODES=" << points.size() << ", ELEMENTS=" << triangles.size() << ", DATAPACKING=POINT, ZONEfloat=FETRIANGLE\n";

    // Write point cloud
    for (const auto& point : points) {
        outFile << point.x << " " << point.y << "\n";
    }

    // Write triangle vertices
    for (const auto& triangle: triangles) {
        outFile << triangle.vertices[0] + 1 << " " << triangle.vertices[1] + 1 << " " << triangle.vertices[2] + 1 << "\n";
    }

    outFile.close();
}


void Triangular::delaunayTriangulate() {
    int npts = points.size();

    // Normalize points to (0,0) - (1,1) boundary
    float xmin = 0.f, ymin = 0.f, dmax = 0.f;
    normalizePoints(xmin, ymin, dmax);

    // Sort points by proximity
    binSort();

    // Add super triangle
    points.resize(npts + 3);
    points[npts] = { -100, -100 };
    points[npts + 1] = { 100, -100 };
    points[npts + 2] = { 0, 100 };
    npts += 3;

    // Create vertices and edges
    int ntri = 1;
    triangles.resize(ntri);
    triangles[0].vertices = { npts - 3, npts - 2, npts - 1 };
    triangles[0].edges = { -1, -1, -1 };
    vector<int> triStack(npts - 3);
    int triOnStack = -1;

    // Insert all points and triangulate one by one
    for (int i = 0; i < npts - 3; i++) {
        // Find triangle T which contains points[i]
        // where ntri - 1 is last triangle created
        int T = searchForTriangle(i, ntri - 1);

        // Create new triangles around point i
        ntri += 2;
        createNewTriangles(ntri, T, i, triOnStack, triStack);

        // Fix triangles in the stack
        emptyStack(triOnStack, i, triStack);
    }

    // Remove Super Triangle vertices
    removeSuperTriangle(ntri);

    // Undo the mapping
    denormalizePoints(xmin, ymin, dmax);

    // Save to TecPlot data file
    writeTecplotFile("Triangulation.dat");

}

void Triangular::readTecplotFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
    }

    // Skip headers until we find the data section
    while (std::getline(file, line)) {
        if (line.find("ZONE") != std::string::npos) {
            break;
        }
    }

    // Read point data
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Node point;
        if (iss >> point.x >> point.y) {
            points.push_back(point);
        }
    }

    file.close();
}
