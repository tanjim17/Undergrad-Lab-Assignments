#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <windows.h>
#include <GL/glut.h>

#define pi (2 * acos(0.0))

struct point
{
	double x,y,z;
};
typedef point vect;

const double ROTATION_ANGLE = 3 * pi / 180;
const double TRANSLATION_AMOUNT = 2;
const int STACK_COUNT = 25;
const int SLICE_COUNT = 25;
const double SIDE = 60;

double radius, side;
point pos;
vect u, r, l;

vect sum(vect v1, vect v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vect scale(vect v, double factor) {
    return {v.x * factor, v.y * factor, v.z * factor};
}

vect cross_product(vect v1, vect v2) {
    return {v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

vect rotate_(vect v, vect perp_v, double angle) {
    return sum(scale(v, cos(angle)), scale(cross_product(perp_v, v), sin(angle)));
}

void drawAxes() {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES); {
        glVertex3f( 100,0,0);
        glVertex3f(-100,0,0);

        glVertex3f(0,-100,0);
        glVertex3f(0, 100,0);

        glVertex3f(0,0, 100);
        glVertex3f(0,0,-100);
    } glEnd();
}

void drawSquare(double a) {
	glBegin(GL_QUADS); {
        glVertex3f(a, a, 0);
        glVertex3f(a, -a, 0);
        glVertex3f(-a, -a, 0);
        glVertex3f(-a, a, 0);
	} glEnd();
}

void drawSphereSlice(double radius) {
    point p[STACK_COUNT][SLICE_COUNT];
    double h, r, angle;
    for (int i = 0; i < STACK_COUNT; i++) {
        angle = (i / (double)STACK_COUNT) * pi / 2;
        r = radius * cos(angle);
        h = radius * sin(angle);
        for (int j = 0; j < SLICE_COUNT; j++) {
            angle = (j / (double)SLICE_COUNT) * pi / 2;
            p[i][j] = {r * cos(angle), r * sin(angle), h};
        }
    }
    for (int i = 0; i < STACK_COUNT - 1; i++) {
        for (int j = 0; j < SLICE_COUNT - 1; j++) {
            glBegin(GL_QUADS); {
                glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
                glVertex3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
                glVertex3f(p[i + 1][j + 1].x, p[i + 1][j + 1].y, p[i + 1][j + 1].z);
                glVertex3f(p[i + 1][j].x, p[i + 1][j].y, p[i + 1][j].z);
            } glEnd();
        }
    }
}

void drawCylinderSlice(double radius, double height) {
    point p[SLICE_COUNT];
    double angle;
    for (int i = 0; i < SLICE_COUNT; i++) {
        angle = (i / (double)SLICE_COUNT) * pi / 2;
        p[i] = {radius * cos(angle), radius * sin(angle), height};
    }
    for (int i = 0; i < SLICE_COUNT - 1; i++) {
        glBegin(GL_QUADS); {
            glVertex3f(p[i].x, p[i].y, p[i].z);
            glVertex3f(p[i + 1].x, p[i + 1].y, p[i + 1].z);
            glVertex3f(p[i + 1].x, p[i + 1].y, -p[i + 1].z);
            glVertex3f(p[i].x, p[i].y, -p[i].z);
        } glEnd();
    }
}

void drawSphereCube(double radius) {
    double side = SIDE - radius * 2;
    double sideHalf = side / 2;
    point corners[4] = {{sideHalf, sideHalf, sideHalf},
        {-sideHalf, sideHalf, sideHalf},
        {-sideHalf, -sideHalf, sideHalf},
        {sideHalf, -sideHalf, sideHalf}};

    /** draw 1/8 of sphere **/
    glColor3f(1, 0, 0);
    for (int i = 0; i < 4; i++) { // top
        glPushMatrix();
        glTranslatef(corners[i].x, corners[i].y, corners[i].z);
        glRotatef(90 * i, 0, 0, 1);
        drawSphereSlice(radius);
        glPopMatrix();
    }
    for (int i = 0; i < 4; i++) { // bottom
        glPushMatrix();
        glRotatef(180, 1, 0, 0) ;
        glTranslatef(corners[i].x, corners[i].y, corners[i].z);
        glRotatef(90 * i, 0, 0, 1);
        drawSphereSlice(radius);
        glPopMatrix();
    }

    /** draw 1/4 of cylinder **/
    glColor3f(0, 1, 0);
    for (int i = 0; i < 4; i++) { // along z-axis
        glPushMatrix();
        glTranslatef(corners[i].x, corners[i].y, 0);
        glRotatef(90 * i, 0, 0, 1);
        drawCylinderSlice(radius, sideHalf);
        glPopMatrix();
    }
    for (int i = 0; i < 4; i++) { // along y-axis
        glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glTranslatef(corners[i].x, corners[i].y, 0);
        glRotatef(90 * i, 0, 0, 1);
        drawCylinderSlice(radius, sideHalf);
        glPopMatrix();
    }
    for (int i = 0; i < 4; i++) { // along x-axis
        glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glTranslatef(corners[i].x, corners[i].y, 0);
        glRotatef(90 * i, 0, 0, 1);
        drawCylinderSlice(radius, sideHalf);
        glPopMatrix();
    }

    /** draw square **/
    glColor3f(1, 1, 1);
    glPushMatrix() ;
    glTranslatef(0, 0, SIDE / 2);
    drawSquare(sideHalf); // top
    glTranslatef(0, 0, -SIDE);
    drawSquare(sideHalf); // bottom
    glPopMatrix();
    for (int i = 0 ; i < 4; i++) { // lateral
        glPushMatrix();
        glRotatef(90 * i, 0, 0, 1);
        glTranslatef(SIDE / 2, 0, 0);
        glRotatef(90, 0, 1, 0);
        drawSquare(sideHalf);
        glPopMatrix();
    }
}

void keyboardListener(unsigned char key, int x,int y) {
	switch(key) {
        case '1':
            r = rotate_(r, u, ROTATION_ANGLE);
            l = rotate_(l, u, ROTATION_ANGLE);
            break;
        case '2':
            r = rotate_(r, u, -ROTATION_ANGLE);
            l = rotate_(l, u, -ROTATION_ANGLE);
		    break;
        case '3':
            l = rotate_(l, r, ROTATION_ANGLE);
            u = rotate_(u, r, ROTATION_ANGLE);
            break;
        case '4':
            l = rotate_(l, r, -ROTATION_ANGLE);
            u = rotate_(u, r, -ROTATION_ANGLE);
		    break;
        case '5':
            u = rotate_(u, l, ROTATION_ANGLE);
            r = rotate_(r, l, ROTATION_ANGLE);
            break;
        case '6':
            u = rotate_(u, l, -ROTATION_ANGLE);
            r = rotate_(r, l, -ROTATION_ANGLE);
		    break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y) {
	switch(key) {
	    case GLUT_KEY_UP:
            pos = sum(pos, scale(l, TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_DOWN:
            pos = sum(pos, scale(l, -TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_RIGHT:
		    pos = sum(pos, scale(r, TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_LEFT:
		    pos = sum(pos, scale(r, -TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_PAGE_UP:
		    pos = sum(pos, scale(u, TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos = sum(pos, scale(u, -TRANSLATION_AMOUNT));
			break;
		case GLUT_KEY_HOME:
		    if (radius < SIDE / 2) radius++;
			break;
		case GLUT_KEY_END:
		    if (radius > 0) radius--;
			break;
		default:
			break;
	}
}

void display() {
	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?
	point target = sum(pos, l);
	gluLookAt(pos.x,pos.y,pos.z, target.x,target.y,target.z, u.x,u.y,u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	//add objects
	drawAxes();
	drawSphereCube(radius);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate() {
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization
	radius = 15;
	pos = {100, 100, 0};
	u = {0, 0, 1};
	r = {-1/sqrt(2), 1/sqrt(2), 0};
	l = {-1/sqrt(2), -1/sqrt(2), 0};

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv) {
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Sphere-Cube Transformation");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
