#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>

#include "gl_viewer.h"
#include "obj.h"
#include "timer.h"

class line_segment
{
public:
    line_segment(int x0, int y0, int x1, int y1) 
        : x0(x0), y0(y0), x1(x1), y1(y1)
    {}

    int x0, y0, x1, y1;
}; 

class application : public gl_viewer
{
public:
    application();
    ~application();
    void init_event();
    void draw_event();
    void mouse_click_event(
        mouse_button button, mouse_button_state button_state, 
        int x, int y
    );
    void mouse_move_event(
        int x, int y
    );
    void keyboard_event(unsigned char key, int x, int y);

private:
    bool solid;
    timer t;
    obj o;
}; 

#endif
