#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#define GLM_FORCE_RADIANS // suppress a warning in GLM 0.9.5
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Parse
{
public:
	Parse();
	~Parse();
	std::vector<GLfloat> ParsePositionData(char* filepath);
};
