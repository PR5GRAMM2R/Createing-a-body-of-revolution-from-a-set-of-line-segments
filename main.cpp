#include "glSetup.h"				// 2020203011 πË¡÷»Ø
#include <iostream>

using namespace std;

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <list>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

void	render2D(GLFWwindow* window);
void	render3D(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);
void	mouseButton(GLFWwindow* window, int button, int action, int mods);
void	mouseMove(GLFWwindow* window, double x, double y);
void	screen2world(float xs, float ys, float& xw, float& yw);

// Light configuration
vec4 light0(0.0, 5.0, 0.0, 1);
vec4 light1(0.0, -5.0, 0.0, 1);
vec4 light2(0.0, 0.0, -2.0, 1);

float	AXIS_LENGTH = 0.5;
float	AXIS_LINE_WIDTH = 2.0;

GLfloat bgColor[4] = { 1, 1, 1, 1 };

enum class InputMode
{
	NONE = 0,
	DRAGGING = 1,
	COMPLETE = 2,
};

InputMode	inputMode = InputMode::NONE;

bool	pause = true;

float	timeStep = 1.0f / 120;
float	period = 8.0f;
float rotationTheta = 0.5f;
int frame = 0;

vec2 point[2];

list<vec2> points2D;

vec3* points3D;

bool axes = true;

bool mode2D = true;

int curveCount = 36;

bool drawLine = true;

bool flatShadingMode = true;

void init()
{
	inputMode = InputMode::NONE;

	point[0] = vec2(0.0f, 0.0f);
	point[1] = vec2(0.0f, 0.0f);
	points2D.clear();
}


int		main(int argc, char* argv[])
{
	perspectiveView = false;

	vsync = 0;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL)
		return -1;

	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_POLYGON_OFFSET_FILL);

	reshape(window, windowW, windowH);

	std::cout << std::endl;
	std::cout << "Mouse button down :	start point of the line segment" << std::endl;
	std::cout << "Mouse dragging :		changes the line segment" << std::endl;
	std::cout << "Mouse button up :		end point of the line segment" << std::endl;
	cout << endl;
	std::cout << "Keyboard Input :		d for Transforming the dimension" << std::endl;
	std::cout << "Keyboard Input :		space for Play / Pause" << std::endl;
	std::cout << "Keyboard Input :		right for Increasing sample points " << std::endl;
	std::cout << "Keyboard Input :		left for Decreasing sample points" << std::endl;
	std::cout << "Keyboard Input :		s for Flat Shading / Gouraud Shading" << std::endl;
	std::cout << "Keyboard Input :		l for Wireframe On / Off" << std::endl;

	float previous = (float)glfwGetTime();
	float elapsed = 0;

	init();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float now = (float)glfwGetTime();
		float delta_t = now - previous;
		previous = now;

		elapsed += delta_t;

		if (elapsed > timeStep) {
			if (!pause) {
				frame++;
				//rotationTheta = float(2.0 * M_PI) / period * elapsed;
			}

			elapsed = 0;
		}

		if(mode2D)
			render2D(window);
		else
			render3D(window);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


void	render2D(GLFWwindow* window)
{
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glOrtho(-1.0, 1.0, -1.0, 1.0, -nearDist, farDist);

	glDisable(GL_LINE_STIPPLE);

	glColor3f(0, 0, 0);
	glLineWidth(5 * dpiScaling);

	glPointSize(15.0f);

	if (axes) {
		glDisable(GL_LIGHTING);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	if (inputMode == InputMode::DRAGGING) {
		if (points2D.size() != 0) {
			glColor3f(1, 0, 1);
			glBegin(GL_LINE_STRIP);

			list<vec2>::iterator pit;

			for (pit = points2D.begin(); pit != points2D.end(); pit++) {
				glVertex2f(pit->x, pit->y);
			}

			glEnd();
		}

		glColor3f(0, 0, 0);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);

		glBegin(GL_LINES);

		glVertex2f(point[0].x, point[0].y);
		glVertex2f(point[1].x, point[1].y);

		glEnd();

		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);

		list<vec2>::iterator pit;

		for (pit = points2D.begin(); pit != points2D.end(); pit++) {
			glVertex2f(pit->x, pit->y);
		}

		glEnd();
	}
	else
		glDisable(GL_LINE_STIPPLE);

	///////////////////////////////////////////////////////////////////////////////

	if (inputMode == InputMode::COMPLETE) {
		glColor3f(1, 0, 1);
		glBegin(GL_LINE_STRIP);

		list<vec2>::iterator pit;

		for (pit = points2D.begin(); pit != points2D.end(); pit++) {
			glVertex2f(pit->x, pit->y);
		}

		glEnd();

		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);

		for (pit = points2D.begin(); pit != points2D.end(); pit++) {
			glVertex2f(pit->x, pit->y);
		}

		glEnd();
	}
}


void setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light0));

	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT1, GL_POSITION, value_ptr(light1));

	glEnable(GL_LIGHT2);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT2, GL_POSITION, value_ptr(light2));
}


void setupMaterial()
{
	GLfloat mat_ambient[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 128;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}


void setDiffuseColor(const vec3& color)
{
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}


void	render3D(GLFWwindow* window)
{
	vector<vec3> fnormal;	// Face Normal
	vector<vec3> vnormal;	// Vertex Normal

	int curvePointsCount = points2D.size();
	points3D = new vec3[curvePointsCount * curveCount];

	fnormal.resize((curvePointsCount - 1) * (curveCount - 1));
	vnormal.resize(curvePointsCount * curveCount);

	list<vec2>::iterator pit2D;
	int i = 0;

	for (pit2D = points2D.begin(); pit2D != points2D.end(); pit2D++) {
		points3D[i++] = vec3(pit2D->x, pit2D->y, 0);
	}

	float theta = 360.0f / (curveCount - 1);

	for (int i = 0; i < curveCount - 1; i++) {
		glm::mat4 M = glm::mat4(1);
		M = glm::rotate(M, glm::radians(theta), glm::vec3(0, 1, 0));

		for (int j = 0; j < points2D.size(); j++) {
			points3D[curvePointsCount * (i + 1) + j] = glm::vec3(M * glm::vec4(points3D[curvePointsCount * i + j], 1));
		}
	}

	if (!pause) {
		for (int i = 0; i < curvePointsCount * curveCount; i++) {
			glm::mat4 M = glm::mat4(1);
			M = glm::rotate(M, glm::radians(frame * rotationTheta), glm::vec3(1, 0, 0));

			points3D[i] = glm::vec3(M * glm::vec4(points3D[i], 1));
		}
	}

	int fnormalCount = 0;

	for (int i = 0; i < (curveCount - 1) * curvePointsCount; i++) {
		if (i % curvePointsCount == curvePointsCount - 1) {
			continue;
		}

		glm::vec3 p0 = points3D[i];								//	p3	p2
		glm::vec3 p1 = points3D[i + curvePointsCount];			//	p0	p1
		glm::vec3 p2 = points3D[i + curvePointsCount + 1];
		glm::vec3 p3 = points3D[i + 1];

		glm::vec3 n = normalize(cross(p1 - p0, p3 - p0));

		fnormal[fnormalCount++] = n;

		vnormal[i] += n;
		vnormal[i + curvePointsCount] += n;
		vnormal[i + curvePointsCount + 1] += n;
		vnormal[i + 1] += n;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glOrtho(-1.0, 1.0, -1.0, 1.0, -nearDist, farDist);

	// Lighting
	setupLight();

	// Material
	setupMaterial();

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	glDisable(GL_LINE_STIPPLE);

	glLineWidth(3 * dpiScaling);

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(10.0f, 10.0f);

	fnormalCount = 0;

	for (int i = 0; i < (curveCount - 1) * curvePointsCount; i++) {
		if (i % curvePointsCount == curvePointsCount - 1) {
			continue;
		}

		glm::vec3 p0 = points3D[i];								//	p3	p2
		glm::vec3 p1 = points3D[i + curvePointsCount];			//	p0	p1
		glm::vec3 p2 = points3D[i + curvePointsCount + 1];
		glm::vec3 p3 = points3D[i + 1];

		glm::vec3 n = fnormal[fnormalCount++];

		glm::vec3 n0 = normalize(vnormal[i]);								//	n3	n2
		glm::vec3 n1 = normalize(vnormal[i + curvePointsCount]);			//	n0	n1
		glm::vec3 n2 = normalize(vnormal[i + curvePointsCount + 1]);
		glm::vec3 n3 = normalize(vnormal[i + 1]);

		glm::vec3 v = vec3(0, 0, 1);		// View vector

		if (glm::dot(n, v) > 0) {
			glPolygonMode(GL_FRONT, GL_FILL);

			setDiffuseColor(glm::vec3(1, 0.3, 0));

			if (flatShadingMode) {
				glBegin(GL_QUADS);

				glNormal3f(n.x, n.y, n.z);
				glVertex3f(p0.x, p0.y, p0.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();
			}
			else {
				glBegin(GL_QUADS);

				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);
				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();
			}

			/*if (drawLine) {
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1.0, -1.0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				setDiffuseColor(vec3(0, 0, 0));

				glBegin(GL_QUADS);

				//glColor3f(0, 0, 0);
				glVertex3f(p0.x, p0.y, p0.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();

				glDisable(GL_POLYGON_OFFSET_LINE);
			}*/
		}
		else {
			glPolygonMode(GL_BACK, GL_FILL);

			setDiffuseColor(glm::vec3(0, 1, 0.3));

			if (flatShadingMode) {
				glBegin(GL_QUADS);

				glNormal3f(n.x, n.y, n.z);
				glVertex3f(p0.x, p0.y, p0.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();
			}
			else {
				glBegin(GL_QUADS);

				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);
				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();
			}

			/*if (drawLine) {
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1.0, -1.0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				setDiffuseColor(vec3(0, 0, 0));

				glBegin(GL_QUADS);

				//glColor3f(0, 0, 0);
				glVertex3f(p0.x, p0.y, p0.z);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();

				glDisable(GL_POLYGON_OFFSET_LINE);
			}*/
		}
	}

	//glDisable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(1.0f, 1.0f);

	if (drawLine) {
		glLineWidth(3 * dpiScaling);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		setDiffuseColor(glm::vec3(0, 0, 0));

		for (int i = 0; i < (curveCount - 1) * curvePointsCount; i++) {
			if (i % curvePointsCount == curvePointsCount - 1) {
				continue;
			}

			glm::vec3 p0 = points3D[i];								//	p3	p2
			glm::vec3 p1 = points3D[i + curvePointsCount];			//	p0	p1
			glm::vec3 p2 = points3D[i + curvePointsCount + 1];
			glm::vec3 p3 = points3D[i + 1];

			/*glBegin(GL_QUADS);

			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);

			glEnd();*/

			glBegin(GL_LINES);

			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);

			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);

			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);

			glVertex3f(p3.x, p3.y, p3.z);
			glVertex3f(p0.x, p0.y, p0.z);

			glEnd();
		}
	}

	glDisable(GL_POLYGON_OFFSET_LINE);

	delete points3D;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void	screen2world(float xs, float ys, float& xw, float& yw)
{
	float aspect = (float)screenW / screenH;
	xw = 2.0f * (xs / (screenW - 1) - 0.5f) * aspect;
	yw = -2.0f * (ys / (screenH - 1) - 0.5f);
}


