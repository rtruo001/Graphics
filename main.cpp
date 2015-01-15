// Name: Randy Truong
// Quarter, Year: Fall, 2014
// Lab: 022
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////

//NECESSARY FOR INLAB
//#include <GL/glut.h>

//USED FOR MAC
#include <GLUT/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include "point2d.h"
#include "point3d.h"
#include "triangles.h"
#include <vector>
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 1200;
const int SPACE_BAR = 32;

//GLOBAL VARIABLES, VECTORS
vector<Point2D> pts;
vector<Point3D> pts_3D;
vector<Triangle3D> tri_3D;
Point3D averages;
bool wireframe_mode;

int points_length;
int triangles_length;

//Z BUFFER
int z_buffer[1200][1200];

//defined FUNCTIONS
void rotateObject(double);
void zBuffer();
void translate_y(double);
void translate_x(double);
void scale_object(double);

/**************************************************************************************
 * function factorial
 *
 * Deals with making a factorial with the number passed in from the parameters
 **************************************************************************************/
double factorial(int num_to_fact)
{
	double num = 1;
	for (int i = num_to_fact; i > 1; --i)
	{
		num *= i;
	}
	return num;
}

/**************************************************************************************
 * function mouse
 *
 * Called whenever mouse is clicked
 **************************************************************************************/
void mouse(int button, int state, int x, int y)
{
	//When the mouse is pressed, puts the point into the pts array
	if (state == GLUT_DOWN)
	{
		cout << "DETECTING BUTTON PRESS" << endl;
		y = WINDOW_HEIGHT - y;
		pts.push_back(Point2D(x,y));	
		cout << "CLICK DETECTED" << endl; 
		cout << "X Point: " << x << endl;
		cout << "Y Point: " << y << endl << endl << endl;
	}
	//Needed to redisplay the screen
	glutPostRedisplay();
}

/**************************************************************************************
 * function keyboard
 *
 * Called whenever buttons w a s d are pressed, used for scaling and rotating
 **************************************************************************************/
void keyboard(unsigned char key, int x, int y)
{
	//When 0 is pressedm clears the pts array
	if (key == '0')
	{
		pts.clear();
		cout << "CLEARING SCREEN" << endl << endl << endl;
	}
	//Rotates when d or a is pressed, d for rotating right
	else if (key == 'd')
	{
		zBuffer();
		rotateObject(0.1);
	}
	//a is for rotating left
	else if (key == 'a')
	{
		zBuffer();
		rotateObject(-0.1);
	}
	//Scales the object larger
	else if (key == 'w')
	{
		zBuffer();
		scale_object(1.03);
	}
	//Scales the object smaller
	else if (key == 's')
	{
		zBuffer();
		scale_object(0.97);
	}
	//The space bar switches the modes
	else if (key == SPACE_BAR)
	{
		zBuffer();
		//Switches modes accordingly
		if (wireframe_mode)
			wireframe_mode = false;
		else
			wireframe_mode = true;
	}

	//Needed to redisplay the screen
	glutPostRedisplay();
}

/**************************************************************************************
 * function keyboard_directions
 *
 * Called whenever buttons up left right or down are pressed, used for translating
 **************************************************************************************/
void keyboard_directions(int key, int x, int y)
{
	//Sets new values for the z buffer and also translate accordingly to the button pressed
	zBuffer();
	if( key == GLUT_KEY_LEFT)
	{
		translate_x(-8);
	}
	else if( key == GLUT_KEY_RIGHT)
	{
		translate_x(8);
	}
	else if( key == GLUT_KEY_UP)
	{
		translate_y(8);
	}
	else if( key == GLUT_KEY_DOWN)
	{
		translate_y(-8);
	}

	//Resets the averages accordingly to the new x y z
	double avg_X = 0, avg_Y = 0, avg_Z = 0;
	for (int i = 0; i < points_length; ++i)
	{
		avg_X += pts_3D[i].x;
		avg_Y += pts_3D[i].y;
		avg_Z += pts_3D[i].z;
	}
	averages = Point3D(avg_X/points_length, avg_Y/points_length, avg_Z/points_length);

	//Needed to redisplay the screen
	glutPostRedisplay();
}

