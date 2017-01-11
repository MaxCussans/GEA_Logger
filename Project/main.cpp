#define STRING2(x) #x
#define STRING(x) STRING2(x)

#if __cplusplus < 201103L
#pragma message("WARNING: the compiler may not be C++11 compliant. __cplusplus version is : " STRING(__cplusplus))
#endif
// end::C++11check[]

// tag::includes[]
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "src/Parse.h"
#include "src/Heatmap.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
// end::includes[]

// tag::using[]
// see https://isocpp.org/wiki/faq/Coding-standards#using-namespace-std
// don't use the whole namespace, either use the specific ones you want, or just type std::
using std::cout;
using std::endl;
using std::max;
// end::using[]


// tag::globalVariables[]
std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext
int frameCount = 0;
std::string frameLine = "";
char* filepath = "";
Parse parse;
Heatmap heatmap;
std::vector<Heatmap> heatmaps;
std::vector <GLfloat> coordinates;
std::vector <GLfloat> heatmapSquares;
bool showHeatmap = true;
GLfloat trajectoryr = 0.5f;
GLfloat trajectoryg = 0.5f;
GLfloat trajectoryb = 0.5f;
GLfloat heatmapr = 0.1f;
GLfloat heatmapg = 0.1f;
GLfloat heatmapb = 0.1f;
// end::globalVariables[]


// tag::vertexShader[]
//string holding the **source** of our vertex shader, to save loading from a file
const std::string strVertexShader = R"(
	#version 330
	in vec3 position;
	in vec4 vertexColor;
	out vec4 fragmentColor;
	void main()
	{
		 gl_Position = vec4(position, 1.0);
		 fragmentColor = vertexColor;
	}
)";
// end::vertexShader[]

// tag::fragmentShader[]
//string holding the **source** of our fragment shader, to save loading from a file
const std::string strFragmentShader = R"(
	#version 330
	in vec4 fragmentColor;
	out vec4 outputColor;
	void main()
	{
		  outputColor = fragmentColor;
	}
)";
// end::fragmentShader[]

// tag::ourVariables[]
//our variables
bool done = false;




//the color we'll pass to the GLSL //using different values from CPU and static GLSL examples, to make it clear this is working

										//our GL and GLSL variables

GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)
GLint positionLocation; //GLuint that we'll fill in with the location of the `position` attribute in the GLSL
GLint colorLocation; //GLuint that we'll fill in with the location of the `color` variable in the GLSL

GLuint vertexDataBufferObject;
GLuint vertexArrayObject;


GLuint heatmapVertexDataBufferObject;
GLuint heatmapVertexArrayObject;

// end::ourVariables[]


// end Global Variables
/////////////////////////

// tag::initialise[]
void initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	cout << "SDL initialised OK!\n";
}
// end::initialise[]

// tag::createWindow[]
void createWindow()
{
	//get executable name, and use as window title
	int beginIdxWindows = exeName.rfind("\\"); //find last occurrence of a backslash
	int beginIdxLinux = exeName.rfind("/"); //find last occurrence of a forward slash
	int beginIdx = max(beginIdxWindows, beginIdxLinux);
	std::string exeNameEnd = exeName.substr(beginIdx + 1);
	const char *exeNameCStr = exeNameEnd.c_str();

	//create window
	win = SDL_CreateWindow(exeNameCStr, 100, 100, 600, 600, SDL_WINDOW_OPENGL); //same height and width makes the window square ...

																				//error handling
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "SDL CreatedWindow OK!\n";
}
// end::createWindow[]

// tag::setGLAttributes[]
void setGLAttributes()
{
	int major = 3;
	int minor = 3;
	cout << "Built for OpenGL Version " << major << "." << minor << endl; //ahttps://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
																		  // set the opengl context version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //core profile
	cout << "Set OpenGL context to versicreate remote branchon " << major << "." << minor << " OK!\n";
}
// tag::setGLAttributes[]

// tag::createContext[]
void createContext()
{
	setGLAttributes();

	context = SDL_GL_CreateContext(win);
	if (context == nullptr) {
		SDL_DestroyWindow(win);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}
// end::createContext[]

// tag::initGlew[]
void initGlew()
{
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev) {
		std::cout << "GLEW Error: " << glewGetErrorString(rev) << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLEW Init OK!\n";
	}
}
// end::initGlew[]

// tag::createShader[]
GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	//error check
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}
// end::createShader[]

// tag::createProgram[]
GLuint createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}
// end::createProgram[]

