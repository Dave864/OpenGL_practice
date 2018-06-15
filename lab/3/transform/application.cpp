#include "application.h"

#include <iostream>
#include <cassert>
#include "vector3.h"

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

void draw_grid();
    
application::application() 
    : solid(true)
{
}

application::~application()
{
}

// triggered once after the OpenGL context is initialized
void application::init_event()
{

    cout << "CAMERA CONTROLS: \n  LMB: Rotate \n  MMB: Pan \n  LMB: Dolly" << endl;
    cout << "KEYBOARD CONTROLS: \n  '=': Toggle wireframe mode" << endl;

    const GLfloat ambient[] = { 0.15, 0.15, 0.15, 1.0 };
    const GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
    const GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    // enable a light
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);
    
    // enable depth-testing, colored materials, and lighting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    
    // normalize normals so lighting calculations are correct
    // when using GLUT primitives
    glEnable(GL_NORMALIZE);
    
    // enable smooth shading
    glShadeModel(GL_SMOOTH);

    // set the cameras default coordinates
    camera.set_distance(20);
    camera.set_elevation(35);
    camera.set_twist(45);

    t.reset();
}

// triggered each time the application needs to redraw
void application::draw_event()
{
    // apply our camera transformation
    camera.apply_gl_transform();
    
    // set the position of the light
    const GLfloat light_pos1[] = { 0.0, 10.0, 0.0, 1 };   
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
    
    // draws the grid and frame at the origin
    draw_grid();

    //
    // create some various objects in the world
    //

    glPushMatrix();
    glColor3f(1, 1, 0);
    glRotatef(t.elapsed()*100, 0, 1, 0);
    solid ? glutSolidCube(2) : glutWireCube(2);

        glPushMatrix();
        glColor3f(1, .5, 0);
        glTranslatef(6, 0, 0);
        solid ? glutSolidSphere(1, 10, 10) : glutWireSphere(1, 10, 10);
    
            glPushMatrix();
            glColor3f(1, .5, 0);
            glRotatef(90, 1, 0, 0);
            glRotatef(t.elapsed()*90, 0, -1, 0);
            glScalef(1, 1, .10);
            solid ? glutSolidTorus(1, 1.5, 10, 10) : glutWireTorus(1, 1.5, 10, 10);
            glPopMatrix();

        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 1);
        glTranslatef(10, 0, 0);
        glRotatef(180*t.elapsed(), 1, 0, 0);
        solid ? glutSolidSphere(.5, 5, 5) : glutWireSphere(.5, 5, 5);

            glPushMatrix();
            glColor3f(0, 1, 1);
            glTranslatef(0, 0, 1);
            solid ? glutSolidCone(.5, 1, 10, 10) : glutWireCone(.5, 1, 10, 10);
            glPopMatrix();
    
            glPushMatrix();
            glColor3f(0, 1, 1);
            glTranslatef(0, 0, -1);
            glRotatef(180, 1, 0, 0);
            solid ? glutSolidCone(.5, 1, 10, 10) : glutWireCone(.5, 1, 10, 10);
            glPopMatrix();

        glPopMatrix();

    glPopMatrix();
}

// triggered when mouse is clicked
void application::mouse_click_event(
    mouse_button button, mouse_button_state button_state, 
    int x, int y
    )
{
}
    
// triggered when mouse button is held down and the mouse is
// moved
void application::mouse_move_event(
    int x, int y
    )
{
}

// triggered when a key is pressed on the keyboard
void application::keyboard_event(unsigned char key, int x, int y)
{
    if (key == '=')
        solid = !solid;
}

void draw_grid()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);

    //
    // Draws the coordinate frame at origin
    //
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5); 
    glBegin(GL_LINES);

    // x-axis
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    
    // y-axis
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    
    // z-axis
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();
    
    //
    // Draws a grid along the x-z plane
    //
    glLineWidth(1.0);
    glColor3f(.20, .20, .20);
    glBegin(GL_LINES);

    int ncells = 20;
    int ncells2 = ncells/2;

    for (int i= 0; i <= ncells; i++)
    {
        int k = -ncells2;
        k +=i;
        glVertex3f(ncells2,0,k);
        glVertex3f(-ncells2,0,k);
        glVertex3f(k,0,ncells2);
        glVertex3f(k,0,-ncells2);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}