/**************************************************************************************
 * function renderPixel
 *
 * Renders a quad at cell (x, y) with dimensions CELL_LENGTH
 **************************************************************************************/
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);

	glVertex2f(x,y);

	glEnd();
}

/**************************************************************************************
 * function renderPixelColor
 *
 * Renders a quad at cell (x, y) with dimensions CELL_LENGTH with color
 **************************************************************************************/
void renderPixelColor(int x, int y, float r, float g, float b)
{
	glBegin(GL_POINTS);

	glColor3f(r,g,b);
	glVertex2f(x,y);

	glEnd();
}

/**************************************************************************************
 * function renderLine
 *
 * Renders a line from (x0,y0) to (x1,y1), works for any direction.
 **************************************************************************************/
void renderLine(int x0, int y0, int x1, int y1)
{
	glBegin(GL_POINTS);
		
	if (abs(x1-x0) > abs(y1-y0))
	{
		if (x0 > x1)
		{
			int temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp; 
		}
		double i = x0;
		double m = (double)(y1 - y0) /(double)(x1 - x0);
		double y = y0;

		while (i < x1)
		{
			y += m;
			renderPixel(i,y);
			i = i+1;	
		}
	}

	else
	{
		if (y0 > y1)
		{
			int temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp; 
		}
		double i = y0;
		double m = (double)(x1 - x0) /(double)(y1 - y0);
		double x = x0;

		while (i < y1)
		{
			x += m;
			renderPixel(x,i);
			i = i+1;	
		}
	}

	glEnd();
}

/**************************************************************************************
 * function get_slopes
 *
 * Returns the slopes of the line given by the two points
 **************************************************************************************/
double get_slopes(Point3D highest_y_vertex, Point3D lowest_x_vertex)
{
	double x_slopes = (double)highest_y_vertex.x - (double)lowest_x_vertex.x;
	double y_slopes = (double)highest_y_vertex.y - (double)lowest_x_vertex.y;
	if (x_slopes == 0)
	{
		return highest_y_vertex.x;
	}
	double slopes_total = y_slopes / x_slopes;
	
	return slopes_total;
}

/**************************************************************************************
 * function zBuffer
 *
 * Initializes the values in the Z buffer
 **************************************************************************************/
void zBuffer()
{
	for (int row = 0; row < WINDOW_HEIGHT; ++row)
	{
		for (int col = 0; col < WINDOW_WIDTH; ++col)
		{
			z_buffer[row][col] = 1000;
		}
	}
}

/**************************************************************************************
 * function Cross_Product
 *
 * Does the cross product of two vectors, returning the vector
 **************************************************************************************/
Point3D Cross_Product(Point3D v1,Point3D v2)
{
	//i - A
	//j - B
	//k - C
	double i = (v1.y * v2.z) - (v2.y * v1.z);
	double j = -((v1.x * v2.z) - (v2.x * v1.z));
	double k = (v1.x * v2.y) - (v2.x * v1.y);

	return Point3D(i,j,k);
}

/**************************************************************************************
 * function fillTriangle
 *
 * USED IN ASSIGNMENT 1, Modifed the fillTriangles to work with 1 triangle input
 **************************************************************************************/
