CXX = g++
CXXFLAGS = -O3
CPPFLAGS = -Wall -g
LIBS  = -lGL -lGLU -lglut
OBJS  = gl_viewer.o main.o application.o
SRCS = gl_viewer.cpp main.cpp application.cpp
TARGET = ddaline

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

a1:	$(OBJS)
	$(CXX) $(OBJS) -o ddaline $(LIBS)

clean:
	rm -f *.o ddaline

depend:
	makedepend -- $(CPPFLAGS) -- $(SRCS)

gl_viewer.o: gl_viewer.h
main.o: application.h gl_viewer.h
application.o: application.h gl_viewer.h
