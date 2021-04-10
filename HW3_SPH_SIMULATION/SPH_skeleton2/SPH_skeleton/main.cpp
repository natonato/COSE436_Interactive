#pragma once
#include <iostream>
#include "Viewer.h"

using namespace std;

Viewer OpenGL_Viewer;
int frame;
void saveImg(int frame);
void ScreenCapture(char* imgName);
void Initialize(void)
{
	OpenGL_Viewer.Initialize();
	frame = 0;
}

void Render(void)
{
	OpenGL_Viewer.Render();
}

void Reshape(int w, int h)
{
	OpenGL_Viewer.Reshape(w, h);
}

void Mouse(int mouse_event, int state, int x, int y)
{
	OpenGL_Viewer.Mouse(mouse_event, state, x, y);
}

void Motion(int x, int y)
{
	OpenGL_Viewer.Motion(x, y);
}

void Keyboard(unsigned char key, int x, int y)
{
	OpenGL_Viewer.Keyboard(key, x, y);
}

void Update(int value)
{
	OpenGL_Viewer.Update();
	glutPostRedisplay();
	if(OpenGL_Viewer.m_start && OpenGL_Viewer.m_capture)saveImg(frame++);
	glutTimerFunc(10, Update, 0);
}

int main(int argc, char ** argv)
{
	//InitGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Smoothed-Particle Hydrodynamics Simulation");

	glEnable(GL_DEPTH_TEST);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

	Initialize();
	glutTimerFunc(10, Update, 0);
	glutDisplayFunc(Render);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);

	glutMainLoop();


	return 0;
}
void saveImg(int frame) {
	char* imgName = new char[80];
	if (frame < 10) {
		sprintf(imgName, "c:\\0interactive\\img\\test\\result_0000%d.bmp", frame);
	}
	else if (frame < 100) {
		sprintf(imgName, "c:\\0interactive\\img\\test\\result_000%d.bmp", frame);

	}
	else if (frame < 1000) {
		sprintf(imgName, "c:\\0interactive\\img\\test\\result_00%d.bmp", frame);

	}
	else if (frame < 10000) {
		sprintf(imgName, "c:\\0interactive\\img\\test\\result_0%d.bmp", frame);

	}
	else {
		sprintf(imgName, "c:\\0interactive\\img\\test\\result_%d.bmp", frame);

	}
	ScreenCapture(imgName);
}

void ScreenCapture(char* imgName) {
	int width = 0;
	int height = 0;
	RECT ImageRect;
	GLbyte *pPixel = NULL;
	HANDLE hSavefile = NULL;


	BITMAPFILEHEADER BMFH;
	BITMAPINFOHEADER BMIH;

	DWORD RealWidthBytes = 0;
	DWORD dwWrite = 0;

	width = 800;
	height = 800;

	RealWidthBytes = (width * 4 * 5) ? ((width * 4) + (5 - (width * 4 % 5))) : (width * 4);

	pPixel = (GLbyte *)malloc(width * 4 * height);
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixel);
	wchar_t wtext[80];
	mbstowcs(wtext, imgName, strlen(imgName) + 1);
	LPWSTR ptr = wtext;
	hSavefile = CreateFile(ptr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	BMFH.bfType = 0x4d42;
	BMFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (RealWidthBytes*height);

	BMFH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	WriteFile(hSavefile, &BMFH, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);

	BMIH.biBitCount = 24;
	BMIH.biClrImportant = 0;
	BMIH.biClrUsed = 0;
	BMIH.biCompression = BI_RGB;
	BMIH.biHeight = height;
	BMIH.biPlanes = 1;
	BMIH.biSize = sizeof(BITMAPINFOHEADER);
	BMIH.biSizeImage = 0;
	BMIH.biWidth = width;
	BMIH.biXPelsPerMeter = 0;
	BMIH.biYPelsPerMeter = 0;


	WriteFile(hSavefile, &BMIH, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	WriteFile(hSavefile, pPixel, (RealWidthBytes*height), &dwWrite, NULL);

	CloseHandle(hSavefile);
	free(pPixel);

}