#include "gl_viewer.h"

#include <cassert>
#include <iostream>

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

gl_viewer* gl_viewer::singleton = NULL; 

gl_viewer::gl_viewer()
{
    assert(!singleton); // ensure only one instance is created
    singleton = this;
}

gl_viewer::~gl_viewer()
{
    singleton = NULL;
}

void gl_viewer::init(int argc, char *argv[], int width, int height)
{
    // set up glut
    glutInit(&argc, argv);

    // create a 24-bit double-buffered window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); 

    glutInitWindowSize(width, height);
    glutCreateWindow("CS130 DDA Lab Assignment");

    // called when Glut needs to display
    glutDisplayFunc(&gl_viewer::glut_display_event_wrapper);

    // called when Glut has detected a mouse click
    glutMouseFunc(&gl_viewer::glut_mouse_click_event_wrapper);
    
    // called when Glut has detected mouse motion
    glutMotionFunc(&gl_viewer::glut_mouse_move_event_wrapper);

    // called when Glut has key input
    glutKeyboardFunc(&gl_viewer::glut_keyboard_event_wrapper);

    // called when the window is resized
    glutReshapeFunc(&gl_viewer::glut_reshape_event_wrapper);

    // clear our background to black when glClear is called
    glClearColor(0.0, 0.0, 0.0, 0); 

    this->width = width;
    this->height = height;
}

void gl_viewer::run()
{
    // pass execution to Glut. Now Glut is running the main loop.
    glutMainLoop();
}

void gl_viewer::glut_display_event_wrapper()
{
    // clear our back color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // call our gl_viewers draw event and render to back buffer
    singleton->draw_event();

    // swap the back buffer with the front (user always sees front
    // buffer on display)
    glutSwapBuffers();
}

void gl_viewer::glut_mouse_click_event_wrapper(
    int button, int state, int x, int y
    )
{
    // normally (0,0) is at the top left of screen. Since this is
    // somewhat unintuitive, it has been changed to be the bottom
    // left
    singleton->mouse_click_event(
        (mouse_button)button, (mouse_button_state)state, 
        x, singleton->height - y
    );

    // if a button is pressed down or released, force a redraw
    glutPostRedisplay();
}

void gl_viewer::glut_mouse_move_event_wrapper(
    int x, int y
    )
{
    // normally (0,0) is at the top left of screen. Since this is
    // somewhat unintuitive, it has been changed to be the bottom
    // left
    singleton->mouse_move_event(
        x, singleton->height - y
    );
    
    glutPostRedisplay();
}

void gl_viewer::glut_keyboard_event_wrapper(unsigned char key, int x, int y)
{
    singleton->keyboard_event(key, x, y);
    
    // force a redraw
    glutPostRedisplay();
}

void gl_viewer::glut_reshape_event_wrapper(int width, int height)
{
    singleton->width = width;
    singleton->height = height;

    // adjust the view frustrum and viewport 
    // to reflect the new window dimensions
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();
}

