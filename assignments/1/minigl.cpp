/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include "minigl.h"

using namespace std;

MGLmatrix_mode  m_mode;
MGLpoly_mode p_mode;
MGLbool running;

MGLpixel COLOR = 0;

vector < vector < MGLfloat > > Projection;
vector < vector < MGLfloat > > Matrices;

struct Coordinates
{
    MGLfloat x, y, z, w;
    MGLpixel R, B, G;
};
vector < vector < Coordinates > > Triangles;
vector < vector < Coordinates > > Quads;

/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
    printf("%s\n", description);
    exit(1);
}

void BoundingBox(MGLint a, MGLint b, MGLint c, int & min, int & max)
{
    //get min
    min = (a < min)? a : min;
    min = (b < min)? b : min;
    min = (c < min)? c : min;
    //get max
    max = (a > max)? a : max;
    max = (b > max)? b : max;
    max = (c > max)? c : max;
}

//get alpha, beta, or gamma values
MGLfloat getABG(int x, int y, int xb, int yb, int xc, int yc)
{
    MGLfloat val = 0;
    val = (yb - yc)*x + (xc - xb)*y + xb*yc - xc*yb;
    return val;
}

/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width,
                   MGLsize height,
                   MGLpixel *data)
{
    int min_x;
    int min_y; 
    int max_x; 
    int max_y;
    MGLfloat alpha, beta, gamma;
    MGLpixel color, Red, Green, Blue;
    MGLint xa, ya, xb, yb, xc, yc ,xr, yr;
    MGLfloat za, zb, zc, zr;
    MGLfloat zBuff [width*height];
    for(unsigned int x = 0; x < width; x++)
    {
	for(unsigned int y = 0; y < height; y++)
	{
	    zBuff[y*width+x] = INFINITY;
	}
    }

    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    for(unsigned int i = 0; i < Triangles.size(); i++)
    {
	xa = (int)((width/2)*(Triangles[i][0].x + 1));
	ya = (int)((height/2)*(1 + Triangles[i][0].y));
	za = Triangles[i][0].z;

	xb = (int)((width/2)*(Triangles[i][1].x + 1));
	yb = (int)((height/2)*(1 + Triangles[i][1].y));
	zb = Triangles[i][1].z;

	xc = (int)((width/2)*(Triangles[i][2].x + 1));
	yc = (int)((height/2)*(1 + Triangles[i][2].y));
	zc = Triangles[i][2].z;
	//triangle bounding box
	min_x = width;
	min_y = height;
	max_x = 0;
	max_y = 0;
	BoundingBox(xa, xb, xc, min_x, max_x);
	BoundingBox(ya, yb, yc, min_y, max_y);
	min_x = (min_x < 0)? 0 : min_x;
	max_x = (max_x > (int)width)? width : max_x;
	min_y = (min_y < 0)? 0 : min_y;
	max_y = (max_y > (int)height)? height : max_y;
	//rasterize triangle
	for(int y = min_y; y < max_y; y++)
	{
	    for(int x = min_x; x < max_x; x++)
	    {
		alpha = getABG(x, y, xb, yb, xc, yc)/getABG(xa, ya, xb, yb, xc, yc);
		beta = getABG(x, y, xc, yc, xa, ya)/getABG(xb, yb, xc, yc, xa, ya);
		gamma = getABG(x, y, xa, ya, xb, yb)/getABG(xc, yc, xa, ya, xb, yb);
	    	if(alpha >= 0 && beta >= 0 && gamma >= 0)
	    	{
		    if(zBuff[y*width+x] > (alpha*za + beta*zb + gamma*zc))
		    {
			zBuff[y*width+x] = alpha*za + beta*zb + gamma*zc;
			//get color
			Red = (int)(alpha*Triangles[i][0].R+ beta*Triangles[i][1].R + gamma*Triangles[i][2].R);
			Green = (int)(alpha*Triangles[i][0].G + beta*Triangles[i][1].G + gamma*Triangles[i][2].G);
			Blue = (int)(alpha*Triangles[i][0].B + beta*Triangles[i][1].B + gamma*Triangles[i][2].B);
			color = 0;
			MGL_SET_RED(color, Red);
			MGL_SET_GREEN(color, Green);
			MGL_SET_BLUE(color, Blue);
		    	data[y*width+x] = color;
		    }
	    	}
	    }
	}
    }
    for(unsigned int i = 0; i < Quads.size(); i++)
    {
	xa = (int)((width/2)*(Quads[i][0].x + 1));
	ya = (int)((height/2)*(1 + Quads[i][0].y));
	za = Quads[i][0].z;

	xb = (int)((width/2)*(Quads[i][1].x + 1));
	yb = (int)((height/2)*(1 + Quads[i][1].y));
	zb = Quads[i][1].z;

	xc = (int)((width/2)*(Quads[i][2].x + 1));
	yc = (int)((height/2)*(1 + Quads[i][2].y));
	zc = Quads[i][2].z;

	xr = (int)((width/2)*(Quads[i][3].x + 1));
	yr = (int)((height/2)*(1 + Quads[i][3].y));
	zr = Quads[i][3].z;
	//triangle 1 bounding box
	min_x = width;
	min_y = height;
	max_x = 0;
	max_y = 0;
	BoundingBox(xa, xb, xc, min_x, max_x);
	BoundingBox(ya, yb, yc, min_y, max_y);
	min_x = (min_x < 0)? 0 : min_x;
	max_x = (max_x > (int)width)? width : max_x;
	min_y = (min_y < 0)? 0 : min_y;
	max_y = (max_y > (int)height)? height : max_y;
	for(int y = min_y; y < max_y; y++)
	{
	    for(int x = min_x; x < max_x; x++)
	    {
		alpha = getABG(x, y, xb, yb, xc, yc)/getABG(xa, ya, xb, yb, xc, yc);
		beta = getABG(x, y, xc, yc, xa, ya)/getABG(xb, yb, xc, yc, xa, ya);
		gamma = getABG(x, y, xa, ya, xb, yb)/getABG(xc, yc, xa, ya, xb, yb);
	    	if(alpha >= 0 && beta >= 0 && gamma >= 0)
	    	{
		    if((alpha > 0 || (getABG(xa, ya, xb, yb, xc, yc) * getABG(xr, yr, xb, yb, xc, yc)) > 0) && 
			(beta > 0 || (getABG(xb, yb, xc, yc, xa, ya) * getABG(xr, yr, xc, yc, xa, ya)) > 0) && 
			(gamma > 0 || (getABG(xc, yc, xa, ya, xb, yb) * getABG(xr, yr, xa, ya, xb, yb)) > 0))
		    {
		   	if(zBuff[y*width+x] > (alpha*za + beta*zb + gamma*zc))
		    	{
			    zBuff[y*width+x] = alpha*za + beta*zb + gamma*zc;
			    //get color
			    Red = (int)(alpha*Quads[i][0].R + beta*Quads[i][1].R + gamma*Quads[i][2].R);
			    Green = (int)(alpha*Quads[i][0].G + beta*Quads[i][1].G + gamma*Quads[i][2].G);
			    Blue = (int)(alpha*Quads[i][0].B + beta*Quads[i][1].B + gamma*Quads[i][2].B);
			    color = 0;
			    MGL_SET_RED(color, Red);
			    MGL_SET_GREEN(color, Green);
			    MGL_SET_BLUE(color, Blue);
			    data[y*width+x] = color;
		  	}
		    }
	    	}
	    }
	}
	//triangle 2 bounding box
	min_x = width;
	min_y = height;
	max_x = 0;
	max_y = 0;
	BoundingBox(xa, xc, xr, min_x, max_x);
	BoundingBox(ya, yc, yr, min_y, max_y);
	min_x = (min_x < 0)? 0 : min_x;
	max_x = (max_x > (int)width)? width : max_x;
	min_y = (min_y < 0)? 0 : min_y;
	max_y = (max_y > (int)height)? height : max_y;
	for(int y = min_y; y < max_y; y++)
	{
	    for(int x = min_x; x < max_x; x++)
	    {
		alpha = getABG(x, y, xc, yc, xr, yr)/getABG(xa, ya, xc, yc, xr, yr);
		beta = getABG(x, y, xr, yr, xa, ya)/getABG(xc, yc, xr, yr, xa, ya);
		gamma = getABG(x, y, xa, ya, xc, yc)/getABG(xr, yr, xa, ya, xc, yc);
	    	if(alpha >= 0 && beta >= 0 && gamma >= 0)
	    	{
	 	    if((alpha > 0 || (getABG(xa, ya, xc, yc, xr, yr) * getABG(xb, yb, xc, yc, xr, yr)) > 0) && 
			(beta > 0 || (getABG(xc, yc, xr, yr, xa, ya) * getABG(xb, yb, xr, yr, xa, ya)) > 0) && 
			(gamma > 0 || (getABG(xr, yr, xa, ya, xc, yc) * getABG(xb, yb, xa, ya, xc, yc)) > 0))
		    {
		   	if(zBuff[y*width+x] > (alpha*za + beta*zc + gamma*zr))
		    	{
			    zBuff[y*width+x] = alpha*za + beta*zc + gamma*zr;
			    //get color
			    Red = (int)(alpha*Quads[i][0].R + beta*Quads[i][2].R + gamma*Quads[i][3].R);
			    Green = (int)(alpha*Quads[i][0].G + beta*Quads[i][2].G + gamma*Quads[i][3].G);
			    Blue = (int)(alpha*Quads[i][0].B + beta*Quads[i][2].B + gamma*Quads[i][3].B);
			    color = 0;
			    MGL_SET_RED(color, Red);
			    MGL_SET_GREEN(color, Green);
			    MGL_SET_BLUE(color, Blue);
			    data[y*width+x] = color;
		  	}
		    }
	    	}
	    }
	}
    }
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    p_mode = mode;
    running = true;
}