// tag::initializeProgram[]
void initializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(createShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = createProgram(shaderList);
	if (theProgram == 0)
	{
		cout << "GLSL program creation error." << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLSL program creation OK! GLUint is: " << theProgram << std::endl;
	}

	positionLocation = glGetAttribLocation(theProgram, "position");
	colorLocation = glGetAttribLocation(theProgram, "vertexColor");
	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}
// end::initializeProgram[]

// tag::initializeVertexArrayObject[]
//setup a GL object (a VertexArrayObject) that stores how to access data and from where
void initializeTrajectoryVertexArrayObject(int index)
{
	glGenVertexArrays(1, &heatmaps[index].trajectoryVertObj); //create a Vertex Array Object
	cout << "Vertex Array Object created OK! GLUint is: " << heatmaps[index].trajectoryVertObj << std::endl;

	glBindVertexArray(heatmaps[index].trajectoryVertObj); //make the just created vertexArrayObject the active one

	glBindBuffer(GL_ARRAY_BUFFER, heatmaps[index].trajectoryVertBuff); //bind vertexDataBufferObject

	glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(0 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(3 * sizeof(GLfloat)));

	glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

						  //cleanup
	glDisableVertexAttribArray(positionLocation); //disable vertex attribute at index positionLocation
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

}
// end::initializeVertexArrayObject[]

// tag::initializeVertexBuffer[]
void initializeTrajectoryVertexBuffer(std::vector<GLfloat> data, int index)
{
	glGenBuffers(1, &heatmaps[index].trajectoryVertBuff);

	glBindBuffer(GL_ARRAY_BUFFER, heatmaps[index].trajectoryVertBuff);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.front(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "vertexDataBufferObject created OK! GLUint is: " << heatmaps[index].trajectoryVertBuff << std::endl;

	initializeTrajectoryVertexArrayObject(index);
}
// end::initializeVertexBuffer[]

void initializeHeatmapVertexArrayObject(int index)
{
	glGenVertexArrays(1, &heatmaps[index].vertObj); //create a Vertex Array Object
	cout << "Heatmap Vertex Array Object created GLUint is: " << heatmaps[index].vertObj << std::endl;

	glBindVertexArray(heatmaps[index].vertObj); //make the just created vertexArrayObject the active one

	glBindBuffer(GL_ARRAY_BUFFER, heatmaps[index].vertBuff); //bind vertexDataBufferObject

	glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation
	glEnableVertexAttribArray(colorLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(0 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(3 * sizeof(GLfloat)));

	glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

						  //cleanup
	glDisableVertexAttribArray(positionLocation); //disable vertex attribute at index positionLocation
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

}

void initializeHeatmapVertexBuffer(std::vector<GLfloat> data, int index)
{
	glGenBuffers(1, &heatmaps[index].vertBuff);

	glBindBuffer(GL_ARRAY_BUFFER, heatmaps[index].vertBuff);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.front(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "Heatmap vertexDataBufferObject created OK! GLUint is: " << heatmaps[index].vertBuff << std::endl;

	initializeHeatmapVertexArrayObject(index);

}

// tag::loadAssets[]
void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program, and get IDs of attributes and variables

	cout << "Loaded Assets OK!\n";
}
// end::loadAssets[]

// tag::handleInput[]
void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

					 //NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
						 //  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
				case SDLK_ESCAPE: done = true;
					break;

				case SDLK_SPACE:
					//show heatmap
					for (int x = 0; x < heatmaps.size(); x++)
					{
						//heatmapSquares = heatmap.CreateHeatmap();
						//heatmaps[x].vertBuff = heatmaps.size();
						//heatmaps[x].vertObj = heatmaps.size();
						//heatmaps[x].vertexData = heatmaps[x].CreateHeatmap();

						//for (int i = 0; i < heatmaps[x].data.size(); i += 3)
						//{
						//	for (int j = 0; j < heatmaps[x].vertexData.size(); j += 42)
						//	{
						//		//check if trajectory goes through square
						//		if ((/*right of left side*/heatmaps[x].data[i] >= heatmaps[x].vertexData[j] &&/*left of right side*/ heatmaps[x].data[i] <= heatmaps[x].vertexData[j + 14]) && (/*above bottom*/heatmaps[x].data[i + 1] >= heatmaps[x].vertexData[j + 8] &&/*lower than top*/ heatmaps[x].data[i + 1] <= heatmaps[x].vertexData[j + 1]))
						//		{
						//			heatmaps[x].vertexData[j + 6] += 0.01f;
						//			heatmaps[x].vertexData[j + 13] += 0.01f;
						//			heatmaps[x].vertexData[j + 20] += 0.01f;
						//			heatmaps[x].vertexData[j + 27] += 0.01f;
						//			heatmaps[x].vertexData[j + 34] += 0.01f;
						//			heatmaps[x].vertexData[j + 41] += 0.01f;
						//		}
						//	}
						//}


						initializeHeatmapVertexBuffer(heatmaps[x].vertexData, x);
						initializeTrajectoryVertexBuffer(heatmaps[x].data, x);
					}
					break;



				}
			break;

		case SDL_DROPFILE:
		{
			//when file is dropped on window store the coordinates in a vector of vec3s
			filepath = event.drop.file;
			Heatmap tempHeat = Heatmap();
			tempHeat.data = parse.ParsePositionData(filepath);
			
			heatmapSquares = heatmap.CreateHeatmap();
			tempHeat.vertBuff = heatmaps.size();
			tempHeat.vertObj = heatmaps.size();
			tempHeat.trajectoryVertBuff = heatmaps.size() + 1;
			tempHeat.trajectoryVertObj = heatmaps.size() + 1;


			tempHeat.vertexData = tempHeat.CreateHeatmap();

			if (showHeatmap == true)
			{
				for (int i = 0; i < tempHeat.data.size(); i += 7)
				{
					for (int j = 0; j < tempHeat.vertexData.size(); j += 42)
					{
						//check if trajectory goes through square
						if ((/*right of left side*/tempHeat.data[i] >= tempHeat.vertexData[j] &&/*left of right side*/ tempHeat.data[i] <= tempHeat.vertexData[j + 14]) && (/*above bottom*/tempHeat.data[i + 1] >= tempHeat.vertexData[j + 8] &&/*lower than top*/ tempHeat.data[i + 1] <= tempHeat.vertexData[j + 1]))
						{
							tempHeat.vertexData[j + 6] += 0.01f;
							tempHeat.vertexData[j + 13] += 0.01f;
							tempHeat.vertexData[j + 20] += 0.01f;
							tempHeat.vertexData[j + 27] += 0.01f;
							tempHeat.vertexData[j + 34] += 0.01f;
							tempHeat.vertexData[j + 41] += 0.01f;
						}
					}
				}
			}


			heatmaps.push_back(tempHeat);
			break;
		}
		}
	}
}
// end::handleInput[]

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	//WARNING - we should calculate an appropriate amount of time to simulate - not always use a constant amount of time
	// see, for example, http://headerphile.blogspot.co.uk/2014/07/part-9-no-more-delays.html

	//CHANGE ME
}
// end::updateSimulation[]

