// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

// Shaders loaded into memory as character arrays
const GLchar* vertexSource =
"#version 430 core\n"
"in vec3 position;"
"in vec3 color;"
"out vec3 Color;"
"void main() {"
"   Color = color;"
"   gl_Position = vec4(position, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 430 core\n"
"in vec3 Color;"
"out vec4 outColor;"
"void main() {"
"   outColor = vec4(Color, 1.0);"
"}";

int main()
{
	//Initialize GLFW
	//GLFW is a library responsible for creating windows, taking in user input,
	//and OpenGL context creation
	//For more info: http://www.glfw.org/
	glfwInit();

	//Set OpenGL version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Set window size to be static
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "CS130 Lab1", 0, 0); // Windowed

	//Attach OpenGL context to window
	glfwMakeContextCurrent(window);

	//GLEW is an OpenGL loading library: http://glew.sourceforge.net/
	//It loads the available OpenGL functions depending on hardware and driver version

	//On why glewExperimental is set to true:
	//https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW_.28OpenGL_Extension_Wrangler.29
	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK)
		cout << "glewInit failed." << endl;

	cout << "Calling glewInit creates GL error: " << glGetError() << endl;
	cout << "Can be ignored for now and this will reset the error flag" << endl;


	//opengl code
	//Create Vertex Array Object and make it active
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo[1];
	glGenBuffers(1, vbo);

	GLfloat vertices[8712];
	ifstream model;

	model.open("monkey.raw", ifstream::in);
	for(unsigned int i = 0; i < 8712; i++){
			model >> vertices[i];
			vertices[i]/=2;
	}
	model.close();
	/*(GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};*/

	//Make active buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char shader_err_buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, shader_err_buffer);
		cout << shader_err_buffer << endl;
	}

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	if (status != GL_TRUE) {
		char shader_err_buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, shader_err_buffer);
		cout << shader_err_buffer << endl;
	}

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Specify the layout of the color data
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	//Event loop 
	//Handles user input and rendering 
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen to black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw a triangle from the 3 vertices
		glDrawArrays(GL_TRIANGLES, 0, 8712);

		//GLFW has a front and back buffer to prevent screen tearing
		//Front buffer is the one being displayed
		//Back buffer is the one being rendered to
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

	}

	//Clean up
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, vbo);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();
	return 0;
}

