#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

GLuint loadTexture(const std::string &filename, int &width, int &height);


#define SUCCESS 0
#define LOAD_ERROR 1

class gl_image_texture
{
public:
    gl_image_texture();
    ~gl_image_texture();

    int load_texture(const std::string& filename);
    void bind() const;

private:
    GLuint texid;
    int width, height;
};

#endif
