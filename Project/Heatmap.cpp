#include "Heatmap.h"



Heatmap::Heatmap()
{
}


Heatmap::~Heatmap()
{
}


std::vector<GLfloat> Heatmap::CreateHeatmap()
{
	//squares holds coords of squares
	std::vector<GLfloat> squares;
	GLfloat squarecoordtopleftx;
	GLfloat squarecoordtoplefty;
	GLfloat squarecoordbottomleftx;
	GLfloat squarecoordbottomlefty;
	GLfloat squarecoordtoprightx;
	GLfloat squarecoordtoprighty;
	GLfloat squarecoordbottomrightx;
	GLfloat squarecoordbottomrighty;
	//rgba values
	GLfloat r = 0.0f;
	GLfloat g = 0.0f;
	GLfloat b = 1.0f;
	GLfloat a = 0.0f;


	//z coord is always 110.65 in logs
	GLfloat z = 110.65/3000;
	//heatmap is 20x20
	GLfloat heatmapsize = 40;

	for (int i = 0; i < heatmapsize; i++)
	{
		for (int j = 0; j < heatmapsize; j++)
		{
			//determine points of squares
			squarecoordtopleftx = -1.0f + (i * (2 / heatmapsize));
			squarecoordtoplefty = 1.0f - (j * (2 / heatmapsize));
			squarecoordbottomleftx = squarecoordtopleftx;
			squarecoordbottomlefty = (1.0f - (j * (2 / heatmapsize))) - (2 / heatmapsize);
			squarecoordtoprightx = (-1.0f + (i * (2 / heatmapsize))) + (2 / heatmapsize);
			squarecoordtoprighty = squarecoordtoplefty;
			squarecoordbottomrightx = squarecoordtoprightx;
			squarecoordbottomrighty = squarecoordbottomlefty;

			//add the coordinates to the vector
			squares.push_back(squarecoordtopleftx);
			squares.push_back(squarecoordtoplefty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
			squares.push_back(squarecoordbottomleftx);
			squares.push_back(squarecoordbottomlefty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
			squares.push_back(squarecoordtoprightx);
			squares.push_back(squarecoordtoprighty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
			squares.push_back(squarecoordbottomrightx);
			squares.push_back(squarecoordbottomrighty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
			squares.push_back(squarecoordtoprightx);
			squares.push_back(squarecoordtoprighty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
			squares.push_back(squarecoordbottomleftx);
			squares.push_back(squarecoordbottomlefty);
			squares.push_back(z);
			squares.push_back(r);
			squares.push_back(g);
			squares.push_back(b);
			squares.push_back(a);
		}
	}

	return squares;
}

bool Heatmap::squareCheck(float left, float right, float top, float bottom)
{
	return true;
}
