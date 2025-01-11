#ifndef __GL_SETUP_H__
#define __GL_SETUP_H__

#if defined(__APPLE__) && defined(__MACH__)
	#include <Opengl/glu.h>
#else
	#include <windows.h>
	#include <gl/GLU.h>
#endif

#include <GLFW/glfw3.h>

extern float	screenScale;
extern int		screenW, screenH;
extern int		windowW, windowH;
extern float	aspect;
extern float	dpiScaling;

extern int		vsync;

extern bool		perspectiveView;
extern float	fovy;
extern float	nearDist;
extern float	farDist;

GLFWwindow*	initializeOpenGL(int argc, char* argv[], GLfloat bg[4]);
void		reshape(GLFWwindow* window, int w, int h);
void		setupProjectionMatrix();

void		drawAxes(float l, float w);

#endif // __GL_SETUP_H__