void ModelMultVertex(MGLfloat * vert)
{
    MGLfloat v0 = vert[0]*Matrices[Matrices.size()-1][0] + vert[1]*Matrices[Matrices.size()-1][4] +
		  vert[2]*Matrices[Matrices.size()-1][8] + vert[3]*Matrices[Matrices.size()-1][12];
    MGLfloat v1 = vert[0]*Matrices[Matrices.size()-1][1] + vert[1]*Matrices[Matrices.size()-1][5] +
		  vert[2]*Matrices[Matrices.size()-1][9] + vert[3]*Matrices[Matrices.size()-1][13];
    MGLfloat v2 = vert[0]*Matrices[Matrices.size()-1][2] + vert[1]*Matrices[Matrices.size()-1][6] +
		  vert[2]*Matrices[Matrices.size()-1][10] + vert[3]*Matrices[Matrices.size()-1][14];
    MGLfloat v3 = vert[0]*Matrices[Matrices.size()-1][3] + vert[1]*Matrices[Matrices.size()-1][7] +
		  vert[2]*Matrices[Matrices.size()-1][11] + vert[3]*Matrices[Matrices.size()-1][15];
    vert[0] = v0;
    vert[1] = v1;
    vert[2] = v2;
    vert[3] = v3;
}

void ProjMultVertex(MGLfloat * vert)
{
    MGLfloat v0 = vert[0]*Projection[Projection.size()-1][0] + vert[1]*Projection[Projection.size()-1][4] +
		  vert[2]*Projection[Projection.size()-1][8] + vert[3]*Projection[Projection.size()-1][12];
    MGLfloat v1 = vert[0]*Projection[Projection.size()-1][1] + vert[1]*Projection[Projection.size()-1][5] +
		  vert[2]*Projection[Projection.size()-1][9] + vert[3]*Projection[Projection.size()-1][13];
    MGLfloat v2 = vert[0]*Projection[Projection.size()-1][2] + vert[1]*Projection[Projection.size()-1][6] +
		  vert[2]*Projection[Projection.size()-1][10] + vert[3]*Projection[Projection.size()-1][14];
    MGLfloat v3 = vert[0]*Projection[Projection.size()-1][3] + vert[1]*Projection[Projection.size()-1][7] +
		  vert[2]*Projection[Projection.size()-1][11] + vert[3]*Projection[Projection.size()-1][15];
    vert[0] = v0;
    vert[1] = v1;
    vert[2] = v2;
    vert[3] = v3;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
    if(!running) MGL_ERROR("MGL_INVALID_OPERATION");
    running = false;
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y)
{
    Coordinates v;
    v.x = x;
    v.y = y;
    v.z = 0;
    v.w = 1;
    v.R = MGL_GET_RED(COLOR);
    v.G = MGL_GET_GREEN(COLOR);
    v.B = MGL_GET_BLUE(COLOR);
    MGLfloat * vert;
    vert = new MGLfloat[4];
    vert[0] = v.x;
    vert[1] = v.y;
    vert[2] = v.z;
    vert[3] = v.w;
    ModelMultVertex(vert);
    ProjMultVertex(vert);
    v.x = vert[0]/vert[3];
    v.y = vert[1]/vert[3];
    v.z = vert[2]/vert[3];
    v.w = vert[3];
    if(p_mode == MGL_TRIANGLES)
    {
	if(Triangles.size() == 0)
	{
	    vector < Coordinates > triangle;
	    Triangles.push_back(triangle);
	    Triangles[0].push_back(v);
	}
	else
	{
	   if(Triangles[Triangles.size()-1].size() == 3)
	   {
		vector < Coordinates > triangle;
		Triangles.push_back(triangle);
	   }
	   Triangles[Triangles.size()-1].push_back(v);
	}
    }
    else
    {
	if(Quads.size() == 0)
	{
	    vector < Coordinates > quad;
	    Quads.push_back(quad);
	    Quads[0].push_back(v);
	}
	else
	{
	   if(Quads[Quads.size()-1].size() == 4)
	   {
		vector < Coordinates > quad;
		Quads.push_back(quad);
	   }
	   Quads[Quads.size()-1].push_back(v);
	}
    }
}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
    Coordinates v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = 1;
    v.R = MGL_GET_RED(COLOR);
    v.G = MGL_GET_GREEN(COLOR);
    v.B = MGL_GET_BLUE(COLOR);
    MGLfloat * vert;
    vert = new MGLfloat[4];
    vert[0] = v.x;
    vert[1] = v.y;
    vert[2] = v.z;
    vert[3] = v.w;
    ModelMultVertex(vert);
    ProjMultVertex(vert);
    v.x = vert[0]/vert[3];
    v.y = vert[1]/vert[3];
    v.z = vert[2]/vert[3];
    v.w = vert[3];
    if(p_mode == MGL_TRIANGLES)
    {
	if(Triangles.size() == 0)
	{
	    vector < Coordinates > triangle;
	    Triangles.push_back(triangle);
	    Triangles[0].push_back(v);
	}
	else
	{
	   if(Triangles[Triangles.size()-1].size() == 3)
	   {
		vector < Coordinates > triangle;
		Triangles.push_back(triangle);
	   }
	   Triangles[Triangles.size()-1].push_back(v);
	}
    }
    else
    {
	if(Quads.size() == 0)
	{
	    vector < Coordinates > quad;
	    Quads.push_back(quad);
	    Quads[0].push_back(v);
	}
	else
	{
	   if(Quads[Quads.size()-1].size() == 4)
	   {
		vector < Coordinates > quad;
		Quads.push_back(quad);
	   }
	   Quads[Quads.size()-1].push_back(v);
	}
    }
}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    m_mode = mode;
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(m_mode == MGL_MODELVIEW)
    {
	vector < MGLfloat > newMatrix (16);
	Matrices.push_back(newMatrix);
	for(unsigned int i = 0; i < 16; i++)
	{
	    Matrices[Matrices.size()-1][i] = Matrices[Matrices.size()-2][i];
	}
    }
    else
    {
	vector < MGLfloat > newMatrix (16);
	Matrices.push_back(newMatrix);
	for(unsigned int i = 0; i < 16; i++)
	{
	    Projection[Projection.size()-1][i] = Projection[Projection.size()-2][i];
	}
    }
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(m_mode == MGL_MODELVIEW)
    {
	if(Matrices.size() == 1) MGL_ERROR("MGL_STACK_UNDERFLOW");
	Matrices.pop_back();
    }
    else
    {
	if(Projection.size() == 1) MGL_ERROR("MGL_STACK_UNDERFLOW");
	Projection.pop_back();
    }
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(m_mode == MGL_MODELVIEW)
    {
	vector < MGLfloat> M (16);
	for(int col = 0; col < 4; col++)
	{
	    for(int row = 0; row < 4; row++)
	    {
		M[(4*col)+row] = (col == row)? 1 : 0;
	    }
	}
	if(Matrices.size() != 0) Matrices.pop_back();
	Matrices.push_back(M);
    }
    else
    {
	vector < MGLfloat > P (16);
	for(int col = 0; col < 4; col++)
	{
	    for(int row = 0; row < 4; row++)
	    {
		P[(4*col)+row] = (col == row)? 1 : 0;
	    }
	}
	if(Projection.size() != 0) Projection.pop_back();
	Projection.push_back(P);
    }
}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix)
{
}

