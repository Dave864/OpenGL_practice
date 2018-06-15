#include <GL/gl.h>
#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;


// Classes
class vector3
{
public:
	vector3(){}
	vector3(double x, double y) : x(x), y(y), z(0) {}
	vector3(double x, double y, double z) : x(x), y(y), z(z) {}
	vector3 operator+(vector3 v){ return vector3(x+v.x, y+v.y, z+v.z); }
	vector3 operator-(vector3 v){ return vector3(x-v.x, y-v.y, z-v.z); }
	vector3 operator*(double s){ return vector3(s*x, s*y, s*z); }
	bool operator!=(vector3 v){ return (x!=v.x or y!=v.y or z!=v.z); }	
	bool operator==(vector3 v){ return (x==v.x and y==v.y and z==v.z); }
public:
	double x,y,z;
};
//vector3 operator-(vector3 v) {return vector3(0,0,0) - v;}
vector3 operator/(vector3 v, double s) {return v*(1.0/s);}
vector3 operator*(double s, vector3 v) {return v*s;}
double dot(vector3 a, vector3 b) { return (a.x*b.x + a.y*b.y + a.z*b.z); }
vector3 normal(vector3 v) { return (1.0/sqrt(dot(v,v)))*v; }
template<class T> T square(T a) { return a*a; }



//Some defines
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

const int XMIN = -SCREEN_WIDTH/2;
const int XMAX = SCREEN_WIDTH/2;
const int YMIN = -SCREEN_HEIGHT/2;
const int YMAX = SCREEN_HEIGHT/2;



//Checks for intersections between a ray and a sphere
double intersections(vector3 rayDir, vector3 rayStart, vector3 center, double radius)
{
	vector3 rayLen = rayStart - center;
	return square(dot(rayDir, rayLen)) - dot(rayLen, rayLen) + square(radius);
}

//Calculates the position of the first intersection of a ray with a sphere
vector3 rayStopPos(vector3 rayDir, vector3 rayStart, vector3 center, double radius)
{
	double distance = -sqrt(intersections(rayDir, rayStart, center, radius));
	distance -= dot(rayDir,(rayStart - center));
	return rayStart + distance*rayDir;
}

//Plots a point
void plot(int x, int y, float r, float g, float b)
{
	glBegin(GL_POINTS);
		glColor3f(r,g,b);
		glVertex2i(x,y);
	glEnd();
}



//Main Function
int main(int nArgs, char** args)
{
	//Initialize the window
	SDL_Init(SDL_INIT_VIDEO);
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //8 bits for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); //8 bits for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); //8 bits for blue
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //enable page flipping
	SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32,SDL_OPENGL);
	SDL_WM_SetCaption("CS130 Lab", NULL);

	//Set up the projection - don't worry about this
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(XMIN, XMAX, YMIN, YMAX,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vector3 spherePosition(0,0,30);
	double sphereRadius=10;
	vector3 lightPosition(-20,-10,-10);
	vector3 pixelgridCenter(0,0,5);
	vector3 cameraPos(0,0,0);
	vector3 ambientColor(.2f,.2f,.2f);
	vector3 diffuseColor(.5f,.5f,.5f);
	vector3 specularColor(.5f,.5f,.5f);
	vector3 x_incr(.025,0,0);
	vector3 y_incr(0,.025,0);

	while(true)
	{
		//Update platform specific stuff
		SDL_Event event;
		SDL_PollEvent(&event); //Get events
		if(event.type == SDL_QUIT) //if you click the [X] exit out
			break;
		//Graphics
		glClear(GL_COLOR_BUFFER_BIT); //Clear the screen
		for(int x = XMIN; x <= XMAX; ++x)
		for(int y = YMIN; y <= YMAX; ++y)
		{
		    vector3 color(0,0,0); // fill this in with the appropriate colors
		    //cast ray
		    vector3 rayDir = pixelgridCenter+x*x_incr+y*y_incr;
		    rayDir = normal(rayDir);
		    //check for intersections
		    if(intersections(rayDir, cameraPos, spherePosition, sphereRadius) >= 0)
		    {
		    	//if(intersection) set pixel to object color
		    	double c_x, c_y, c_z;
			vector3 rayBounce = rayStopPos(rayDir, cameraPos, spherePosition, sphereRadius);
			vector3 toLight = normal(lightPosition - rayBounce);
			vector3 norm = (rayBounce - spherePosition)/sphereRadius;
			c_x = ambientColor.x + diffuseColor.x*max(0.0, dot(toLight, norm)) + specularColor.x;
			c_y = ambientColor.y + diffuseColor.y*max(0.0, dot(toLight, norm)) + specularColor.y;
			c_z = ambientColor.z + diffuseColor.z*max(0.0, dot(toLight, norm)) + specularColor.z;
		    	color = vector3(c_x, c_y, c_z);
		    }
		    //else set pixel to background
		    plot(x,y,color.x,color.y,color.z);
		    // to help with the math:
		    // http://www.csee.umbc.edu/~olano/435f02/ray-sphere.html	
		}

		SDL_GL_SwapBuffers(); //Finished drawing
	}



	//Exit out
	SDL_Quit();
	return 0;
}
