#include "application.h"

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "matrix.h"
#include "transform.h"

struct particle
{
    double mass;
    matrix<3,1> position;
    matrix<3,1> prev_position;
    matrix<3,1> velocity;
    matrix<3,1> prev_velocity;
    matrix<3,1> force;
    matrix<3,1> color;

    particle()
    {
	mass = 1;
	set_color(1.0f, 1.0f, 1.0f);
	prev_position.set(0, 0, 0.0f);
	prev_position.set(1, 0, 0.0f);
	prev_position.set(2, 0, 0.0f);
	set_pos(0.0f, 0.0f, 0.0f);

	prev_velocity.set(0, 0, 0.0f);
	prev_velocity.set(1, 0, 0.0f);
	prev_velocity.set(2, 0, 0.0f);
	set_velocity(0.0f, 0.0f, 0.0f);

	set_force(0.0f, 0.0f, 0.0f);
    }

    void set_color(float red, float green, float blue)
    {
	color.set(0, 0, red);
	color.set(1, 0, green);
	color.set(2, 0, blue);
    }

    void set_pos(float x, float y, float z)
    {
	prev_position.set(0, 0, position.get(0, 0));
	prev_position.set(1, 0, position.get(1, 0));
	prev_position.set(2, 0, position.get(2, 0));
	position.set(0, 0, x);
	position.set(1, 0, y);
	position.set(2, 0, z);
    }

    void set_velocity(float x, float y, float z)
    {
	prev_velocity.set(0, 0, velocity.get(0, 0));
	prev_velocity.set(1, 0, velocity.get(1, 0));
	prev_velocity.set(2, 0, velocity.get(2, 0));
	velocity.set(0, 0, x);
	velocity.set(1, 0, y);
	velocity.set(2, 0, z);
    }

    void set_force(float x, float y, float z)
    {
	force.set(0, 0, x);
	force.set(1, 0, y);
	force.set(2, 0, z);
    }
};

enum { NONE, AMBIENT, DIFFUSE, SPECULAR, NUM_MODES };

particle Particles[5000];

void draw_grid(int dim);
void draw_obj(obj *o, const gl_image_texture_map& textures);

void set_pixel(int x, int y, float col[3])
{
    // write a 1x1 block of pixels of color col to framebuffer
    // coordinates (x, y)
    //glRasterPos2i(x, y);
    //glDrawPixels(1, 1, GL_RGB, GL_FLOAT, col);
    
    // use glVertex instead of glDrawPixels (faster)
    glBegin(GL_POINTS);
    glColor3fv(col);
    glVertex2f(x, y);
    glEnd();
}

application::application() 
    : raytrace(false), rendmode(SPECULAR), paused(false), sim_t(0.0)
{
}

application::~application()
{
}

// triggered once after the OpenGL context is initialized
void application::init_event()
{

    cout << "CAMERA CONTROLS: \n  LMB: Rotate \n  MMB: Pan \n  LMB: Dolly" << endl;
    cout << "KEYBOARD CONTROLS: \n  ' ': Pause simulation" << endl;

    // initialilze the particles
    for(unsigned int i = 0; i < 5000; i++)
    {
	particle p;
	Particles[i] = p;
	Particles[i].set_velocity((rand() % 10) - 5, rand() % 5, (rand() % 10) - 5);
	Particles[i].set_force(0.0, Particles[i].mass * -9.81, 0.0);
    }

    const GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    const GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    const GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    // enable a light
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);
    
    // set global ambient lighting
    GLfloat amb[] = { 0.4, 0.4, 0.4, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    
    // enable depth-testing, colored materials, and lighting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    
    // normalize normals so lighting calculations are correct
    // when using GLUT primitives
    glEnable(GL_RESCALE_NORMAL);
    
    // enable smooth shading
    glShadeModel(GL_SMOOTH);

    glClearColor(0,0,0,0);
    
    // set the cameras default coordinates
    camera.set_distance(6);
    camera.set_elevation(35);
    camera.set_twist(45);

    t.reset();
    o.load("crater.obj");
    
    // loads up all the textures referenced by the .mtl file
    const obj::string_to_material_map& mats = o.get_materials();
    for (obj::string_to_material_map::const_iterator i = mats.begin(); 
        i != mats.end(); ++i
        )
    {
        if (!i->second.map_kd.empty()) {
            string filename = i->second.map_kd;

            // add texture if we have not already loaded it
            if (texs.find(filename) == texs.end()) {
                gl_image_texture *tex = new gl_image_texture();
                if (tex->load_texture(filename) != SUCCESS) {
                    cout << "could not load texture file: " << filename << endl;
                    exit(0);
                }
                texs[filename] = tex;
            }
        }
    }
}