/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(m_mode == MGL_MODELVIEW)
    {
	MGLfloat i0 = Matrices[Matrices.size()-1][0]*matrix[0] + Matrices[Matrices.size()-1][4]*matrix[1] +
		      Matrices[Matrices.size()-1][8]*matrix[2] + Matrices[Matrices.size()-1][12]*matrix[3];
	MGLfloat i1 = Matrices[Matrices.size()-1][1]*matrix[0] + Matrices[Matrices.size()-1][5]*matrix[1] + 
		      Matrices[Matrices.size()-1][9]*matrix[2] + Matrices[Matrices.size()-1][13]*matrix[3];
	MGLfloat i2 = Matrices[Matrices.size()-1][2]*matrix[0] + Matrices[Matrices.size()-1][6]*matrix[1] +
		      Matrices[Matrices.size()-1][10]*matrix[2] + Matrices[Matrices.size()-1][14]*matrix[3];
	MGLfloat i3 = Matrices[Matrices.size()-1][3]*matrix[0] + Matrices[Matrices.size()-1][7]*matrix[1] +
		      Matrices[Matrices.size()-1][11]*matrix[2] + Matrices[Matrices.size()-1][15]*matrix[3];
	//column 1
	MGLfloat i4 = Matrices[Matrices.size()-1][0]*matrix[4] + Matrices[Matrices.size()-1][4]*matrix[5] +
		      Matrices[Matrices.size()-1][8]*matrix[6] + Matrices[Matrices.size()-1][12]*matrix[7];
	MGLfloat i5 = Matrices[Matrices.size()-1][1]*matrix[4] + Matrices[Matrices.size()-1][5]*matrix[5] + 
		      Matrices[Matrices.size()-1][9]*matrix[6] + Matrices[Matrices.size()-1][13]*matrix[7];
	MGLfloat i6 = Matrices[Matrices.size()-1][2]*matrix[4] + Matrices[Matrices.size()-1][6]*matrix[5] +
		      Matrices[Matrices.size()-1][10]*matrix[6] + Matrices[Matrices.size()-1][14]*matrix[7];
	MGLfloat i7 = Matrices[Matrices.size()-1][3]*matrix[4] + Matrices[Matrices.size()-1][7]*matrix[5] +
		      Matrices[Matrices.size()-1][11]*matrix[6] + Matrices[Matrices.size()-1][15]*matrix[7];
	//column 2
	MGLfloat i8 = Matrices[Matrices.size()-1][0]*matrix[8] + Matrices[Matrices.size()-1][4]*matrix[9] +
		      Matrices[Matrices.size()-1][8]*matrix[10] + Matrices[Matrices.size()-1][12]*matrix[11];
	MGLfloat i9 = Matrices[Matrices.size()-1][1]*matrix[8] + Matrices[Matrices.size()-1][5]*matrix[9] +
		      Matrices[Matrices.size()-1][9]*matrix[10] + Matrices[Matrices.size()-1][13]*matrix[11];
	MGLfloat i10 = Matrices[Matrices.size()-1][2]*matrix[8] + Matrices[Matrices.size()-1][6]*matrix[9] +
		       Matrices[Matrices.size()-1][10]*matrix[10] + Matrices[Matrices.size()-1][14]*matrix[11];
	MGLfloat i11 = Matrices[Matrices.size()-1][3]*matrix[8] + Matrices[Matrices.size()-1][7]*matrix[9] + 
		       Matrices[Matrices.size()-1][11]*matrix[10] + Matrices[Matrices.size()-1][15]*matrix[11];
	//column 3
	MGLfloat i12 = Matrices[Matrices.size()-1][0]*matrix[12] + Matrices[Matrices.size()-1][4]*matrix[13] +
		       Matrices[Matrices.size()-1][8]*matrix[14] + Matrices[Matrices.size()-1][12]*matrix[15];
	MGLfloat i13 = Matrices[Matrices.size()-1][1]*matrix[12] + Matrices[Matrices.size()-1][5]*matrix[13] +
		       Matrices[Matrices.size()-1][9]*matrix[14] + Matrices[Matrices.size()-1][13]*matrix[15];
	MGLfloat i14 = Matrices[Matrices.size()-1][2]*matrix[12] + Matrices[Matrices.size()-1][6]*matrix[13] +
		       Matrices[Matrices.size()-1][10]*matrix[14] + Matrices[Matrices.size()-1][14]*matrix[15];
	MGLfloat i15 = Matrices[Matrices.size()-1][3]*matrix[12] + Matrices[Matrices.size()-1][7]*matrix[13] +
		       Matrices[Matrices.size()-1][11]*matrix[14] + Matrices[Matrices.size()-1][15]*matrix[15];
	Matrices[Matrices.size()-1][0] = i0;
	Matrices[Matrices.size()-1][1] = i1;
	Matrices[Matrices.size()-1][2] = i2;
	Matrices[Matrices.size()-1][3] = i3;
	Matrices[Matrices.size()-1][4] = i4;
	Matrices[Matrices.size()-1][5] = i5;
	Matrices[Matrices.size()-1][6] = i6;
	Matrices[Matrices.size()-1][7] = i7;
	Matrices[Matrices.size()-1][8] = i8;
	Matrices[Matrices.size()-1][9] = i9;
	Matrices[Matrices.size()-1][10] = i10;
	Matrices[Matrices.size()-1][11] = i11;
	Matrices[Matrices.size()-1][12] = i12;
	Matrices[Matrices.size()-1][13] = i13;
	Matrices[Matrices.size()-1][14] = i14;
	Matrices[Matrices.size()-1][15] = i15;
    }
    else
    {
	//column 0
	MGLfloat i0 = Projection[Projection.size()-1][0]*matrix[0] + Projection[Projection.size()-1][4]*matrix[1] +
		      Projection[Projection.size()-1][8]*matrix[2] + Projection[Projection.size()-1][12]*matrix[3];
	MGLfloat i1 = Projection[Projection.size()-1][1]*matrix[0] + Projection[Projection.size()-1][5]*matrix[1] + 
		      Projection[Projection.size()-1][9]*matrix[2] + Projection[Projection.size()-1][13]*matrix[3];
	MGLfloat i2 = Projection[Projection.size()-1][2]*matrix[0] + Projection[Projection.size()-1][6]*matrix[1] +
		      Projection[Projection.size()-1][10]*matrix[2] + Projection[Projection.size()-1][14]*matrix[3];
	MGLfloat i3 = Projection[Projection.size()-1][3]*matrix[0] + Projection[Projection.size()-1][7]*matrix[1] +
		      Projection[Projection.size()-1][11]*matrix[2] + Projection[Projection.size()-1][15]*matrix[3];
	//column 1
	MGLfloat i4 = Projection[Projection.size()-1][0]*matrix[4] + Projection[Projection.size()-1][4]*matrix[5] +
		      Projection[Projection.size()-1][8]*matrix[6] + Projection[Projection.size()-1][12]*matrix[7];
	MGLfloat i5 = Projection[Projection.size()-1][1]*matrix[4] + Projection[Projection.size()-1][5]*matrix[5] + 
		      Projection[Projection.size()-1][9]*matrix[6] + Projection[Projection.size()-1][13]*matrix[7];
	MGLfloat i6 = Projection[Projection.size()-1][2]*matrix[4] + Projection[Projection.size()-1][6]*matrix[5] +
		      Projection[Projection.size()-1][10]*matrix[6] + Projection[Projection.size()-1][14]*matrix[7];
	MGLfloat i7 = Projection[Projection.size()-1][3]*matrix[4] + Projection[Projection.size()-1][7]*matrix[5] +
		      Projection[Projection.size()-1][11]*matrix[6] + Projection[Projection.size()-1][15]*matrix[7];
	//column 2
	MGLfloat i8 = Projection[Projection.size()-1][0]*matrix[8] + Projection[Projection.size()-1][4]*matrix[9] +
		      Projection[Projection.size()-1][8]*matrix[10] + Projection[Projection.size()-1][12]*matrix[11];
	MGLfloat i9 = Projection[Projection.size()-1][1]*matrix[8] + Projection[Projection.size()-1][5]*matrix[9] +
		      Projection[Projection.size()-1][9]*matrix[10] + Projection[Projection.size()-1][13]*matrix[11];
	MGLfloat i10 = Projection[Projection.size()-1][2]*matrix[8] + Projection[Projection.size()-1][6]*matrix[9] +
		       Projection[Projection.size()-1][10]*matrix[10] + Projection[Projection.size()-1][14]*matrix[11];
	MGLfloat i11 = Projection[Projection.size()-1][3]*matrix[8] + Projection[Projection.size()-1][7]*matrix[9] + 
		       Projection[Projection.size()-1][11]*matrix[10] + Projection[Projection.size()-1][15]*matrix[11];
	//column 3
	MGLfloat i12 = Projection[Projection.size()-1][0]*matrix[12] + Projection[Projection.size()-1][4]*matrix[13] +
		       Projection[Projection.size()-1][8]*matrix[14] + Projection[Projection.size()-1][12]*matrix[15];
	MGLfloat i13 = Projection[Projection.size()-1][1]*matrix[12] + Projection[Projection.size()-1][5]*matrix[13] +
		       Projection[Projection.size()-1][9]*matrix[14] + Projection[Projection.size()-1][13]*matrix[15];
	MGLfloat i14 = Projection[Projection.size()-1][2]*matrix[12] + Projection[Projection.size()-1][6]*matrix[13] +
		       Projection[Projection.size()-1][10]*matrix[14] + Projection[Projection.size()-1][14]*matrix[15];
	MGLfloat i15 = Projection[Projection.size()-1][3]*matrix[12] + Projection[Projection.size()-1][7]*matrix[13] +
		       Projection[Projection.size()-1][11]*matrix[14] + Projection[Projection.size()-1][15]*matrix[15];
	Projection[Projection.size()-1][0] = i0;
	Projection[Projection.size()-1][1] = i1;
	Projection[Projection.size()-1][2] = i2;
	Projection[Projection.size()-1][3] = i3;
	Projection[Projection.size()-1][4] = i4;
	Projection[Projection.size()-1][5] = i5;
	Projection[Projection.size()-1][6] = i6;
	Projection[Projection.size()-1][7] = i7;
	Projection[Projection.size()-1][8] = i8;
	Projection[Projection.size()-1][9] = i9;
	Projection[Projection.size()-1][10] = i10;
	Projection[Projection.size()-1][11] = i11;
	Projection[Projection.size()-1][12] = i12;
	Projection[Projection.size()-1][13] = i13;
	Projection[Projection.size()-1][14] = i14;
	Projection[Projection.size()-1][15] = i15;
    }
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    MGLfloat  translate [16];
    //column 0
    translate[0] = 1;
    translate[1] = 0;
    translate[2] = 0;
    translate[3] = 0;
    //column 1
    translate[4] = 0;
    translate[5] = 1;
    translate[6] = 0;
    translate[7] = 0;
    //column 2
    translate[8] = 0;
    translate[9] = 0;
    translate[10] = 1;
    translate[11] = 0;
    //column 3
    translate[12] = x;
    translate[13] = y;
    translate[14] = z;
    translate[15] = 1;
    mglMultMatrix(translate);
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    MGLfloat rotate[16];
    angle *= (3.14159265/180);
    MGLfloat c = cos(angle);
    MGLfloat s = sin(angle);
    MGLfloat n = sqrt(x*x + y*y + z*z);
    x /= n;
    y /= n;
    z /= n;
    //column 0
    rotate[0] = ((x*x)*(1-c))+c;
    rotate[1] = ((y*x)*(1-c))+(z*s);
    rotate[2] = ((x*z)*(1-c))-(y*s);
    rotate[3] = 0;
    //column 1
    rotate[4] = ((x*y)*(1-c))-(z*s);
    rotate[5] = ((y*y)*(1-c))+c;
    rotate[6] = ((y*z)*(1-c))+(x*s);
    rotate[7] = 0;
    //column 2
    rotate[8] = ((x*z)*(1-c))+(y*s);
    rotate[9] = ((y*z)*(1-c))-(x*s);
    rotate[10] = ((z*z)*(1-c))+c;
    rotate[11] = 0;
    //column 3
    rotate[12] = 0;
    rotate[13] = 0;
    rotate[14] = 0;
    rotate[15] = 1;
    mglMultMatrix(rotate);
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    MGLfloat scale[16];
    //column 0
    scale[0] = x;
    scale[1] = 0;
    scale[2] = 0;
    scale[3] = 0;
    //column 1
    scale[4] = 0;
    scale[5] = y;
    scale[6] = 0;
    scale[7] = 0;
    //column 2
    scale[8] = 0;
    scale[9] = 0;
    scale[10] = z;
    scale[11] = 0;
    //column 3
    scale[12] = 0;
    scale[13] = 0;
    scale[14] = 0;
    scale[15] = 1;
    mglMultMatrix(scale);
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(near == far || top == bottom || left == right) MGL_ERROR("MGL_INVALID_VALUE");
    MGLfloat Frustum[16];
    //column 0
    Frustum[0] = (2*near)/(right-left);
    Frustum[1] = 0;
    Frustum[2] = 0;
    Frustum[3] = 0;
    //column 1
    Frustum[4] = 0;
    Frustum[5] = (2*near)/(top-bottom);
    Frustum[6] = 0;
    Frustum[7] = 0;
    //column 2
    Frustum[8] = (right+left)/(right-left);
    Frustum[9] = (top+bottom)/(top-bottom);
    Frustum[10] = -((far+near)/(far-near));
    Frustum[11] = -1;
    //column 3
    Frustum[12] = 0;
    Frustum[13] = 0;
    Frustum[14] = -((2*far*near)/(far-near));
    Frustum[15] = 0;
    mglMultMatrix(Frustum);
    //Transformations.push_back(Projection);
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far)
{
    if(running) MGL_ERROR("MGL_INVALID_OPERATION");
    if(near == far || top == bottom || left == right) MGL_ERROR("MGL_INVALID_VALUE");
    MGLfloat Ortho[16];
    //column 0
    Ortho[0] = 2/(right-left);
    Ortho[1] = 0;
    Ortho[2] = 0;
    Ortho[3] = 0;
    //column 1
    Ortho[4] = 0;
    Ortho[5] = 2/(top-bottom);
    Ortho[6] = 0;
    Ortho[7] = 0;
    //column 2
    Ortho[8] = 0;
    Ortho[9] = 0;
    Ortho[10] = -2/(far-near);
    Ortho[11] = 0;
    //column 3
    Ortho[12] = -((right+left)/(right-left));
    Ortho[13] = -((top+bottom)/(top-bottom));
    Ortho[14] = -((far+near)/(far-near));
    Ortho[15] = 1;
    mglMultMatrix(Ortho);
    //Transformations.push_back(Projection);
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
    COLOR = 0;
    MGL_SET_RED(COLOR, red);
    MGL_SET_GREEN(COLOR, green);
    MGL_SET_BLUE(COLOR, blue);
}
