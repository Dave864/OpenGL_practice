// Name: 
// Quarter, Year: 
// Lab:
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <vector>
#include <math.h>
#include "point2d.h"
#include <iostream>
#include <vector>

using namespace std;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
unsigned int clickCnt = 0;
vector <Point2D> controlPoints;

double coeff(unsigned int k, unsigned int n)
{
   double result = 1;
   for(unsigned int i = 1; i <= k; i++)
   {
      result *= (n + 1 -i)/i;
   }
   return result;
}

void Bezier_render()
{
  Point2D point0, point1;
  unsigned int n = controlPoints.size()-1;
  glBegin(GL_LINE_STRIP);
  glColor3f(1.0f,0.0f,0.0f);  
  for(double t = 0.0; t < 1; t += 0.01)
  {
     //point0 = controlPoints[0]*(1-t) + controlPoints[1]*t;
     //point1 = controlPoints[1]*(1-t) + controlPoints[2]*t;
     //pointF = point0*(1-t) + point1*t;
     //glVertex2d(pointF.x, pointF.y);
     Point2D pointF;
     for(unsigned int i = 0; i <= n; i++) 
     {
	pointF += controlPoints[i] * coeff(i, n) * pow((1 - t), (n - i)) * pow(t, i);
     }
     glVertex2d(pointF.x, pointF.y);
  }
  glEnd();
}

void GL_render()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glutSwapBuffers();

  /*glBegin(GL_LINE_STRIP);
  glColor3f(1.0f,0.0f,0.0f);  
  for(unsigned int i = 0; i < controlPoints.size(); i++)
  {
    glVertex2d(controlPoints[i].x, controlPoints[i].y);
  }
  glEnd();*/
  Bezier_render();

  glBegin(GL_POINTS);
  glColor3f(0.0f, 1.0f, 0.0f);
  for(unsigned int i = 0; i < controlPoints.size(); i++)
  {
    glVertex2d(controlPoints[i].x, controlPoints[i].y);
  }
  glEnd();
  glFlush();
}

void GL_mouse(int button,int state,int x,int y)
{
  y=WINDOW_HEIGHT-y;
  GLdouble mv_mat[16];
  GLdouble proj_mat[16];
  GLint vp_mat[4];
  glGetDoublev(GL_MODELVIEW_MATRIX,mv_mat);
  glGetDoublev(GL_PROJECTION_MATRIX,proj_mat);
  glGetIntegerv(GL_VIEWPORT,vp_mat);
  
  double dummy_z; // we don't care about the z-value but need something to pass in
  if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
    if(controlPoints.size() > 0 && clickCnt == 0) controlPoints.clear();
    Point2D point;
    gluUnProject(x,y,0,mv_mat,proj_mat,vp_mat,&point.x,&point.y,&dummy_z);
    controlPoints.push_back(point);
    clickCnt++;
  }
  /*else if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN && clickCnt==1){
    gluUnProject(x,y,0,mv_mat,proj_mat,vp_mat,&second.x,&second.y,&dummy_z);
    clickCnt++;
  }*/  
  else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN){
    clickCnt = 0;
    glutPostRedisplay();
  }  
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
  glutInit(argc, argv);
  //glutinitdisplaymode(glut_rgba | glut_double);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

  //glmatrixmode(gl_projectION_MATRIX);
  //glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
  for(unsigned int i = 0; i < 3; i++)
  {
    Point2D point;
    controlPoints.push_back(point);
  }
  glutCreateWindow("CS 130 - <Lab 8 - Bezier Curves>");
  glutDisplayFunc(GL_render);
  glutMouseFunc(GL_mouse);
}

int main(int argc, char** argv)
{	
  GLInit(&argc, argv);
  glutMainLoop();
  return 0;
}