void fillTriangle(Triangle3D triangle, float red_color, float green_color, float blue_color)
{
	//	Triangle has 3 variables, index to p1 p2 p3
	
	//Variables for detecting which vertext is the highest, middle, and lowest in terms of the y and x values.
	Point3D highest_y_vertex;
	Point3D lowest_y_vertex;
	Point3D highest_x_vertex;
	Point3D lowest_x_vertex;
	//Middle vertex is just used in context of being the middle y vertex
	Point3D middle_vertex;
	Point3D middle_x_vertex;

	/**************************************************************************************
	 * Sets the highest y vertex to the highest y vertex of the triangle
	 * Sets the lowest y vertex to the lowest y vertext of the triangle
	 * Sets the middle vertex accordingly 	
	 **************************************************************************************/
	//Finds the points accordingly for p1 of the triangle

	if (pts_3D[triangle.p1].y >= pts_3D[triangle.p2].y && pts_3D[triangle.p1].y >= pts_3D[triangle.p3].y)
	{
		highest_y_vertex = pts_3D[triangle.p1];
		if (pts_3D[triangle.p2].y <= pts_3D[triangle.p3].y)
		{
			lowest_y_vertex = pts_3D[triangle.p2];
			middle_vertex = pts_3D[triangle.p3];
		}
		else
		{
			lowest_y_vertex = pts_3D[triangle.p3];
			middle_vertex = pts_3D[triangle.p2];
		}
	}
	//Does the same for the p2 of the triangle
	else if (pts_3D[triangle.p2].y >= pts_3D[triangle.p1].y && pts_3D[triangle.p2].y >= pts_3D[triangle.p3].y)
	{
		highest_y_vertex = pts_3D[triangle.p2];
		if (pts_3D[triangle.p1].y <= pts_3D[triangle.p3].y)
		{
			lowest_y_vertex = pts_3D[triangle.p1];
			middle_vertex = pts_3D[triangle.p3];
		}
		else
		{
			lowest_y_vertex = pts_3D[triangle.p3];
			middle_vertex = pts_3D[triangle.p1];
		}
	}
	//Does the same for the p3 of the triangle
	else
	{
		highest_y_vertex = pts_3D[triangle.p3];
		if (pts_3D[triangle.p1].y <= pts_3D[triangle.p2].y)
		{
			lowest_y_vertex = pts_3D[triangle.p1];
			middle_vertex = pts_3D[triangle.p2];
		}
		else
		{
			lowest_y_vertex = pts_3D[triangle.p2];
			middle_vertex = pts_3D[triangle.p1];
		}
	}

	/**************************************************************************************
	 * Do the same for the x vertices for the highest, middle, and lowest.
	 * Sets the highest x vertex to the highest x vertex of the triangle
	 * Sets the lowest x vertex to the lowest x vertext of the triangle
	 **************************************************************************************/
	//Finds the points accordingly for p1 of the triangle
	if (pts_3D[triangle.p1].x >= pts_3D[triangle.p2].x && pts_3D[triangle.p1].x >= pts_3D[triangle.p3].x)
	{
		highest_x_vertex = pts_3D[triangle.p1];
		if (pts_3D[triangle.p2].x <= pts_3D[triangle.p3].x)
		{
			lowest_x_vertex = pts_3D[triangle.p2];
			middle_x_vertex = pts_3D[triangle.p3];
		}
		else
		{
			lowest_x_vertex = pts_3D[triangle.p3];
			middle_x_vertex = pts_3D[triangle.p2];
		}
	}

	//Does the same for the p2 of the triangle
	else if (pts_3D[triangle.p2].x >= pts_3D[triangle.p1].x && pts_3D[triangle.p2].x >= pts_3D[triangle.p3].x)
	{
		highest_x_vertex = pts_3D[triangle.p2];
		if (pts_3D[triangle.p1].x <= pts_3D[triangle.p3].x)
		{
			lowest_x_vertex = pts_3D[triangle.p1];
			middle_x_vertex = pts_3D[triangle.p3];
		}
		else
		{
			lowest_x_vertex = pts_3D[triangle.p3];
			middle_x_vertex = pts_3D[triangle.p1];
		}
	}
	//Does the same for the p3 of the triangle
	else
	{
		highest_x_vertex = pts_3D[triangle.p3];
		if (pts_3D[triangle.p1].x <= pts_3D[triangle.p2].x)
		{
			lowest_x_vertex = pts_3D[triangle.p1];
			middle_x_vertex = pts_3D[triangle.p2];
		}
		else
		{
			lowest_x_vertex = pts_3D[triangle.p2];
			middle_x_vertex = pts_3D[triangle.p1];
		}
	}

	/**************************************************************************************
	 * Deals with how to fill the triangles depending on how the triangle is shaped
 	 **************************************************************************************/	
	double slope_value_left = 0.0;
	double slope_value_right = 0.0;
	double slope_value_above_mid = 0.0;
	double b_value_left = 0.0;
	double b_value_right = 0.0;
	double b_value_above_mid = 0.0;

	//These next sets the slope of the left line of the triangle, and the slope of the right line
	//We do this to know the boundaries of the triangle
	//We also check the point of the triangle if it can be split into another triangle
	//This would therefore let us do the scanline through each triangle
	if (lowest_x_vertex.x == lowest_y_vertex.x)
	{
		slope_value_left = get_slopes(highest_y_vertex,lowest_x_vertex);
		slope_value_right = get_slopes(middle_vertex,lowest_x_vertex);
		slope_value_above_mid = get_slopes(middle_vertex, highest_y_vertex);
		b_value_right = (double)middle_vertex.y - (slope_value_right) * (double)middle_vertex.x;
		b_value_left = (double)highest_y_vertex.y - (slope_value_left) * (double)highest_y_vertex.x;
		b_value_above_mid = (double)middle_vertex.y - (slope_value_above_mid) * (double)middle_vertex.x;
	}
	else if (highest_x_vertex.x == lowest_y_vertex.x)
	{
		slope_value_left = get_slopes(middle_vertex,highest_x_vertex);
		slope_value_right = get_slopes(highest_y_vertex,highest_x_vertex);
		slope_value_above_mid = get_slopes(middle_vertex, highest_y_vertex);
		b_value_right = (double)highest_y_vertex.y - (slope_value_right) * (double)highest_y_vertex.x;
		b_value_left = (double)highest_x_vertex.y - (slope_value_left) * (double)highest_x_vertex.x;
		b_value_above_mid = (double)middle_vertex.y - (slope_value_above_mid) * (double)middle_vertex.x;
	}
	else if (highest_y_vertex.x == lowest_x_vertex.x)
	{
		slope_value_left = get_slopes(lowest_y_vertex,highest_y_vertex);
		slope_value_right = get_slopes(lowest_y_vertex,middle_vertex);
		slope_value_above_mid = get_slopes(middle_vertex, highest_y_vertex);
		b_value_right = (double)lowest_y_vertex.y - (slope_value_right) * (double)lowest_y_vertex.x;
		b_value_left = (double)highest_y_vertex.y - (slope_value_left) * (double)highest_y_vertex.x;
		b_value_above_mid = (double)middle_vertex.y - (slope_value_above_mid) * (double)middle_vertex.x;
	}
	else if (highest_y_vertex.x == highest_x_vertex.x)
	{
		slope_value_left = get_slopes(lowest_y_vertex,middle_vertex);
		slope_value_right = get_slopes(lowest_y_vertex,highest_y_vertex);
		slope_value_above_mid = get_slopes(middle_vertex, highest_y_vertex);
		b_value_right = (double)lowest_y_vertex.y - (slope_value_right) * (double)lowest_y_vertex.x;
		b_value_left = (double)lowest_y_vertex.y - (slope_value_left) * (double)lowest_y_vertex.x;
		b_value_above_mid = (double)middle_vertex.y - (slope_value_above_mid) * (double)middle_vertex.x;
	}
	else
	{
		slope_value_left = get_slopes(lowest_y_vertex,middle_x_vertex);
		slope_value_right = get_slopes(lowest_y_vertex,highest_x_vertex);
		slope_value_above_mid = get_slopes(middle_vertex, highest_x_vertex);
		b_value_right = (double)lowest_y_vertex.y - (slope_value_right) * (double)lowest_y_vertex.x;
		b_value_left = (double)lowest_y_vertex.y - (slope_value_left) * (double)lowest_y_vertex.x;
		b_value_above_mid = (double)middle_vertex.y - (slope_value_above_mid) * (double)middle_vertex.x;
	}

	/**************************************************************************************
 	 * Uses the plane equation to set up the z buffer.
 	 **************************************************************************************/	
	//The first vector from one point of the triangle to the other
	Point3D v1 = Point3D(pts_3D[triangle.p2].x - pts_3D[triangle.p1].x 
						,pts_3D[triangle.p2].y - pts_3D[triangle.p1].y 
						,pts_3D[triangle.p2].z - pts_3D[triangle.p1].z );
	//The second vector from one point of the triangle to the other
	Point3D v2 = Point3D(pts_3D[triangle.p3].x - pts_3D[triangle.p1].x 
						,pts_3D[triangle.p3].y - pts_3D[triangle.p1].y 
						,pts_3D[triangle.p3].z - pts_3D[triangle.p1].z );
	//The normal vector from the cross products of the previous vectors
	Point3D normalVector = Cross_Product(v1, v2);
	//The plane equation used to find the z
	double A = normalVector.x;
	double B = normalVector.y;
	double C = normalVector.z;
	double D = ((-1)*(A * pts_3D[triangle.p1].x) - (B * pts_3D[triangle.p1].y) - (C * pts_3D[triangle.p1].z));
	double Z = 0.0;//((-A * pts_3D[triangle.p1].x) - (B * pts_3D[triangle.p1].y) - D) / C;

	/**************************************************************************************
 	 * Goes through the scanlines, forming the triangle as well as setting the z values
 	 * Adjusts the triangles accordingly to the different cases of which triangle it is
 	 **************************************************************************************/	
	for (int y_scan = lowest_y_vertex.y; y_scan <= highest_y_vertex.y; ++y_scan)
	{
		//Calculating the Z values
		Z = (((-1)*(A * lowest_x_vertex.x)) - ((B * y_scan)) - D) / C;
		for (int x_scan = lowest_x_vertex.x; x_scan <= highest_x_vertex.x; ++x_scan)
		{
			if (lowest_x_vertex.x == lowest_y_vertex.x)
			{
				if (x_scan >= (((double)y_scan - b_value_left) / slope_value_left) &&
					x_scan <= (((double)y_scan - b_value_right) / slope_value_right))
				{
					if ((x_scan > 0 && x_scan < WINDOW_WIDTH) && (y_scan > 0 && y_scan < WINDOW_HEIGHT))
					{
						if (y_scan > middle_vertex.y)
						{
							slope_value_right = slope_value_above_mid;
							b_value_right = b_value_above_mid;
						}
						if (Z < z_buffer[x_scan][y_scan])
					    {
					    	z_buffer[x_scan][y_scan] = Z;
					    	renderPixelColor(x_scan, y_scan, red_color, green_color, blue_color);
					    }			
					}
				}
			}
			else if (highest_x_vertex.x == lowest_y_vertex.x)
			{
				if (x_scan >= (((double)y_scan - b_value_left) / slope_value_left) &&
					x_scan <= (((double)y_scan - b_value_right) / slope_value_right))
				{
					if ((x_scan > 0 && x_scan < WINDOW_WIDTH) && (y_scan > 0 && y_scan < WINDOW_HEIGHT))
					{
						if (y_scan > middle_vertex.y)
						{
							slope_value_left = slope_value_above_mid;
							b_value_left = b_value_above_mid;
						}
						if (Z < z_buffer[x_scan][y_scan])
					    {
					    	z_buffer[x_scan][y_scan] = Z;
					    	renderPixelColor(x_scan, y_scan, red_color, green_color, blue_color);
					    }	
					}			
				}
			}
			else if (highest_y_vertex.x == lowest_x_vertex.x)
			{
				if (x_scan >= (((double)y_scan - b_value_left) / slope_value_left) &&
					x_scan <= (((double)y_scan - b_value_right) / slope_value_right))
				{
					if ((x_scan > 0 && x_scan < WINDOW_WIDTH) && (y_scan > 0 && y_scan < WINDOW_HEIGHT))
					{
						if (y_scan > middle_vertex.y)
						{
							slope_value_right = slope_value_above_mid;
							b_value_right = b_value_above_mid;
						}
						if (Z < z_buffer[x_scan][y_scan])
					    {
					    	z_buffer[x_scan][y_scan] = Z;
				    		renderPixelColor(x_scan, y_scan, red_color, green_color, blue_color);
					    }		
					}		
				}
			}
			else if (highest_y_vertex.x == highest_x_vertex.x)
			{
				if (x_scan >= (((double)y_scan - b_value_left) / slope_value_left) &&
					x_scan <= (((double)y_scan - b_value_right) / slope_value_right))
				{
					if ((x_scan > 0 && x_scan < WINDOW_WIDTH) && (y_scan > 0 && y_scan < WINDOW_HEIGHT))
					{
						if (y_scan > middle_vertex.y)
						{
							slope_value_left = slope_value_above_mid;
							b_value_left = b_value_above_mid;
						}
						if (Z < z_buffer[x_scan][y_scan])
					    {
					    	z_buffer[x_scan][y_scan] = Z;
				    		renderPixelColor(x_scan, y_scan, red_color, green_color, blue_color);
					    }	
					}			
				}
			}
			else
			{
				if (x_scan >= (((double)y_scan - b_value_left) / slope_value_left) &&
					x_scan <= (((double)y_scan - b_value_right) / slope_value_right))
				{
					if ((x_scan > 0 && x_scan < WINDOW_WIDTH) && (y_scan > 0 && y_scan < WINDOW_HEIGHT))
					{
						if (Z < z_buffer[x_scan][y_scan])
					    {
					    	z_buffer[x_scan][y_scan] = Z;
				    		renderPixelColor(x_scan, y_scan, red_color, green_color, blue_color);
					    }				
					}
				}
			}			
			//Updates the Z buffer
			Z = Z - (A/C);
		}
	}
}

