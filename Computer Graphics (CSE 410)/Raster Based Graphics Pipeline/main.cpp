#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#define pi (2 * acos(0.0))
using namespace std;
typedef vector<double> vect;
typedef vector<vect> matrix;
typedef vector<vector<vector<int>>> colorMatrix;

void printMatrix(matrix m) {
    for(int i = 0; i < m.size(); i++) {
        for(int j = 0; j < m[0].size(); j++) {
            cout << m[i][j] << " ";
        }
        cout << "\n";
    }
     cout << "\n";
}

void printVector(vect v) {
    for(int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << "\n\n";
}

double degToRad(double theta) {
    return theta * pi / 180.0;
}

vect sumVect(vect v1, vect v2) {
    vect ans(v1.size());
    for(int i = 0; i < ans.size(); i++) {
        ans[i] = v1[i] + v2[i];
    }
    return ans;
}

vect scalarMult(vect v1, double x) {
    vect ans(v1.size());
    for(int i = 0; i < ans.size(); i++) {
        ans[i] = v1[i] * x;
    }
    return ans;
}

double dotProduct(vect v1, vect v2) {
    double ans = 0.0;
    for(int i = 0; i < v1.size(); i++) {
        ans += v1[i] * v2[i];
    }
    return ans;
}

vect crossProduct(vect v1, vect v2) {
    vect ans(3);
    ans[0] = v1[1] * v2[2] - v1[2] * v2[1];
    ans[1] = v1[2] * v2[0] - v1[0] * v2[2];
    ans[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return ans;
}

vect applyRodriguesFormula(vect x, vect a, double theta) {
    return sumVect(scalarMult(x, cos(degToRad(theta))), sumVect(
        scalarMult(a, dotProduct(a, x) * (1 - degToRad(theta))),
        scalarMult(crossProduct(a, x), sin(degToRad(theta)))));
}

void scaleByW(vect& v) {
    for (int i = 0; i < 4; i++) {
        v[i] /= v[3];
    }
}

void normalize(vect& v) {
     double factor = 0.0;
     for (int i = 0; i < v.size(); i++) {
        factor += pow(v[i], 2.0);
    }
    factor = sqrt(factor);
    for (int i = 0; i < v.size(); i++) {
        v[i] /= factor;
    }
}

matrix multiplyMatrices(matrix m1, matrix m2) {
    // printMatrix(m1);
    // printMatrix(m2);
    matrix ans(m1.size(), vect (m2[0].size(), 0.0));
    for(int j = 0; j < ans[0].size(); j++) {
        for(int i = 0; i < ans.size(); i++) {
            for(int k = 0; k < m2.size(); k++) {
                ans[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    // printMatrix(ans);
    return ans;
}

matrix createIdentityMatrix() {
    matrix mat(4, vect (4));
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            mat[i][j] = i == j ? 1.0 : 0.0;
        }
    }
    return mat;
}

matrix createTranslationMatrix(double tx, double ty, double tz) {
    matrix mat = createIdentityMatrix();
    mat[0][3] = tx;
    mat[1][3] = ty;
    mat[2][3] = tz;
    return mat;
}

matrix createScalingMatrix(double sx, double sy, double sz) {
    matrix mat = createIdentityMatrix();
    mat[0][0] = sx;
    mat[1][1] = sy;
    mat[2][2] = sz;
    return mat;
}

matrix createRotationMatrix(double angle, double ax, double ay, double az) {
    vect a{ax, ay, az};
    normalize(a);

    vect c1 = applyRodriguesFormula({1.0, 0.0, 0.0}, a, angle);
    vect c2 = applyRodriguesFormula({0.0, 1.0, 0.0}, a, angle);
    vect c3 = applyRodriguesFormula({0.0, 0.0, 1.0}, a, angle);

    matrix mat = createIdentityMatrix();
    mat[0][0] = c1[0];
    mat[1][0] = c1[1];
    mat[2][0] = c1[2];

    mat[0][1] = c2[0];
    mat[1][1] = c2[1];
    mat[2][1] = c2[2];

    mat[0][2] = c3[0];
    mat[1][2] = c3[1];
    mat[2][2] = c3[2];
    return mat;
}

matrix createViewMatrix(vect eye, vect look, vect up) {
    vect l = sumVect(look, scalarMult(eye, -1.0));
    normalize(l);
    vect r = crossProduct(l, up);
    normalize(r);
    vect u = crossProduct(r, l);

    matrix translation = createTranslationMatrix(-eye[0], -eye[1], -eye[2]);

    matrix rotation = createIdentityMatrix();
    rotation[0][0] = r[0];
    rotation[0][1] = r[1];
    rotation[0][2] = r[2];

    rotation[1][0] = u[0];
    rotation[1][1] = u[1];
    rotation[1][2] = u[2];

    rotation[2][0] = -l[0];
    rotation[2][1] = -l[1];
    rotation[2][2] = -l[2];
    return multiplyMatrices(translation, rotation);
}

matrix createProjectionMatrix(double fovY, double aspectRatio, double near, double far) {
    double fovX = fovY * aspectRatio;
    double r = near * tan(degToRad(fovX / 2.0));
    double t = near * tan(degToRad(fovY / 2.0));

    matrix mat(4, vect(4, 0.0));
    mat[0][0] = near/r;
    mat[1][1] = near/t;
    mat[2][2] = -(far+near)/(far-near);
    mat[2][3] = -(2.0*far*near)/(far-near);
    mat[3][2] = -1.0;
    return mat;
}

matrix vectorToMatrix(vect v) {
    matrix ans(v.size(), vect (1));
    for(int i = 0; i < v.size(); i++) {
        ans[i][0] = v[i];
    }
    return ans;
}

vect matrixToVector(matrix m) {
    vect ans(m.size());
    for(int i = 0; i < m.size(); i++) {
        ans[i] = m[i][0];
    }
    return ans;
}

struct Triangle {
    double points[3][3];
    int color[3];
};

void updateBuffers(double& zEntry, vector<int>& frameEntry, Triangle& triangle, double zp, double frontLimitZ) {
    if(zp >= frontLimitZ && zp < zEntry) {
        zEntry = zp;
        for(int c = 0; c < 3; c++) {
            frameEntry[c] = triangle.color[c];
        }
    }
}

pair<double, double> findIntersections(Triangle triangle, double ys, pair<int, int> endPoints) {
    double x1 = triangle.points[endPoints.first][0];
    double y1 = triangle.points[endPoints.first][1];
    double z1 = triangle.points[endPoints.first][2];

    double x2 = triangle.points[endPoints.second][0];
    double y2 = triangle.points[endPoints.second][1];
    double z2 = triangle.points[endPoints.second][2];

    double x = x1 + ((x2 - x1) / (y2 - y1)) * (ys - y1);
    double z = z1 + ((z2 - z1) / (y2 - y1)) * (ys - y1);

    return {x, z};
}

int main() {
    matrix points(3, vect(3));
    ifstream in;
    ofstream out;
    out << setprecision(7) << fixed;
    string dirName;

    /* Input directory name, gluLookAt and gluPerspective parameters */

    //cout << "Enter directory name:\n";
    //cin >> dirName;
    dirName = "t1";
    in.open(dirName + "/scene.txt");
    double eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ;
    double fovY, aspectRatio, near, far;
    in >> eyeX >> eyeY >> eyeZ;
    in >> lookX >> lookY >> lookZ;
    in >> upX >> upY >> upZ;
    in >> fovY >> aspectRatio >> near >> far;

    /* Stage 1: Modeling Transformation */

    out.open(dirName + "/stage1.txt");
    stack<matrix> transformationStack;
    transformationStack.push(createIdentityMatrix());
    int triangleCount = 0;
    string cmd;

    while(true) {
        in >> cmd;
        if(cmd == "triangle") {
            vect point(4);
            for (int i = 0; i < 3; i++) {
                in >> point[0] >> point[1] >> point[2];
                point[3] = 1.0;
                point = matrixToVector(multiplyMatrices(transformationStack.top(), vectorToMatrix(point)));
                //printVector(point);
                scaleByW(point);
                out << point[0] << " " << point[1] << " " << point[2] << endl;
            }
            out << endl;
            triangleCount++;
        } else if(cmd == "translate") {
            double tx, ty, tz;
            in >> tx >> ty >> tz;
            matrix temp = multiplyMatrices(transformationStack.top(), createTranslationMatrix(tx, ty, tz));
            transformationStack.pop();
            transformationStack.push(temp);
        } else if(cmd == "scale") {
            double sx, sy, sz;
            in >> sx >> sy >> sz;
            matrix temp = multiplyMatrices(transformationStack.top(), createScalingMatrix(sx, sy, sz));
            transformationStack.pop();
            transformationStack.push(temp);
        } else if(cmd == "rotate") {
            double angle, ax, ay, az;
            in >> angle >> ax >> ay >> az;
            matrix temp = multiplyMatrices(transformationStack.top(), createRotationMatrix(angle, ax, ay, az));
            transformationStack.pop();
            transformationStack.push(temp);
        } else if(cmd == "push") {
            transformationStack.push(transformationStack.top());
        } else if(cmd == "pop") {
            transformationStack.pop();
        } else if(cmd == "end") {
            break;
        } else {
            cout << "invalid command!\n";
            exit(1);
        }
    }
    in.close();
    out.close();

    /* Stage 2: View Transformation */

    in.open(dirName + "/stage1.txt");
    out.open(dirName + "/stage2.txt");

    matrix viewTransformation = createViewMatrix({eyeX, eyeY, eyeZ}, {lookX, lookY, lookZ}, {upX, upY, upZ});

    for(int i = 0; i < triangleCount; i++) {
        vect point(4);
        for (int i = 0; i < 3; i++) {
            in >> point[0] >> point[1] >> point[2];
            point[3] = 1.0;
            point = matrixToVector(multiplyMatrices(viewTransformation, vectorToMatrix(point)));
            scaleByW(point);
            out << point[0] << " " << point[1] << " " << point[2] << endl;
        }
        out << endl;
    }
    in.close();
    out.close();

    /* Stage 3: Projection Transformation */

    in.open(dirName + "/stage2.txt");
    out.open(dirName + "/stage3.txt");

    matrix projectionTransformation = createProjectionMatrix(fovY, aspectRatio, near, far);

    for(int i = 0; i < triangleCount; i++) {
        vect point(4);
        for (int i = 0; i < 3; i++) {
            in >> point[0] >> point[1] >> point[2];
            point[3] = 1.0;
            point = matrixToVector(multiplyMatrices(projectionTransformation, vectorToMatrix(point)));
            scaleByW(point);
            out << point[0] << " " << point[1] << " " << point[2] << endl;
        }
        out << endl;
    }
    in.close();
    out.close();

    /* Stage 4: Clipping & Scan Conversion Using Z-buffer Algorithm */

    /* read data */

    in.open(dirName + "/config.txt");
    int screenWidth, screenHeight;
    double leftLimitX, rightLimitX, bottomLimitY, topLimitY, frontLimitZ, rearLimitZ;
    in >> screenWidth >> screenHeight;
    in >> leftLimitX;
    in >> bottomLimitY;
    in >> frontLimitZ >> rearLimitZ;
    in.close();

    rightLimitX = -leftLimitX;
    topLimitY = -bottomLimitY;

    in.open(dirName + "/stage3.txt");
    Triangle triangles[triangleCount];
    for (int k = 0; k < triangleCount; k++) {
        for (int i = 0; i < 3; i++) {
            in >> triangles[k].points[i][0] >> triangles[k].points[i][1] >> triangles[k].points[i][2];
        }
        for (int i = 0; i < 3; i++) { // r, g, b
            triangles[k].color[i] = rand() % 256;
        }
    }
    in.close();

    /* initialize z-buffer and frame buffer */

    double dx, dy, leftX, rightX, topY, bottomY;
    dx = (rightLimitX - leftLimitX) / screenWidth;
    dy = (topLimitY - bottomLimitY) / screenHeight;
    leftX = leftLimitX + (dx / 2.0);
    rightX = rightLimitX - (dx / 2.0);
    topY = topLimitY - (dy / 2.0);
    bottomY = bottomLimitY + (dy / 2.0);

    matrix zBuffer(screenHeight, vect (screenWidth, rearLimitZ));
    colorMatrix frameBuffer(screenHeight, vector<vector<int>> (screenWidth, vector<int> (3, 0)));

    /* applying procedure */

    int topScanLine, bottomScanLine;
    for(int k = 0; k < triangleCount; k++) {
        /* finding top scan line & bottom scan line after necessary clipping */
        double maxY = max(triangles[k].points[0][1], max(triangles[k].points[1][1], triangles[k].points[2][1]));
        double minY = min(triangles[k].points[0][1], min(triangles[k].points[1][1], triangles[k].points[2][1]));
        topScanLine = maxY > topY ? 0 : (int)round((topY - maxY) / dy);
        bottomScanLine = minY < bottomY ? screenHeight - 1 : screenHeight - 1 - ((int)round((minY - bottomY) / dy));

        int leftIntersectingColumn, rightIntersectingColumn;
        for(int i = topScanLine; i <= bottomScanLine; i++) {
            double ys = topY - i * dy;

            pair<int,int> a = {0, 1};
            pair<int,int> b = {0, 2};
            // checking whether y values are equal. if equal, we will get 0 in the denominator
            // while using the formula to calculate x and z
            if (triangles[k].points[a.first][1] == triangles[k].points[a.second][1]) {
                a = {1, 2};
            } else if (triangles[k].points[b.first][1] == triangles[k].points[b.second][1]) {
                b = {1, 2};
            }

            double xa, xb, za, zb;
            pair<double, double> temp = findIntersections(triangles[k], ys, a);
            xa = temp.first; za = temp.second;
            temp = findIntersections(triangles[k], ys, b);
            xb = temp.first; zb = temp.second;
            if (xa > xb) {
                swap(xa, xb);
                swap(za, zb);
            }

            leftIntersectingColumn = xa > leftX ? round((xa - leftX) / dx) : 0;
            rightIntersectingColumn = xb < rightX ? round((xb - leftX) / dx) : screenWidth - 1;


            double xp = leftX + leftIntersectingColumn * dx;
            double factor = (zb - za) / (xb - xa);
            double zp = za + factor * (xp - xa);
            double c = factor * dx;
            updateBuffers(zBuffer[i][leftIntersectingColumn], frameBuffer[i][leftIntersectingColumn],
                triangles[k], zp, frontLimitZ);
            for(int j = leftIntersectingColumn + 1; j <= rightIntersectingColumn; j++) {
                zp = zp + c;
                updateBuffers(zBuffer[i][j], frameBuffer[i][j], triangles[k], zp, frontLimitZ);
            }
        }
    }

    /* saving output */

    bitmap_image bitmapImage(screenWidth, screenHeight);
    for(int i = 0; i < screenHeight; i++) {
        for(int j = 0; j < screenWidth; j++) {
            bitmapImage.set_pixel(j, i, frameBuffer[i][j][0], frameBuffer[i][j][1], frameBuffer[i][j][2]);
        }
    }
    bitmapImage.save_image(dirName + "/out.bmp");

    out.open(dirName + "/z-buffer.txt");
    for(int i = 0; i < screenHeight; i++) {
        for(int j = 0; j < screenWidth; j++) {
            if(zBuffer[i][j] < rearLimitZ) {
                out << zBuffer[i][j] << '\t';
            }
        }
        out << "\n";
    }
    out.close();

    return 0;
}
