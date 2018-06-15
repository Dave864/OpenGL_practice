#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h> //OpenGL
#include <GL/glext.h> //OpenGL extensions
#include <GL/glu.h> //Extra utilities for opengl
#include <SDL/SDL.h> //windowing stuff

#include "obj.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
using namespace std;



void draw_obj(obj *o)
{
    glDisable(GL_COLOR_MATERIAL);
    size_t nfaces = o->get_face_count();
    for (size_t i = 0; i < nfaces; ++i) 
    {
        const obj::face& f = o->get_face(i);

        glPushMatrix();
        if (f.mat != "none") {

            const obj::material& mat = o->get_material(f.mat);

            GLfloat mat_amb[] = { mat.ka[0], mat.ka[1], mat.ka[2], 1 };
            GLfloat mat_dif[] = { mat.kd[0], mat.kd[1], mat.kd[2], 1 };
            GLfloat mat_spec[] = { mat.ks[0], mat.ks[1], mat.ks[2], 1 };
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            glMaterialf(GL_FRONT, GL_SHININESS, mat.ns);
        }


        glBegin(GL_POLYGON);
        for (size_t j = 0; j < f.vind.size(); ++j) 
        {
            if (f.nind.size() == f.vind.size()) {
                const float *norm = o->get_normal(f.nind[j]);
                glNormal3fv(norm);
            }

            const float *vert = o->get_vertex(f.vind[j]);
            glVertex3fv(vert);
        }
        glEnd();
        glPopMatrix();
    }

    glEnable(GL_COLOR_MATERIAL);
}


string getTextFile(const char* name)
{
	string ret;
	int c = 0;
	FILE* file = fopen(name, "rb");
	while(c != EOF)
	{
		c = fgetc(file);
		if(c != EOF)
			ret += c;
	}
	fclose(file);
	return ret;	
}

//Call this to debug your shader. This will give you error messages when there is something wrong
void debugShader(GLuint vertexShader, GLuint fragmentShader, GLuint shaderProgram)
{
	char vertexInfoLog[1024];
	char fragmentInfoLog[1024];
	char programInfoLog[1024];
	glGetShaderInfoLog(vertexShader, 1024, NULL, vertexInfoLog);
	glGetShaderInfoLog(fragmentShader, 1024, NULL, fragmentInfoLog);
	glGetProgramInfoLog(shaderProgram, 1024, NULL, programInfoLog);
	cout << vertexInfoLog << endl;
	cout << fragmentInfoLog << endl;
	cout << programInfoLog << endl;
}