/**************************************************************************************
 * function drawWires
 *
 * Renders the lines of the triangles
 **************************************************************************************/
void drawWires(Triangle3D triangle)
{
	renderLine(pts_3D[triangle.p1].x, pts_3D[triangle.p1].y, pts_3D[triangle.p2].x, pts_3D[triangle.p2].y);
	renderLine(pts_3D[triangle.p1].x, pts_3D[triangle.p1].y, pts_3D[triangle.p3].x, pts_3D[triangle.p3].y);
	renderLine(pts_3D[triangle.p3].x, pts_3D[triangle.p3].y, pts_3D[triangle.p2].x, pts_3D[triangle.p2].y);
}

/**************************************************************************************
 * function rotateObject
 *
 * Rotates an object accordingly to the given parameter theta
 **************************************************************************************/
void rotateObject(double theta)
{
	//Goes through the vertecies to rotate each point accordingly
	for (int i = 0; i < points_length; ++i)
	{
		//Sets the x y z values accordingly with the averages
		//First moves it accordingly to start the rotation
		pts_3D[i].x = pts_3D[i].x - averages.x;
		pts_3D[i].y = pts_3D[i].y - averages.y;
		pts_3D[i].z = pts_3D[i].z - averages.z;

		//Rotates the object accordingly to the given theta
		double old_x = pts_3D[i].x;
		double old_z = pts_3D[i].z;
		pts_3D[i].x = pts_3D[i].x * cos(theta) + pts_3D[i].z * sin(theta);
		pts_3D[i].z = -(old_x * sin(theta)) + old_z * cos(theta);
		
		//Places object back into place
		pts_3D[i].x = pts_3D[i].x + averages.x;
 		pts_3D[i].y = pts_3D[i].y + averages.y;
 		pts_3D[i].z = pts_3D[i].z + averages.z;
	}
}

