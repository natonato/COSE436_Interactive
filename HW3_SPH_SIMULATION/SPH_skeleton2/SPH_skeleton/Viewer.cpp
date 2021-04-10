#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "Viewer.h"
#include "Windows.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

Viewer::Viewer(void)
{
	m_Zoom = 20;
	for (int i = 0; i < 2; i++)
	{
		m_Rotate[i] = 0.0f;
		m_Translate[i] = 0.0f;
		m_Mouse_Coord[i] = 0.0f;
	}
	m_Mouse_Event[0] = m_Mouse_Event[1] = m_Mouse_Event[2] = 0;
	m_start = false;
	m_capture = false;
}
bool interationMode = false;

Viewer::~Viewer(void)
{
}

void Viewer::Initialize(void)
{
	S_Simulator.Initialize();
	S_Simulator.ClothParticle();
	S_Simulator.Lighting();
}

void Viewer::Update()
{
	if (m_start == true) 
	{
		S_Simulator.Update();
	}
}

void Viewer::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -m_Zoom);

	glTranslatef(m_Translate[0], m_Translate[1], 0.0);
	glPushMatrix();
	glRotatef(m_Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(m_Rotate[1], 0.0, 1.0, 0.0);
	S_Simulator.Render(m_Rotate[0], m_Rotate[1]);

	glPopMatrix();

	float light_pos[] = { 150.0, 150.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glutSwapBuffers();
	//glutPostRedisplay();
}

void Viewer::Reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 500);
	//glOrtho(-20, 20, -20, 20, 0, 100);
	glViewport(0, 0, w, h);
	//glOrtho(-20.5, 20.5, -20.5, 20.5, 0, 100);
	//glutPostRedisplay();
}

void Viewer::Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
	case 'q':
	case 'Q':
		exit(0);
		break;
	case' ':
		m_start = !m_start;
		break;
	case 'r':
	case 'R':
		S_Simulator.Initialize();
		S_Simulator.ClothParticle();
		break;
	case 'e':
	case 'E':
		S_Simulator.Pouring();
		break;
	case'f':
	case'F':
		interationMode = !interationMode;
		break;
	case'd':
	case'D':
		if (m_start)
		S_Simulator.spherePos.x += 0.1;
		break;
	case'a':
	case'A':
		if (m_start)
		S_Simulator.spherePos.x -= 0.1;
		break;
	case'w':
	case'W':
		if (m_start)
		S_Simulator.spherePos.z -= 0.1;
		break;
	case's':
	case'S':
		if (m_start)
		S_Simulator.spherePos.z += 0.1;
		break;
	case'z':
	case'Z':
		m_capture = !m_capture;
		break;
	
	}
	glutPostRedisplay();
}

void Viewer::Mouse(int mouse_event, int state, int x, int y)
{
	m_Mouse_Coord[0] = x;
	m_Mouse_Coord[1] = y;
	switch (mouse_event)
	{
	case GLUT_LEFT_BUTTON:
		m_Mouse_Event[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		m_Mouse_Event[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		m_Mouse_Event[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

void Viewer::Motion(int x, int y)
{
	int diffx = x - m_Mouse_Coord[0];
	int diffy = y - m_Mouse_Coord[1];

	m_Mouse_Coord[0] = x;
	m_Mouse_Coord[1] = y;

	if (m_Mouse_Event[0])
	{
		if (!interationMode)
		{
			m_Rotate[0] += (float) 0.1 * diffy;
			m_Rotate[1] += (float) 0.1 * diffx;
		}
		else if (interationMode)
		{
			//Basic Implements 5. User Interaction
			glm::vec3 force = glm::vec3((float)diffx, (float)(-1.0f * diffy), 0.0);
			force = glm::rotateX(force, glm::radians(m_Rotate[0]));
			force = glm::rotateY(force, glm::radians(m_Rotate[1]));
			force.z = -1 * force.z;

			S_Simulator.cloth->add_force(vec3(force.x, force.y, force.z));
		}
	}
	else if (m_Mouse_Event[1])
	{
		m_Translate[0] += (float)0.1 * diffx;
		m_Translate[1] -= (float)0.1 * diffy;
	}
	else if (m_Mouse_Event[2])
	{
		m_Zoom += (float)0.1 * diffy;
	}
	glutPostRedisplay();
}
