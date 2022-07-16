#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#define pi (2 * acos(0.0))
using namespace std;
typedef vector<double> vect;
typedef vector<vect> matrix;

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
    printMatrix(m1);
    printMatrix(m2);
    matrix ans(m1.size(), vect (m2[0].size(), 0.0));
    for(int j = 0; j < ans[0].size(); j++) {
        for(int i = 0; i < ans.size(); i++) {
            for(int k = 0; k < m2.size(); k++) {
                ans[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    printMatrix(ans);
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

int main() {
    ifstream in;
    ofstream out;
    string dirName;

    /* input directory name */
    //cout << "Enter directory name:\n";
    //cin >> dirName;
    dirName = "t1";

    /* input gluLookAt & gluPerspective parameters */
    in.open(dirName + "/scene.txt");
    double eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ;
    double fovY, aspectRatio, near, far;
    in >> eyeX >> eyeY >> eyeZ;
    in >> lookX >> lookY >> lookZ;
    in >> upX >> upY >> upZ;
    in >> fovY >> aspectRatio >> near >> far;

    /* stage 1: modeling transformation */
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
                out << fixed << setprecision(7) << point[0] << " " << point[1] << " " << point[2] << endl;
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

    /* stage 2: view transformation */
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
            out << fixed << setprecision(7) << point[0] << " " << point[1] << " " << point[2] << endl;
        }
        out << endl;
    }
    in.close();
    out.close();

    /* stage 3: projection transformation */
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
            out << fixed << setprecision(7) << point[0] << " " << point[1] << " " << point[2] << endl;
        }
        out << endl;
    }
    in.close();
    out.close();

    /* stage4: clipping & scan conversion using z-buffer algorithm */
}