/**************************************************************************************
 * function translate_y
 *
 * Moves the object accordingly to the given parameter movement in the y axis.
 **************************************************************************************/
void translate_y(double movement)
{
	//Goes through the vertecies to move each point accordingly
	for (int i = 0; i < points_length; ++i)
	{
		pts_3D[i].y += movement;
	}
}

/**************************************************************************************
 * function translate_x
 *
 * Moves the object accordingly to the given parameter movement in the x axis.
 **************************************************************************************/
void translate_x(double movement)
{
	//Goes through the vertecies to move each point accordingly
	for (int i = 0; i < points_length; ++i)
	{
		pts_3D[i].x += movement;
	}
}

/**************************************************************************************
 * function scale_object
 *
 * Scales the object accordingly to the given parameter scale_value
 **************************************************************************************/
void scale_object(double scale_value)
{
	//Goes through the vertecies to scale each point accordingly
	for (int i = 0; i < points_length; ++i)
	{
		//Used to move object to scale accordingly
		pts_3D[i].x = pts_3D[i].x - averages.x;
		pts_3D[i].y = pts_3D[i].y - averages.y;
		pts_3D[i].z = pts_3D[i].z - averages.z;

		//Scales
		pts_3D[i].x = pts_3D[i].x * scale_value;
		pts_3D[i].y = pts_3D[i].y * scale_value;
		pts_3D[i].z = pts_3D[i].z * scale_value;
	
		//translate to original position
		pts_3D[i].x = pts_3D[i].x + averages.x;
		pts_3D[i].y = pts_3D[i].y + averages.y;
		pts_3D[i].z = pts_3D[i].z + averages.z;
	}
}

