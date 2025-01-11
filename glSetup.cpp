#include "glSetup.h"

#include <string.h>
#include <iostream>

using namespace std;

bool	perspectiveView = true;

float	screenScale = 0.5f;
int		screenW = 0, screenH = 0;
int		windowW, windowH;
float	aspect;
float	dpiScaling = 0;

int		vsync = 1;

float	fovy = 46.4f;

float	nearDist = 1.0f;
float	farDist = 20.0f;

extern GLfloat bgColor[4];	////////

void		errorCallback(int error, const char* description)
{
	cerr << "####" << description << endl;
}


GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bg[4])
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW, monitorH;
	glfwGetMonitorPhysicalSize(monitor, &monitorW, &monitorH);
	cerr << "Status : Monitor " << monitorW << "mm x " << monitorH << "mm" << endl;

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	if (screenW == 0)
		screenW = int(videoMode->width * screenScale);
	if (screenH == 0)
		screenH = int(videoMode->height * screenScale);

	GLFWwindow* window = glfwCreateWindow(screenW, screenH, argv[0], NULL, NULL);
	if (!window) {
		glfwTerminate();
		cerr << "Failed in glfwCreateWindow()" << endl;
		return NULL;
	}

	glfwMakeContextCurrent(window);

	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glfwSwapBuffers(window);

	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "Status : Screen " << screenW << " x " << screenH << endl;

	glfwGetFramebufferSize(window, &windowW, &windowH);
	cerr << "Status : Framebuffer " << windowW << " x " << windowH << endl;

	if (dpiScaling == 0)
		dpiScaling = float(windowW) / screenW;

	glfwSetFramebufferSizeCallback(window, reshape);

	cout << "Status : Renderer " << glGetString(GL_RENDERER) << endl;
	cout << "Status : Vendor " << glGetString(GL_VENDOR) << endl;
	cout << "Status : OpenGL " << glGetString(GL_VERSION) << endl;

	glfwSwapInterval(vsync);

	return window;
}


void		reshape(GLFWwindow* window, int w, int h)
{
	aspect = (float)w / h;

	windowW = w;
	windowH = h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	setupProjectionMatrix();

	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "reshape(" << w << ", " << ")";
	cerr << " with screen " << screenW << " x " << screenH << endl;
}


void		setupProjectionMatrix()
{
	if (perspectiveView)
		gluPerspective(fovy, aspect, nearDist, farDist);
	else
		glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -nearDist, farDist);
}


void		drawAxes(float l, float w)
{
	glLineWidth(w);

	glBegin(GL_LINES);

	glColor3f(1, 0, 0);		glVertex3f(0, 0, 0);	glVertex3f(1, 0, 0);

	glColor3f(0, 1, 0);		glVertex3f(0, 0, 0);	glVertex3f(0, 1, 0);

	glColor3f(0, 0, 1);		glVertex3f(0, 0, 0);	glVertex3f(0, 0, 1);

	glEnd();
}