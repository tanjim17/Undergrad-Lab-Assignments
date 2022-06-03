#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

struct point {
	double x,y,z;
};

const int SLICE_COUNT = 32; // need to be divisible by 4
const int RADIUS = 20;
const int HEIGHT = 10;
const double MOVE_AMOUNT = 4;
const double ROTATION_AMOUNT = 4;

double cameraHeight, cameraAngle;
double distanceTravelled, rotationAngle;
point center;

void drawGrid() {
	int i;
    glColor3f(1, 1, 1);
    glBegin(GL_LINES); {
        for(i = -8; i <= 8; i++) {
            // lines parallel to Y-axis
            glVertex3f(i*20, -200, 0);
            glVertex3f(i*20,  200, 0);
            // lines parallel to X-axis
            glVertex3f(-200, i*20, 0);
            glVertex3f( 200, i*20, 0);
        }
    } glEnd();
}

void drawWheel(double radius, double height, int sliceCount) {
    point p[sliceCount + 1];
    point spokePoints[4];

    int divisor = sliceCount / 4;
    double angle;
    for (int i = 0; i < sliceCount + 1; i++) {
        angle = (i / (double)sliceCount) * pi * 2;
        if( i % divisor == 0 && i < sliceCount) {
            spokePoints[i / divisor] = {radius * cos(angle), radius * sin(angle), height / 4};
        }
        p[i] = {radius * cos(angle), radius * sin(angle), height / 2};
    }

    glPushMatrix() ;
    glTranslatef(0, 0, radius);
    glTranslatef(center.x, center.y, center.z) ;
    glRotatef(rotationAngle, 0, 0, 1) ;
    glRotatef(distanceTravelled * (360 / (2 * pi * radius)), 0, 1, 0) ;
    glRotatef(90, 1, 0, 0) ;

    /** draw rim of wheel **/
    for (int i = 0; i < sliceCount; i++) {
        if (i % 2) glColor3f(0.6, 0.6, 0.6);
        else glColor3f(0.3, 0.3, 0.3);
            glBegin(GL_QUADS); {
                glVertex3f(p[i].x, p[i].y, p[i].z) ;
                glVertex3f(p[i + 1].x, p[i + 1].y, p[i + 1].z) ;
                glVertex3f(p[i + 1].x, p[i + 1].y, -p[i + 1].z) ;
                glVertex3f(p[i].x, p[i].y, -p[i].z) ;
            } glEnd();
    }

    /** draw spoke of wheel **/
    glColor3f(1, 1, 1);
    for (int i = 0; i < 2; i++) {
        glBegin(GL_QUADS); {
            glVertex3f(spokePoints[i].x, spokePoints[i].y, spokePoints[i].z) ;
            glVertex3f(spokePoints[i + 2].x, spokePoints[i + 2].y, spokePoints[i + 2].z) ;
            glVertex3f(spokePoints[i + 2].x, spokePoints[i + 2].y, -spokePoints[i + 2].z) ;
            glVertex3f(spokePoints[i].x, spokePoints[i].y, -spokePoints[i].z) ;
        } glEnd();
    }
    glPopMatrix();
}

void keyboardListener(unsigned char key, int x,int y) {
    double angle;
	switch(key) {
		case '1':
            distanceTravelled += MOVE_AMOUNT;
		    angle = rotationAngle * pi / 180;
            center.x += MOVE_AMOUNT * cos(angle);
            center.y += MOVE_AMOUNT * sin(angle);
            break;
        case '2':
            distanceTravelled -= MOVE_AMOUNT;
		    angle = rotationAngle * pi / 180;
            center.x -= MOVE_AMOUNT * cos(angle);
            center.y -= MOVE_AMOUNT * sin(angle);
            break;
        case '3':
            rotationAngle += ROTATION_AMOUNT;
            break;
        case '4':
            rotationAngle -= ROTATION_AMOUNT;
            break;
		default:
			break;
	}
}

void specialKeyListener(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:
			cameraHeight += 3.0;
			break;
		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
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
	gluLookAt(100 * cos(cameraAngle), 100 * sin(cameraAngle), cameraHeight,    0, 0, 0,    0, 0, 1);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	//add objects
	drawGrid();
	drawWheel(RADIUS, HEIGHT, SLICE_COUNT);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	cameraHeight = 100;
	cameraAngle = 1;
    distanceTravelled = rotationAngle = 0;
    center = {0, 0, 0};

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

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Wheel");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
