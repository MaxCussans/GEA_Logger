#include "Parsing.h"



Parsing::Parsing()
{
}


Parsing::~Parsing()
{
}


//function to parse position log files
std::vector<GLfloat> Parsing::ParsePositionData(char* filepath)
{
	std::ifstream file;
	std::string buffer;
	std::string coord;
	std::vector<GLfloat> trajectory;
	GLfloat coordinatex;
	GLfloat coordinatey;
	GLfloat coordinatez;

	if (filepath != nullptr)
	{
		file.open(filepath);
	}
	if (file.is_open())
	{
		while (getline(file, buffer))
		{
			int xyz = 0;
			float x = 0;
			float y = 0;
			float z = 0;

			if (buffer == "Location ")
			{
				SDL_Log("Player Location parsing has begun!\n");
			}
			else
			{
				//first 4 characters are always "V(X=" so can be ignored
				for (int i = 4; i < buffer.size(); i++)
				{
					//ignore ',' and ')' characters
					if (buffer[i] != ',' && buffer[i] != ')')
					{
						//parse coordinate
						coord += buffer[i];
					}
					//after x coordinate has been parsed
					else if (buffer[i] == ',' && xyz == 0)
					{
						x = stof(coord);
						xyz++;
						//ignore the next 3 chars since they arent the next coordinate
						i += 3;
						coord = "";
					}
					//after y coordinate has been parsed
					else if (buffer[i] == ',' && xyz == 1)
					{
						y = stof(coord);
						xyz++;
						//ignore the next 3 chars since they arent the next coordinate
						i += 3;
						coord = "";
					}
					//end of line
					else if (buffer[i] == ')' && xyz == 2)
					{
						z = stof(coord);
						coord = "";
					}
				}

				coordinatex = (x / 3000) - 0.7;
				coordinatey = y / 3000;
				coordinatez = z / 3000;
				trajectory.push_back(coordinatex);
				trajectory.push_back(coordinatey);
				trajectory.push_back(coordinatez);

			}
		}
		//finished parsing
		file.close();
	}
	return trajectory;
}
