CS130 Fall 2015 Lab1

0. Don't forget to save all of your work on your google drive or student account before logging out.

1. To compile lab1.cpp, use the "make" or "make all" command in the terminal while in the same directory as the cpp file.

2. Entering "make clean" like in (1)  will remove lab1.out.

3. When you run lab1.out, if you encounter this error at the terminal: "./lab1.out: error while loading shared libraries: libGLEW.so.1.13: cannot open shared object file: No such file or directory," then you will need to update the LD_LIBRARY_PATH environment variable so that all the linked libraries needed to run your code can be found.

At the terminal enter in "export LD_LIBRARY_PATH=/usr/csshare/pkgs/glew-1.13.0/lib64/" and press enter.

To confirm that the environment variable was set you can enter in "echo $LD_LIBRARY_PATH". 

This will only apply to your current terminal session. Opening a new terminal tab or window will require setting the same environment variable again.