/**************************************************************************************
 * function GL_render
 *
 * Renders everything on the screen
 **************************************************************************************/
void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Go through every triangle in the list and draws them either in the fill triangles or wire triangles
	zBuffer();
	for (int i = 0; i < tri_3D.size(); ++i)
	{
		float red_color = ((rand() % 10) + 1) / 10.0;
    	float green_color = ((rand() % 10) + 1) / 10.0;
   		float blue_color = ((rand() % 10) + 1) / 10.0;
   		
   		//checks the wireframe mode
   		if (!wireframe_mode)
			fillTriangle(tri_3D[i],red_color,green_color,blue_color);
		else
			drawWires(tri_3D[i]);
	}

	glutSwapBuffers();
}

/**************************************************************************************
 * function parsing_inputs
 *
 * Parses the input file the user calls in as a parameter
 **************************************************************************************/
void parsing_inputs(string input_file)
{
	points_length = 0;
	triangles_length = 0;
	ifstream input;
	input.open(input_file);

	if (input.is_open())
	{
		input >> points_length;
		cout << endl << "Extracting points length" << endl;
		input >> triangles_length;
		cout << "Extracting triangles length" << endl;

		double x;
		double y;
		double z;

		double avg_X = 0.0;
		double avg_Y = 0.0;
		double avg_Z = 0.0;

		cout << endl << "=============Points=============";
		for (int i = 0; i < points_length; ++i)
		{
			input >> x;
			input >> y;
			input >> z;

			avg_X += x;
			avg_Y += y;
			avg_Z += z;

			cout << endl << i << endl 
							<< "Extracting x: " << x << endl
				 			<< "Extracting y: " << y << endl
							<< "Extracting z: " << z << endl;

			Point3D new_point = Point3D(x,y,z);
			pts_3D.push_back(new_point);
		}

		//Initializes the averages used for moving/rotating/scaling
		averages = Point3D(avg_X/points_length, avg_Y/points_length, avg_Z/points_length);
		double p1;
		double p2;
		double p3;

		cout << endl << "=============Triangles=============";
		for (int i = 0; i < triangles_length; ++i)
		{
			input >> p1;
			input >> p2;
			input >> p3;
			cout << endl << i << endl 
							<< "Extracting x: " << p1 << endl
				 			<< "Extracting y: " << p2 << endl
							<< "Extracting z: " << p3 << endl;

			Triangle3D new_tri = Triangle3D(p1,p2,p3);
			tri_3D.push_back(new_tri);
		}
		input.close();
	}

	cout << endl << endl << "========================================================================" << endl
						 <<	"A D - Controls the rotation of the object" << endl
						 << "W S - Controls the scaling of the object" << endl
						 << "Up, Down, Left, Right - Controls the translation of the object" << endl
						 << "Space Bar - Changing modes from wire to filled triangles and vice versa" << endl
						 << "========================================================================" << endl << endl;
}

/**************************************************************************************
 * function GLInit
 *
 * Inits OpenGL and all the necessary variables
 **************************************************************************************/
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CS 130 - <Randy Truong>");

	//Our functions used for manipulating the screen
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_directions);
	wireframe_mode = false;

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

	//Needed to display the screen
	glutDisplayFunc(GL_render);
}

/**************************************************************************************
 * function main
 *
 * the main function
 **************************************************************************************/
int main(int argc, char** argv)
{	
	if (argc < 2)
	{
		cerr << "Please input correct arguments with File" << endl;
		return -1;
	}
	else
	{
		parsing_inputs(argv[1]);
	}
	GLInit(&argc, argv);
	glutMainLoop();
	return 0;
}