GLuint loadShader(const char* vertexfilename, const char* fragmentfilename)
{
	string vertexFile = getTextFile(vertexfilename);
	string fragmentFile = getTextFile(fragmentfilename);

	GLuint shaderProgram = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* c = &vertexFile[0];
	glShaderSource(vertexShader, 1, (const GLchar**)&c, NULL);
	glCompileShader(vertexShader);
	glAttachShader(shaderProgram, vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	c = &fragmentFile[0];
	glShaderSource(fragmentShader, 1, (const GLchar**)&c, NULL);
	glCompileShader(fragmentShader);
	glAttachShader(shaderProgram, fragmentShader);	

	glLinkProgram(shaderProgram);

	debugShader(vertexShader, fragmentShader, shaderProgram);

	//Functions you should know to implement this
	//glCreateShader
	//glShaderSource
	//glCompileShader
	//glAttachShader
	//glCreateProgram
	//glLinkProgram
	//http://www.opengl.org/sdk/docs/man/
	//http://www.lighthouse3d.com/tutorials/glsl-tutorial/creating-a-shader/
	//You are provided with the function getTextFile to load a string from a text file

	return shaderProgram;
}






//Here is some boilerplate code. It's not the most object oriented thing in the
//world but it should get you started. Feel free to use and modify.
int main(int nArgs, char** args)
{
	//Initialize the window
	//Setup SDL: http://www.libsdl.org/cgi/docwiki.cgi/SDL_API_by_name
	SDL_Init(SDL_INIT_VIDEO);
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	//setting up 24-bit framebuffer: http://en.wikipedia.org/wiki/Framebuffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //8 bits for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); //8 bits for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); //8 bits for blue

	//enable double buffering: http://en.wikipedia.org/wiki/Page_flipping
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //enable page flipping

	//Create a 800 by 600 window with 32 bits per pixel and uses OpenGL
	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

	SDL_WM_SetCaption("CS130 Lab", NULL); //Set the window caption



	//Set up Opengl stuff
	//http://www.opengl.org/sdk/docs/man/

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION); //This is like setting up our camera lens (all future matrix stuff will modify our "camera lens")
	gluPerspective(45, 4.0/3.0, 1, 100); //field of view, aspect ratio, near plane and far plane(aka min and max view distance)

	glMatrixMode(GL_MODELVIEW); //set it back to modelview (this is for setting the matrix transformations for all of our objects



	//Extra variables for main loop
	double x = 0;
	double y = 0;
	float lightAngle = 0;

	obj mesh;
	mesh.load("monkey.obj");


	//Main loop
	GLuint program = loadShader("vertex.glsl", "fragment.glsl");
	glUseProgram(program);
	while(true)
	{
		float tick = SDL_GetTicks(); //for measuring framerate

		////////////////////////////////////////////////////////////////////
		//LOGIC
		///////////////////////////////////////////////////////////////////
		//Update platform specific stuff
		SDL_Event event;
		SDL_PollEvent(&event); //Get events
		if(event.type == SDL_QUIT) //if you click the [X] exit out
			break;

		//Handle some input
		int mouseX, mouseY;
		char buttons = SDL_GetRelativeMouseState(&mouseX, &mouseY);
		bool leftClick = buttons & SDL_BUTTON(1);
		bool rightClick = buttons & SDL_BUTTON(3);

		//Deal with the input
		const float sensitivity = 0.25;
		if(leftClick)
		{
			x += mouseX*sensitivity;
			y += mouseY*sensitivity;
		}
		if(rightClick)
		{
			lightAngle += mouseX*sensitivity;
		}


		////////////////////////////////////////////////////////////
		//GRAPHICS
		////////////////////////////////////////////////////////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the color buffer and the depth buffer

		// http://fly.srk.fer.hr/~unreal/theredbook/chapter03.html <---information regarding viewing/transformation
		//setup the camera
		glLoadIdentity(); //Load the identity matrix
		gluLookAt(0, 0, 5, //camera position
			  0, 0, 0, //target position(where the camera is looking)
		          0, 1, 0); //up vector(where is up)

		//Enable the light after the camera
		glPushMatrix();
			glRotatef(lightAngle, 0,1,0);
			float position_0[] = {1, 0.5, 5, 1.0};
			float diffuse_0[] = {0.0, 1.0, 0.0, 1.0};
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_0);
			glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse_0);
			glLightfv(GL_LIGHT0, GL_POSITION, position_0);
			glEnable(GL_LIGHT0);

			float position_1[] = {-1, 0.5, 5 , 1.0};
			float diffuse_1[] = {1.0, 0.0, 0.0, 1.0};
			glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_1);
			glLightfv(GL_LIGHT1, GL_SPECULAR, diffuse_1);
               		glLightfv(GL_LIGHT1, GL_POSITION, position_1);
			glEnable(GL_LIGHT1);
		glPopMatrix();

		//draw the model
		glPushMatrix();
			glRotatef(x, 0,1,0); //angle, followed by the axis of rotation
			glRotatef(y, 1,0,0); //angle, followed by the axis of rotation

			draw_obj(&mesh);
		glPopMatrix();



		//Swap the buffers(yes we are using double buffering)
		//To avoid this: http://en.wikipedia.org/wiki/Screen_tearing
		//We are doing this: http://en.wikipedia.org/wiki/Multiple_buffering
		//otherwise we just use single buffering and we won't need this function call
		SDL_GL_SwapBuffers(); //Finished drawing

		//Display framerate
		float fps = 1000.0/(SDL_GetTicks() - tick);
		stringstream ss;
		ss << fps;
		if(rand()%1000 == 0) //hack to display the framerate every now and then
			SDL_WM_SetCaption( (string("CS 130 Lab - FPS:") + ss.str()).c_str(), NULL);
	}



	//Exit out
	SDL_Quit();
	return 0;
}
