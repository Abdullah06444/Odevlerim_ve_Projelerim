#include <time.h>
#include <GL/glut.h>   
#include <cmath>

#include "Rectangle.h"
#include "Circle.h"


// YOU CAN CHANGE THE NUMBER OF SHAPES 
#define SHAPE_COUNT 20

// YOU CAN MODIFY WINDOW SIZE BY CHANGING THESE
// YOU MAY ALSO VIEW WINDOW IN FULL SCREEN
#define WINDOWX 500
#define WINDOWY 500

// UNCOMMENT THE LINE BELOW TO STOP MOVING SHAPES
//#define NO_MOTION

// CHANGE THESE DIMENSIONS HOWEVER YOU LIKE
#define MAX_SHAPE_DIM 70
#define MIN_SHAPE_DIM 10

float g_windowWidth = WINDOWX;
float g_windowHeight = WINDOWY;

Shape*	g_shapeList[SHAPE_COUNT];
int		g_numShapes = 0;
bool	g_bShowIntersection = true;



//------------------------------------

void Initialize()
{
	srand ( time(NULL) );
	
	// delete previous shapes, if there is any
	if (g_numShapes > 0)
	{
		for (int i = 0; i < g_numShapes; i++)
			delete g_shapeList[i];
	}

	// create a new shape repository
	do {
		g_numShapes = rand() % SHAPE_COUNT; // number of shapes are randomly determined		
	} while (g_numShapes < 5); // we dont want to have less than 5 shapes
	
	
	int rect_count = g_numShapes * (rand() % 10 / 10.0f);
	int circle_count =  g_numShapes - rect_count;

	int half_wind_x = 3* g_windowWidth / 4;
	int half_wind_y = 3* g_windowHeight / 4;
	int max_dim = MAX_SHAPE_DIM; // max dim. of any shape 
	int min_dim = MIN_SHAPE_DIM; // min dim. of any shape
	int quad_wind = g_windowWidth / 4;
	
	for (int i= 0; i<g_numShapes; i++)
	{
		
		float x, y;
		float v1, v2;

		// set positions 
		do {
			x = rand() % half_wind_x;
		} while (x <= quad_wind);

		do {
			y = rand() % half_wind_y;
		} while (y <= quad_wind);
		Point2d pos = { x,y };

		// set velocity directions 
		do{
			v1 = rand() % 10 / 10.0f;
			v2 = rand() % 10 / 10.0f;			
		} while (v1 == 0 || v2 == 0);

		v1 *= (rand() % 2) ? -1 : 1;
		v2 *= (rand() % 2) ? -1 : 1;
		float vnorm = sqrt(v1*v1 + v2*v2);
		Point2d veldir = { v1 / vnorm, v2 / vnorm };

		// set velocity 
		float vel;
		do {
			vel = rand() % 2 / 10.0f;
		} while (vel == 0);

#ifdef NO_MOTION
		vel = 0.0f;
#endif
		//set color 
		float R = rand()%100/100.0f;
		float G = rand()%100/100.0f;
		float B = rand()%100/100.0f;
		Point3d color = { R,G,B };
		
		// construct objects
		if (i < rect_count)
		{
			float wx;
			float wy;
			do {
				wx = rand() % quad_wind;
			} while (wx < min_dim || wx>max_dim);

			do {
				wy = rand() % quad_wind;
			} while (wy < min_dim || wy>max_dim);

			Point2d size = { wx, wy };			

			Rect* pRect = new Rect(pos, size, veldir, vel, color);
			g_shapeList[i] = pRect;
		}
		else
		{
			float rad;
			do {
				rad = rand() % quad_wind;
			} while (rad < min_dim || rad>max_dim);

			
			Circ* pCirc = new Circ(pos, rad, veldir, vel, color);
			g_shapeList[i] = pCirc;
		}
	}
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
}
//-------------------------------------
// This function handles the intersections of shapes. 
// if the user is not interested in marking intersections
// s/he can set bMarkIntersections to false..in this case
// no intersection test is performed
void MarkObjects(bool bMarkIntersections)
{
	if (bMarkIntersections == false)
	{
		for (int i = 0; i < g_numShapes; i++)
			g_shapeList[i]->UnMarkShape();
	}
	else
	{
		// reset the states of all shapes as unmarked
		for (int i = 0; i < g_numShapes; i++)
			g_shapeList[i]->UnMarkShape();

		for (int i = 0; i < g_numShapes; i++)
		{			
			for (int j = i+1; j < g_numShapes; j++)
			{

				if (g_shapeList[i]->intersects(g_shapeList[j]))
				{
					g_shapeList[i]->MarkShape();
					g_shapeList[j]->MarkShape();

				}
			}
		}
	}

}
//------------------------------------
void UpdateData()
{
	// create viewport bounding rectangles to keep the shapes within the viewport
	Point2d Winpos = { -1.0,0.0 };
	Point2d Winsize = { 1.0 , g_windowHeight };
	Point2d Winveldir = { 0,0 }; // dummy veldir
	float Winvel = 0.0f; //not moving
	Point3d Wincol = { 0,0,0 }; // dummy color
	Rect WindowRectLeft(Winpos, Winsize, Winveldir, Winvel, Wincol);
	Winpos.x = 0.0; Winpos.y = -1.0;
	Winsize.x = g_windowWidth; Winsize.y = 1.0;
	Rect WindowRectBottom(Winpos, Winsize, Winveldir, Winvel, Wincol);
	Winpos.x = g_windowWidth; Winpos.y = 0.0;
	Winsize.x = 1; Winsize.y = g_windowHeight;
	Rect WindowRectRight(Winpos, Winsize, Winveldir, Winvel, Wincol);
	Winpos.x = 0.0; Winpos.y = g_windowHeight;
	Winsize.x = g_windowWidth; Winsize.y = 1.0f;
	Rect WindowRectUp(Winpos, Winsize, Winveldir, Winvel, Wincol);

	for (int i = 0; i < g_numShapes; i++)
	{
		// move the shape
		g_shapeList[i]->move();

		// if it bounces to the window walls, invert its veldir
		if (g_shapeList[i]->intersects(&WindowRectLeft) ||
			g_shapeList[i]->intersects(&WindowRectRight))
			g_shapeList[i]->invert_xdir();

		if (g_shapeList[i]->intersects(&WindowRectBottom) ||
			g_shapeList[i]->intersects(&WindowRectUp))
			g_shapeList[i]->invert_ydir();
	}
}
//------------------------------------
void ChangeSize(GLsizei w, GLsizei h)
{
	if(h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	g_windowHeight = h;
	g_windowWidth = w;
	
	glOrtho(0, g_windowWidth, 0, g_windowHeight , 1.0f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
//------------------------------------
void processNormalKeys(unsigned char key, int x, int y) 
{

	if (key == 'q') // PRESS 'q' to terminate the application
		exit(0);
	if(key=='r') // PRESS 'r' ket to reset the shapes
		Initialize();
	if (key == 's') // toggle between showing the intersections or not
		g_bShowIntersection = g_bShowIntersection ? false: true;
	
	
}
//------------------------------------
void processSpecialKeys(int key, int x, int y) 
{

	switch(key) {
		case GLUT_KEY_LEFT :				
			break;
		case GLUT_KEY_RIGHT :		
			break;
		case GLUT_KEY_UP:	
			// PRESSING UP ARROW KEY INCREASES THE SHAPE VELOCITIES
			for (int i = 0; i < g_numShapes; i++)
				g_shapeList[i]->increase_vel();
			break;
		case GLUT_KEY_DOWN:
			// PRESSING DOWN ARROW KEY DECREASES THE SHAPE VELOCITIES
			for (int i = 0; i < g_numShapes; i++)
				g_shapeList[i]->decrease_vel();

			break;
	}

}

//-------------------------------------
void display() {


	glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer

	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UpdateData();
	MarkObjects(g_bShowIntersection);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i= 0; i<g_numShapes; i++)
		g_shapeList[i]->draw();


	glutSwapBuffers();
}
//------------------------------------
int main(int argc, char* argv[])
{	

	glutInit(&argc, argv);          // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WINDOWX, WINDOWY);
	
	

	glutCreateWindow("COM102B - PA4");  
	

	// Register callback handler for window re-paint
	glutDisplayFunc(display);       
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(display);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	Initialize();

	
	glutMainLoop();                 // Enter infinitely event-processing loop

	return 0;
}
