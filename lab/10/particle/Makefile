CXX = g++
CXXFLAGS = -O2
CPPFLAGS = -Wall -g
LIBS  = -lGL -lGLU -lglut -lpng
OBJS  = gl_viewer.o gl_camera.o main.o application.o timer.o obj.o gl_texture.o matrix.o 
TARGET = particle

a1:	$(OBJS)
	$(CXX) $(CXXLFAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	rm -f *.o $(TARGET)

gl_camera.o: gl_camera.h transform.h
gl_viewer.o: gl_viewer.h gl_camera.h 
main.o: application.h gl_viewer.h vector.h matrix.h 
application.o: application.h matrix.h transform.h
timer.o: timer.h
obj.o : obj.h
texture.o : gl_texture.h
application.h : gl_texture.h gl_viewer.h obj.h timer.h
vector.h : matrix.h 
gl_camera.h : vector.h 