// tag::preRender[]
void preRender()
{
	glViewport(0, 0, 600, 600); //set viewpoint
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //set clear colour
	glClear(GL_COLOR_BUFFER_BIT); //clear the window (technical the scissor box bounds)
}
// end::preRender[]

// tag::render[]
void render()
{
	glUseProgram(theProgram); //installs the program object specified by program as part of current rendering state

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < heatmaps.size(); i++)
	{
		glBindVertexArray(heatmaps[i].vertObj);
		glDrawArrays(GL_TRIANGLES, 0, heatmaps[i].vertexData.size() / 7);
		glBindVertexArray(heatmaps[i].trajectoryVertObj);

		glLineWidth(5);
		//draw line for trajectory
		glDrawArrays(GL_LINE_STRIP, 0, heatmaps[i].data.size() / 7); //Draw Lines
	}

	
	
	
	glBindVertexArray(0);

	glUseProgram(0); //clean up

}
// end::render[]

// tag::postRender[]
void postRender()
{
	SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)
	frameLine += "Frame: " + std::to_string(frameCount++);
	cout << "\r" << frameLine << std::flush;
	frameLine = "";
}
// end::postRender[]

// tag::cleanUp[]
void cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}
// end::cleanUp[]

// tag::main[]
int main(int argc, char* args[])
{
	exeName = args[0];
	//setup
	//- do just once
	initialise();
	createWindow();

	createContext();

	initGlew();

	glViewport(0, 0, 600, 600); //should check what the actual window res is?

	SDL_GL_SwapWindow(win); //force a swap, to make the trace clearer

							//do stuff that only needs to happen once
							//- create shaders
							//- load vertex data
	loadAssets();

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		preRender();

		render(); // this should render the world state according to VARIABLES -

		postRender();

	}

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}