void	mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	double xs, ys;
	glfwGetCursorPos(window, &xs, &ys);

	float xw, yw;
	screen2world((float)xs, (float)ys, xw, yw);

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		inputMode = InputMode::DRAGGING;

		if (points2D.size() < 2) {
			point[0][0] = xw;
			point[0][1] = yw;
			points2D.emplace_back(point[0]);
			point[1][0] = xw;
			point[1][1] = yw;
		}
		else {
			point[0] = points2D.back();
			point[1][0] = xw;
			point[1][1] = yw;
		}
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		if (points2D.size() != 0)
			inputMode = InputMode::COMPLETE;
		else {
			init();
			return;
		}

		point[1][0] = xw;
		point[1][1] = yw;

		points2D.emplace_back(point[1]);
	}
}


void	mouseMove(GLFWwindow* window, double x, double y)
{
	double xs, ys;
	glfwGetCursorPos(window, &xs, &ys);

	float xw, yw;
	screen2world((float)xs, (float)ys, xw, yw);

	if (inputMode == InputMode::DRAGGING) {
		screen2world((float)x, (float)y, point[1][0], point[1][1]);
	}
}


void	keyboard(GLFWwindow* window, int key, int code, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_DELETE:
			if(points2D.size() > 2)
				points2D.pop_back();
			break;

		case GLFW_KEY_D:
			pause = true;
			mode2D = !mode2D;
			break;

		case GLFW_KEY_SPACE:
			frame = 0;
			pause = !pause;
			break;

		case GLFW_KEY_LEFT:
			curveCount--;			
			curveCount = (curveCount < 4) ? 4 : curveCount;
			break;

		case GLFW_KEY_RIGHT:
			curveCount++;			
			curveCount = (curveCount > 100) ? 100 : curveCount;
			break;

		case GLFW_KEY_L:
			drawLine = !drawLine;
			break;

		case GLFW_KEY_S:
			flatShadingMode = !flatShadingMode;
			break;
		}
	}
}