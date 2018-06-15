#include "application.h"

#include <iostream>
#include <cassert>

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "obj.h"
#include "quaternion.h"

void draw_grid(int dim);
void draw_obj(obj *o);
    
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

    const GLfloat ambient[] = { 0.50, 0.50, 0.50, 1.0 };
    const GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
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

    glClearColor(0.9, 0.9, 1.0, 0.0);

    // set the cameras default coordinates
    camera.set_distance(20);
    camera.set_elevation(35);
    camera.set_twist(45);

    t.reset();

    o.load("fighter.obj");
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
    glColor3f(.60, .60, .60);
    draw_grid(50);

    // draws the plane
    glPushMatrix();
    glTranslatef(0, 1, 0);
    //glRotatef(90*t.elapsed(), 0, 1, 0);
    quaternion p0(45, 1, 0, 1);
    quaternion p1(30, 0, 1, 0);
    quaternion p2;
    p2.slerp(min(t.elapsed()/5, 1.0f), p0, p1);
    float angle, x, y, z;
    p2.to_angle_axis(angle, x, y, z);
    glRotatef(angle, x, y, z);
    glScalef(2.5, 2.5, 2.5);
    draw_obj(&o);
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
    {
        solid = !solid;
        if (solid) {
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
        }
    }
    else if (key == 'r')
    {
        t.reset();
    }
}

void draw_grid(int dim)
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);

    
    //
    // Draws a grid along the x-z plane
    //
    glLineWidth(1.0);
    glBegin(GL_LINES);

    int ncells = dim;
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
    
    //
    // Draws the coordinate frame at origin
    //
    glPushMatrix();
    glScalef(1.0, 1.0, 1.0); 
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
    glEnable(GL_LIGHTING);
}

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
