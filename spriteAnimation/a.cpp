
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h> 
#include <time.h> 
#include <freeglut.h>

#include "RGBApixMap.h"
#include "wav.h"
#include <vector>

using namespace std;

mRGBA temp;
RGBApixmap denoise;
RGBApixmap orignal;


void myInit(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL, 1.0); // accept fragment if fragment alpha = reference alpha i.e. 1.0
								//or glAlphaFunc(GL_NOTEQUAL,0.0);// accept fragment if fragment alpha not= reference alpha i.e. 1.0


	glViewport(0, 0, 1010, 670);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, 1010, 0, 670);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



}
void drawPoint(int x, int y)
{
	glPointSize(21.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}



void Rectangles(void) {
	glStencilFunc(GL_NEVER, 1, 0xFF);		//GL_ALWAYS is comparision func

	glBegin(GL_POLYGON);					//1st button
	glColor3f(0.6, 0.8, 1.0);
	glVertex2f(100, 190);
	glVertex2f(160, 190);
	glVertex2f(160, 240);
	glVertex2f(100, 240);
	glEnd();
	glFlush;

	
}
void insertionSort(unsigned char arr[], int n)
{

	int ii, key, jj;
	for (ii = 1; ii < n; ii++)
	{
		key = arr[ii];
		jj = ii - 1;

		/* Move elements of arr[0..i-1], that are
		greater than key, to one position ahead
		of their current position */
		while (jj >= 0 && arr[jj] > key)
		{
			arr[jj + 1] = arr[jj];
			jj = jj - 1;
		}
		arr[jj + 1] = key;
	}
}

void median(RGBApixmap pix2, RGBApixmap pix1)
{
	vector<vector<mRGBA>> pixels2;

	for (size_t i = 0; i < pix2.nCols; i++)
	{
		vector<mRGBA> row2;
		for (size_t j = 0; j < pix2.nRows; j++)
		{
			temp = pix2.getPixel(i, j);
			row2.push_back(temp);
		}

		pixels2.push_back(row2);
	}

	unsigned char window[9];
	unsigned char window1[9];
	unsigned char window2[9];

	for (size_t row = 1; row < pix2.nCols - 1; row++)
	{
		for (size_t col = 1; col < pix2.nRows - 1; col++)
		{
			//neighbor pixel values are stored in window including this pixel 
			window[0] = pixels2[row - 1][col - 1].r;
			window[1] = pixels2[row - 1][col].r;
			window[2] = pixels2[row - 1][col + 1].r;
			window[3] = pixels2[row][col - 1].r;
			window[4] = pixels2[row][col].r;
			window[5] = pixels2[row][col + 1].r;
			window[6] = pixels2[row + 1][col - 1].r;
			window[7] = pixels2[row + 1][col].r;
			window[8] = pixels2[row + 1][col + 1].r;

			window1[0] = pixels2[row - 1][col - 1].g;
			window1[1] = pixels2[row - 1][col].g;
			window1[2] = pixels2[row - 1][col + 1].g;
			window1[3] = pixels2[row][col - 1].g;
			window1[4] = pixels2[row][col].g;
			window1[5] = pixels2[row][col + 1].g;
			window1[6] = pixels2[row + 1][col - 1].g;
			window1[7] = pixels2[row + 1][col].g;
			window1[8] = pixels2[row + 1][col + 1].g;

			window2[0] = pixels2[row - 1][col - 1].b;
			window2[1] = pixels2[row - 1][col].b;
			window2[2] = pixels2[row - 1][col + 1].b;
			window2[3] = pixels2[row][col - 1].b;
			window2[4] = pixels2[row][col].b;
			window2[5] = pixels2[row][col + 1].b;
			window2[6] = pixels2[row + 1][col - 1].b;
			window2[7] = pixels2[row + 1][col].b;
			window2[8] = pixels2[row + 1][col + 1].b;

			//sort window array 
			insertionSort(window, 9);
			insertionSort(window1, 9);
			insertionSort(window2, 9);

			//put the median to the new array 
			//arr[row][col] = window[4];
			pix1.setPixel(row, col, mRGBA(window[4], window1[4], window2[4], 255));
		}
	}



	glRasterPos2i(56, 68);
	pix2.mDraw();
	glRasterPos2i(555, 61);
	pix1.mDraw();


}

void MAINSCENE()
{
	RGBApixmap pix;

	pix.readBMPFile("LBBG.bmp");
	glRasterPos2i(0, 0);
	pix.mDraw();

}

void rect()
{
	//Slidebar(200, 200, 640 / 2, 480 / 2);
	myInit();
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MAINSCENE();
	denoise.readBMPFile("bg.bmp");
	orignal.readBMPFile("lena.bmp");
	median(orignal, denoise);
	glFlush();

	
}
void MouseButton(int button, int action, int xPixel, int yPixel) {

	if (button == GLUT_LEFT_BUTTON) {
		yPixel = glutGet(GLUT_WINDOW_HEIGHT) - yPixel;

		int * index = new int[1];
		//Index1
		glReadPixels(xPixel, yPixel, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, index);// Read pixel under mouse's cursor and read information from stencil buffer to index when it rectangle index should be equal to 1


		if (index[0] == 1) {
			rect();
			
		}
		//Index 2
		//  glReadPixels(xPixel, yPixel, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index2);

		//else if (index[0] == 2) {
			//circleBB();
		//}

		delete index;
	}

	glutPostRedisplay();
}



void display(void)
{
	glFlush();
}
void scenemain()
{
	RGBApixmap pix;
	pix.readBMPFile("denoisertitle.bmp");
	glRasterPos2i(0, 0);
	pix.mDraw();
}


void vkvv(void)
{
	scenemain();

	glClearStencil(0); // specifies the index used by glClear to clear the stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Rectangles();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);


}


void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_SINGLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(1000, 665);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("RGBA and Sound Demo");
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	myInit();
	vkvv();
	glutDisplayFunc(display);
	//glutTimerFunc(0, myTimer, 0);
	//glutIdleFunc(display);
	glutMouseFunc(&MouseButton);
	glutMainLoop();
}