// triggered each time the application needs to redraw
void application::draw_event()
{
    camera.apply_gl_transform();

    const GLfloat light_pos1[] = { 0.0, 10.0, 0.0, 1 };   
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);

    if (!paused) {
        //
        // DO SIM STUFF HERE.
        //
        // You'll want to simulate for 1/60th of a second.
        // That does not mean your step size should be
        // 1/60th. Simply that if you integrate multiple
        // times within this draw_event, 
        // h * #of integration steps = 0.015
        //
    }

    glLineWidth(2.0);
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_LINES);
        //
        //
        // DRAW YOUR PARTICLE USING GL_LINES HERE
    for(unsigned int i = 0; i < 5000; i++)
    {
	particle p = Particles[i];
        glVertex3f(p.position.get(0,0), p.position.get(1,0), p.position.get(2,0));
        glVertex3f(p.position.get(0,0) + 0.015f * p.velocity.get(0,0), p.position.get(1,0) +  0.015f * p.velocity.get(1,0), p.position.get(2,0) +  0.015f * p.velocity.get(2,0));
	p.set_pos(p.position.get(0,0) + 0.015f * p.velocity.get(0,0), p.position.get(1,0) +  0.015f * p.velocity.get(1,0), p.position.get(2,0) +  0.015f * p.velocity.get(2,0));
	if(p.position.get(1,0) == 0.0f)
	{
	    //TODO
	}
	p.set_velocity(p.velocity.get(0,0) + (0.015f/p.mass)*p.force.get(0,0), p.velocity.get(1,0) + (0.015f/p.mass)*p.force.get(1,0), p.velocity.get(2,0) + (0.015f/p.mass)*p.force.get(2,0));
	Particles[i] = p;
    }
        //
        //
    glEnd();

    // draw the volcano
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glScalef(0.2,0.3,0.2);
    draw_obj(&o, texs);
    glPopMatrix();
    glDisable(GL_LIGHTING);


    glColor3f(0.15, 0.15, 0.15);
    draw_grid(40);

    //
    // This makes sure that the frame rate is locked to close to 60 fps. 
    // For each call to draw_event you will want to run your integrate for 0.015s
    // worth of time.
    //
    float elap = t.elapsed();
    if (elap < 0.015) {
        usleep(1e6*(0.015-elap));
    }
    t.reset();
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

    if (key == 'r') {
        sim_t = 0;
    } else if (key == ' ') {
        paused = !paused;
    } else if (key == 'q') {
        exit(0);
    }
}

void draw_grid(int dim)
{
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
}

void draw_obj(obj *o, const gl_image_texture_map& textures)
{
    glDisable(GL_COLOR_MATERIAL);

    // draw each polygon of the mesh
    size_t nfaces = o->get_face_count();
    for (size_t i = 0; i < nfaces; ++i) 
    {
        const obj::face& f = o->get_face(i);

        // sets the material properties of the face
        const obj::material* mat = o->get_material(f.mat);
        if (mat) {
            if (!mat->map_kd.empty()) {
                gl_image_texture_map::const_iterator it = textures.find(mat->map_kd);
                if (it != textures.end()) {
                    gl_image_texture* tex = it->second;
                    tex->bind();
                }
                GLfloat mat_amb[] = { 1, 1, 1, 1 };
                GLfloat mat_dif[] = { mat->kd[0], mat->kd[1], mat->kd[2], 1 };
                GLfloat mat_spec[] = { mat->ks[0], mat->ks[1], mat->ks[2], 1 };
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            } else {
                GLfloat mat_amb[] = { mat->ka[0], mat->ka[1], mat->ka[2], 1 };
                GLfloat mat_dif[] = { mat->kd[0], mat->kd[1], mat->kd[2], 1 };
                GLfloat mat_spec[] = { mat->ks[0], mat->ks[1], mat->ks[2], 1 };
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            }
            glMaterialf(GL_FRONT, GL_SHININESS, mat->ns);
            
            if (!glIsEnabled(GL_TEXTURE_2D)) glEnable(GL_TEXTURE_2D);
        } else {
            GLfloat mat_amb[] = { 0.5, 0.5, 0.5, 0.5 };
            GLfloat mat_dif[] = { 0.5, 0.5, 0.5, 0.5 };
            GLfloat mat_spec[] = { 0.1, 0.1, 0.1, 1 };

            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            glMaterialf(GL_FRONT, GL_SHININESS, 20);
            if (glIsEnabled(GL_TEXTURE_2D)) glDisable(GL_TEXTURE_2D);
        }


        // draws a single polygon
        glBegin(GL_POLYGON);
        for (size_t j = 0; j < f.vind.size(); ++j) 
        {
            // vertex normal
            if (f.nind.size() == f.vind.size()) {
                const float *norm = o->get_normal(f.nind[j]);
                glNormal3fv(norm);
            }

            // vertex UV coordinate
            if (f.tex.size() > 0) {
                const float* tex = o->get_texture_indices(f.tex[j]);
                glTexCoord2fv(tex);
            }

            // vertex coordinates
            const float *vert = o->get_vertex(f.vind[j]);
            glVertex3fv(vert);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
}
