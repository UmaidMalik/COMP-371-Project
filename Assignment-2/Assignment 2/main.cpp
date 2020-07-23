/*
*	COMP 371 - Assignment 2
*
*	Umaid Malik 27576110
*	Daniel Thibault-Shea 40073133
*	Noor Alali
*	Vishal Senewiratne
*	Dominic Hart
*
*	CONTROLS:

*		VIEW SELECTION CONTROLS
*		==============================================================
*
*       ESCAPE              : QUIT PROGRAM
*
*		==============================================================
*

*		X					: MOVE NEGATIVE Y-DIRECTION
*
*		SPACE				: MOVE POSITIVE Y-DIRECTION
*
*		Q					: PERSPECTIVE PROJECTION
*
*		E					: ORTHOGRAPHIC PROJECTION
*
*		T					: RENDER TRIANGLES
*
*		L					: RENDER LINES
*
*		P					: RENDER POINTS
*
*		C					: HOLD TO DISABLE BACKFACE CULLING
*
*		HOME				: RETURN TO INITIAL POSTION
*
*		MOUSE SCROLL UP		: ZOOM IN
*
*		MOUSE SCROLL DOWN	: ZOOM OUT
*
*
*		PAN AND TILT (MOUSE MOVEMENT) WHILE HOLDING RIGHT MOUSE BUTTON
*		==============================================================
*
*		W		: MOVE FORWARD
*
*		S		: MOVE BACKWARD
*
*		A		: MOVE LEFT
*
*		D		: MOVE RIGHT
*
*		SHIFT	: INCREASE MOVEMENT SPEED
*
*		==============================================================
*
*
*		MODEL CONTROL
*		ENTER EITHER 1, 2, 3, 4 OR 5 TO SELECT MODELS - 0 WILL SELECT ALL MODELS - LEFT MOUSE CLICK WILL DESELECT ALL MODELS
*		====================================================================================================================
*
*		TAB						: TOGGLE BETWEEN INCREMENTAL OR CONTINUOUS MOVEMENT 
*		
*		W						: MOVE MODEL IN +Z DIRECTION
*
*		S						: MOVE MODEL IN -Z DIRECTION
*
*		A						: MOVE MODEL IN -X DIRECTION
*
*		D						: MOVE MODEL IN +X DIRECTION
*
*		X						: MOVE MODEL IN -Y DIRECTION
*
*		SPACE					: MOVE MODEL IN +Y DIRECTION
*
*		R						: RESET MODEL TO INITIAL POSITION
*
*		SHIFT + R				: RESET MODEL TO INITIAL SIZE
*
*		LCTRL + SHIFT + R		: RESET MODEL ORIENTATION
*
*		U						: UPSCALE MODEL
*
*		J						: DOWNSCALE MODEL
*
*		T						: ROTATE MODEL +X DIRECTION
*
*		Y						: ROTATE MODEL -X DIRECTION
*
*		G						: ROTATE MODEL +Y DIRECTION
*
*		H						: ROTATE MODEL -Y DIRECTION
*
*		B						: ROTATE MODEL +Z DIRECTION
*
*		N						: ROTATE MODEL -Z DIRECTION
*
*		SHIFT + W/A/S/D/X/SPACE	: INCREASE MOVEMENT SPEED
*
*		SHIFT + U/J				: INCREASE SCALING SPEED
*
*		ALT						: RANDOMIZE THE POSITION OF THE MODEL WITHIN THE GRID AREA
*
*		ARROW UP				: +Y ORIENTATION
*
*       ARROW DOWN				: -Y ORIENTATION
*
*		ARROW LEFT				: -X ORIENTATION
*
*		ARROW RIGHT				: +X ORIENTATION
*/

#include <iostream>
#include <algorithm>

#define GLEW_STATIC 1 // Allows for linking with Static Library on Windows 



#include <glad/glad.h>
#include <GLFW/glfw3.h> // provides interface for creating a graphical context					
#include <glm/glm.hpp>	// optimized math library with syntax similar to OpenGL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vector"

#include <sphere/Sphere.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>



const int numbObjInScene = 5;                           // make sure to update this if you add more models!!!

const int numGridLines = 100;                           // how many gridlines (going one way)

glm::vec3 initialCameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 initialcameraLookAt(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

glm::vec3 center(0.0f, 0.0f, 0.0f);

glm::vec3 cameraPosition = initialCameraPosition;
glm::vec3 cameraLookAt = initialcameraLookAt;

glm::mat4 identityMatrix = glm::mat4(1.0f);

glm::mat4 projectionMatrix = identityMatrix;
glm::mat4 viewMatrix = identityMatrix;
glm::mat4 worldMatrix = identityMatrix;


glm::mat4 rotationMatrix = identityMatrix;
glm::mat4 scalingMatrix = identityMatrix;
glm::mat4 shearingMatrix = identityMatrix;
glm::mat4 translationMatrix = identityMatrix;
glm::mat4 objectTranslationtionMatrix;
glm::mat4 partMatrix;

glm::mat4 modelTranslationMatrix;
glm::mat4 modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
glm::mat4 modelShearingMatrix;
glm::mat4 modelRotationMatrix;

// matrix for world oriention
glm::mat4 worldOrientationMatrix = identityMatrix;
glm::mat4 worldOrientation_X;
glm::mat4 worldOrientation_Y;
//

// used for rotation angle and rotation snap for world orientation set to 15 degrees
glm::vec3 worldRotation;
const float worldOrientation_ANGLE = 15.0f;

float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;
float cameraSpeed = 1.0f;
float cameraSpeedFast = 4 * cameraSpeed;
float deltaTime;
float fov = 90.0f;

double lastMousePosX, lastMousePosY;

// model parameters
float modelSpeed = 1.0f;
float shearingScale = 1.0f; // while the model is moving, it is sheared. When moving the model with (shift + movement), shearing is doubled 

bool ONE_KEY_PRESSED = false;
bool TWO_KEY_PRESSED = false;
bool THREE_KEY_PRESSED = false;
bool FOUR_KEY_PRESSED = false;
bool FIVE_KEY_PRESSED = false;

bool UP_KEY = false;
bool DOWN_KEY = false;
bool RIGHT_KEY = false;
bool LEFT_KEY = false;

bool T_KEY = GLFW_RELEASE;
bool Y_KEY = GLFW_RELEASE;
bool G_KEY = GLFW_RELEASE;
bool H_KEY = GLFW_RELEASE;
bool B_KEY = GLFW_RELEASE;
bool N_KEY = GLFW_RELEASE;

bool TAB_KEY = GLFW_RELEASE;
bool W_KEY = GLFW_RELEASE;
bool A_KEY = GLFW_RELEASE;
bool S_KEY = GLFW_RELEASE;
bool D_KEY = GLFW_RELEASE;
bool SPACE_KEY = GLFW_RELEASE;
bool X_KEY = GLFW_RELEASE;


bool ALT_KEY = GLFW_RELEASE; // key used to randomize the position of the model

float gridUnit = 0.1f; // used for incremental model translation


GLuint worldMatrixLocation;


// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);				// used for mouse scroll zoom in/out 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);		// used for toggling TAB key ON/OFF
// models
void model_A7();
void model_O9();
void model_S0();
void model_M6();
void model_N7();
void drawAxisLines();
void drawGridlines();
// control function for the models 
void modelControl(GLFWwindow* window, unsigned int* shaderProgram, const float& initSize, float& modelSize, glm::vec3& initPos,
	glm::vec3& modelPos, glm::vec3& rotation, glm::mat4& rotX, glm::mat4& rotY, glm::mat4& rotZ, const float ANGLE, bool KEY_PRESSED);

GLFWwindow* setupWindow();

// end of function protoypes


const char* getVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aColor;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 viewMatrix = mat4(1.0);" // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"out vec3 vertexColor;"
		"void main()"
		"{"
		"   vertexColor = aColor;"
		"	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
}

const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"out vec4 FragColor;"
		"in vec3 vertexColor;"
		"void main()"
		"{"
		" FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
		"}";
}

int compileAndLinkShaders()
{
	// compile and link shader program
	// return shader program id

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = getVertexShaderSource();
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//fragment shader
	unsigned int fragmentShader;
	const char* fragmentShaderSource = getFragmentShaderSource();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TURE);
	glfwWindowHint(GL_DEPTH_TEST, 24);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "COMP 371 - Team 6 - Part 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	return window;
}

GLuint VAO[numbObjInScene];	// vertexArrayObject
GLuint VBO[numbObjInScene];	// vertexBufferObject

int main()
{
	// Setup a window
	GLFWwindow* window = setupWindow();

	// Compile and link shaders
    unsigned int shaderProgram = compileAndLinkShaders();

	// defines vertices and color for the cube
	glm::vec3 vertexArray[] = {

		// cube made of 12 triangles

		// postion						// color
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),

		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
	};

	const float axisLineLength = 5 * (10.0f / (float)numGridLines);  // axis lines are the length of n grid squares

	// red axis line
	glm::vec3 redLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(axisLineLength, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	};

	// green axis line
	glm::vec3 greenLine[] = {
		glm::vec3(0.0f,  0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, axisLineLength, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
	};

	// blue axis line
	glm::vec3 blueLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, axisLineLength), glm::vec3(0.0f, 0.0f, 1.0f),
	};

	
	glGenVertexArrays(numbObjInScene, VAO);
	glGenBuffers(numbObjInScene, VBO);


	// cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));   // color attribute
	glEnableVertexAttribArray(1);


	// red line
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redLine), redLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// green line
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(greenLine), greenLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// blue line
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blueLine), blueLine, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	// grid lines
	const float spacing = 10.0f / (float)numGridLines;      // divide the 2.0 world into the number of gridlines
	const int numDataPoints = 8;                            // this is how many vec3's there are in one gridline (4 vertices with 1 color each)
	float increment = 0.0f;                                 // how much to move a line over
	glm::vec3 zLineColor = glm::vec3(1.0f, 1.0f, 0.0f);     // set line color for lines running parallel to z-axis
	glm::vec3 xLineColor = glm::vec3(1.0f, 1.0f, 0.0f);     // set line color for lines running parallel to x-axis
	glm::vec3 gridLines[numDataPoints * numGridLines];
	

	for (int i = 0; i < numGridLines; ++i) {
		// lines parallel to z-axis
		gridLines[i * numDataPoints ] = glm::vec3(-5.0f + increment, 0.0f, -5.0f);
		gridLines[i * numDataPoints + 1] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		gridLines[i * numDataPoints + 2] = glm::vec3(-5.0f + increment, 0.0f, 5.0f);
		gridLines[i * numDataPoints + 3] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		// lines parallel to x-axis
		gridLines[i * numDataPoints + 4] = glm::vec3(-5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 5] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);
		gridLines[i * numDataPoints + 6] = glm::vec3(5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 7] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);

		increment += spacing;
	}
	
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridLines), gridLines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
	

	// bind to nothing so we don't inadvertantly modify something
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(shaderProgram);

	projectionMatrix = glm::perspective(90.0f, 1024.0f / 768.0f, 0.0005f, 500.0f);

	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	// Variables to be used later in tutorial
	float angle = 0.0f;
	float rotationSpeed = 90.0f;  // degrees per second
	float lastFrameTime = glfwGetTime();
	

	// read mouse position
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);     // clockwise polygons are considered as front-facing

	// z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	
	// initial model parameters
	
	// model A7											// model O9											// model S0											
    const float init_A7_Size = 1.0f;					const float init_O9_Size = 1.0f;					const float init_S0_Size = 1.0f;	
    float model_A7_Size = 0.0f;							float model_O9_Size = 0.0f;							float model_S0_Size = 0.0f;
    glm::vec3 initPos_A7(-4.0f, 0.5f, -4.0f);			glm::vec3 initPos_O9(4.0f, 0.5f, -4.0f);			glm::vec3 initPos_S0(-4.0f, 0.5f, 4.0f);
    glm::vec3 model_A7_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_O9_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_S0_Position(0.0f, 0.0f, 0.0f);
    glm::vec3 A7_theta(0.0f, 0.0f, 0.0f);				glm::vec3 O9_theta(0.0f, 0.0f, 0.0f);				glm::vec3 S0_theta(0.0f, 0.0f, 0.0f);				
    glm::mat4 A7_rotation_X;							glm::mat4 O9_rotation_X;							glm::mat4 S0_rotation_X;							
    glm::mat4 A7_rotation_Y;							glm::mat4 O9_rotation_Y;							glm::mat4 S0_rotation_Y;							
    glm::mat4 A7_rotation_Z;							glm::mat4 O9_rotation_Z;							glm::mat4 S0_rotation_Z;							
	
    // model M6                                         // model N7										
    const float init_M6_Size = 1.0f;                    const float init_N7_Size = 1.0f;
    float model_M6_Size = 0.0f;                         float model_N7_Size = 0.0f;
    glm::vec3 initPos_M6(4.0f, 0.5f, 4.0f);             glm::vec3 initPos_N7(0.0f, 0.0f, 0.0f);
    glm::vec3 model_M6_Position(0.0f, 0.0f, 0.0f);      glm::vec3 model_N7_Position(0.0f, 0.0f, 0.0f);
    glm::vec3 M6_theta(0.0f, 0.0f, 0.0f);               glm::vec3 N7_theta(0.0f, 0.0f, 0.0f);
    glm::mat4 M6_rotation_X;                            glm::mat4 N7_rotation_X;
    glm::mat4 M6_rotation_Y;                            glm::mat4 N7_rotation_Y;
    glm::mat4 M6_rotation_Z;                            glm::mat4 N7_rotation_Z;

	const float ANGLE = 5.0f; // set rotation snap to 5 degrees
	
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{

		//  background color
		//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.28f, 0.68f, 0.82f, 1.0f);	// sky blue color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPointSize(10);		// increased point size so that it is easily visible
		glLineWidth(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);


		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;
		
		// beginning of model A7
		modelControl(window, &shaderProgram, init_A7_Size, model_A7_Size, initPos_A7, model_A7_Position, A7_theta, A7_rotation_X, A7_rotation_Y, A7_rotation_Z, ANGLE, ONE_KEY_PRESSED);
		model_A7();
		// end of model_A7();

		// beginning of model O9
		modelControl(window, &shaderProgram, init_O9_Size, model_O9_Size, initPos_O9, model_O9_Position, O9_theta, O9_rotation_X, O9_rotation_Y, O9_rotation_Z, ANGLE, TWO_KEY_PRESSED);
		model_O9();
		// end of model O9

		// beginning of model S0
		modelControl(window, &shaderProgram, init_S0_Size, model_S0_Size, initPos_S0, model_S0_Position, S0_theta, S0_rotation_X, S0_rotation_Y, S0_rotation_Z, ANGLE, THREE_KEY_PRESSED);
		model_S0();
		// end of model S0

		// beginning of model M6
		modelControl(window, &shaderProgram, init_M6_Size, model_M6_Size, initPos_M6, model_M6_Position, M6_theta, M6_rotation_X, M6_rotation_Y, M6_rotation_Z, ANGLE, FOUR_KEY_PRESSED);
		model_M6();
		// end of model M6

        // beginning of model N7
		modelControl(window, &shaderProgram, init_N7_Size, model_N7_Size, initPos_N7, model_N7_Position, N7_theta, N7_rotation_X, N7_rotation_Y, N7_rotation_Z, ANGLE, FIVE_KEY_PRESSED);
        model_N7();
        // end of model N7

		
		worldMatrix = worldOrientationMatrix;

        drawAxisLines();

        drawGridlines();
		
		
		// teardown: check and call events and swap the buffers
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Detect inputs
		processInput(window); 
		

	}

	// de-allocate all resources
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}


// resize window 
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Press L to render model in LINE mode
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Press T to render model in TRIANGLE mode
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Pess P to render model in POINT mode
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}


	// perspective - projection transform
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		projectionMatrix = glm::perspective(90.0f,			// field of view in degrees
			1024.0f / 768.0f,	// aspect ratio
			0.005f, 500.0f);	// near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far 

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}


	// Press and hold C to disable backface culling
	glEnable(GL_CULL_FACE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !GLFW_RELEASE)
	{
		glDisable(GL_CULL_FACE);
	}


	bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	float currentCameraSpeed = (fastCam) ? cameraSpeedFast : cameraSpeed;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE)) {

		// disable mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// detect mouse input
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		// change in position
		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// conversion to sperical coordinates
		const float cameraAngularSpeed = 5.0f;	// mouse speed
		cameraHorizontalAngle -= dx * cameraAngularSpeed * deltaTime;
		cameraVerticalAngle -= dy * cameraAngularSpeed * deltaTime;

		// limit range of vertical camera angle
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		// conversion to radians
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
		{
			cameraPosition += cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera back
		{
			cameraPosition -= cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera forward
		{
			cameraPosition.y += currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // move camera back
		{
			cameraPosition.y -= currentCameraSpeed * deltaTime;
		}


		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	// enable mouse cursor if right mouse button is released
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// reset to original world position and orientation
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		// reset camera position and lookAt
		cameraPosition = initialCameraPosition;
		cameraLookAt = initialcameraLookAt;

		// world angle/orientation
		worldOrientation_X = identityMatrix;
		worldOrientation_Y = identityMatrix;
		worldOrientationMatrix = identityMatrix;
		worldRotation = { 0.0f, 0.0f, 0.0f };

		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}


	// mouse scroll
	glfwSetScrollCallback(window, scroll_callback); // receives mouse scroll as input

	glfwSetKeyCallback(window, key_callback);
	


	// press 0 mouse button to select all models
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = true;
	}

	// press right mouse button to deselect all models
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 1 to select model A7
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 2 to select model O9
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 3 to select model S0 
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 4 to select model M6
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = false;
	}

	// press 5 to select model (your letter) 
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = true;
	}
	

	// while in model control, holding shift will increase movement speed or scaling speed
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
	{
		modelSpeed = 6.0f;
		shearingScale = 2.0f;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE))
	{
		modelSpeed = 1.0f;
		shearingScale = 1.0f;
	}

	if ((DOWN_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_DOWN))) {

		worldRotation.y -= glm::radians(worldOrientation_ANGLE);

		worldOrientation_Y = {
								  cos(worldRotation.y),  0, sin(worldRotation.y), 0,
											 0,  1,            0, 0,
								 -sin(worldRotation.y),  0, cos(worldRotation.y), 0,
											 0,  0,            0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	DOWN_KEY = glfwGetKey(window, GLFW_KEY_DOWN);

	if ((UP_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_UP))) {

		worldRotation.y += glm::radians(worldOrientation_ANGLE);

		worldOrientation_Y = {
								  cos(worldRotation.y),  0, sin(worldRotation.y), 0,
											 0,  1,            0, 0,
								 -sin(worldRotation.y),  0, cos(worldRotation.y), 0,
											 0,  0,            0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	UP_KEY = glfwGetKey(window, GLFW_KEY_UP);

	if ((LEFT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_LEFT))) {

		worldRotation.x -= glm::radians(worldOrientation_ANGLE);

		worldOrientation_X = {
								  1,            0,             0, 0,
								  0, cos(worldRotation.x), -sin(worldRotation.x), 0,
								  0, sin(worldRotation.x),  cos(worldRotation.x), 0,
								  0,            0,             0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	LEFT_KEY = glfwGetKey(window, GLFW_KEY_LEFT);

	

	if ((RIGHT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT))) {

		worldRotation.x += glm::radians(worldOrientation_ANGLE);

		worldOrientation_X = {
								  1,            0,             0, 0,
								  0, cos(worldRotation.x), -sin(worldRotation.x), 0,
								  0, sin(worldRotation.x),  cos(worldRotation.x), 0,
								  0,            0,             0, 1,
		};
		worldOrientationMatrix = worldOrientation_X * worldOrientation_Y;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	}	RIGHT_KEY = glfwGetKey(window, GLFW_KEY_RIGHT);
	
}

// used for mouse scroll zoom in/out
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		|| (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE))
	{

		// scroll up to zoom in 
		if (yoffset < 0) {
			std::cout << fov << std::endl;
			if (fov <= 120.0f)
			{
				fov = fov + 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
		// scroll down to zoom out 
		if (yoffset > 0) {
			std::cout << fov << std::endl;
			if (fov >= 10.0f)
			{
				fov = fov - 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
	}
}

// used for toggling TAB key ON/OFF 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_RELEASE) return;

	if (key == GLFW_KEY_TAB)
		TAB_KEY = !TAB_KEY;
}

void model_A7()
{	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);


	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;


	// end of model A7
}

void model_O9() {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// beginning of letter O
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of letter O

	// beginning of number 9
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.175f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//vertical
	//right
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.92f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, -2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.62f, 0.425f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of number 9

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;
	// end of model O9
}

void model_S0() {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.3f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// S
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.2f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.6f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ZERO
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.5f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -0.6f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// End of model S0

	//Reset 
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;
}

void model_M6() {

	// set object origin position
	objectTranslationtionMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.2f, 0.0));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	//draw letter M
		//left upright for letter M
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	//translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right upright for letter M
	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end of M

	//draw 6
		//vertical segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//horizontal segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.4f, 0.0f));
	partMatrix = objectTranslationtionMatrix * translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end 6	

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object.
	// also reset shearing matrix and scaling matrix, so that they work correctly with model_A7().
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	modelScalingMatrix = identityMatrix;
	modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
	worldMatrix = identityMatrix;
}

void model_N7()
{

    const float letterSpacing = 0.25f;
    const float left_of_origin = -0.5f;

    translationMatrix = identityMatrix;
    shearingMatrix = identityMatrix;
    scalingMatrix = scale(identityMatrix, glm::vec3(1.0f, 5.0f, 1.0f));

    // Left leg of 'N'
    translationMatrix[3][0] = left_of_origin;
    partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Right leg of 'N'
    translationMatrix[3][0] = left_of_origin + 0.35f;
    partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Diagonal of 'N'
    shearingMatrix[1][0] = -0.7f;
    partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // Diagonal base of '7'
    translationMatrix[3][0] = left_of_origin + 0.35f + letterSpacing;
    shearingMatrix[1][0] = 0.5f;
    partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Horizontal top of '7'
    scalingMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 2.5f, 1.0f));
    shearingMatrix[1][0] = 0.0f;
    translationMatrix[3][0] = left_of_origin + 0.6f + letterSpacing;
    translationMatrix[3][1] = 0.4f;
    glm::mat4 seven_top_rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * seven_top_rotate * scalingMatrix;
	worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
    modelScalingMatrix = identityMatrix;
    modelRotationMatrix = identityMatrix;
	modelShearingMatrix = identityMatrix;
    worldMatrix = identityMatrix;

    // end of model N7
}

void modelControl(GLFWwindow* window, unsigned int* shaderProgram, const float& initSize, float& modelSize, glm::vec3& initPos,
	glm::vec3& modelPos, glm::vec3& rotation, glm::mat4& rotX, glm::mat4& rotY, glm::mat4& rotZ, const float ANGLE, bool KEY_PRESSED) {

	rotation = { rotation.x, rotation.y, rotation.z };
	modelRotationMatrix = rotX * rotY * rotZ;
	modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
	worldMatrixLocation = glGetUniformLocation(*shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);


	if (KEY_PRESSED)
	{
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            modelSize += deltaTime * modelSpeed;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			modelSize -= deltaTime * modelSpeed;

		switch (TAB_KEY) {
			// continuous model movement
		case (false) :
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
					modelPos.x -= deltaTime * modelSpeed;
					modelShearingMatrix[1][0] = 0.5f * shearingScale;
				}
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
					modelPos.x += deltaTime * modelSpeed;
					modelShearingMatrix[1][0] = -0.5f * shearingScale;
				}
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
					modelPos.z -= deltaTime * modelSpeed;
					modelShearingMatrix[1][2] = 0.5f * shearingScale;
				}
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
					modelPos.z += deltaTime * modelSpeed;
					modelShearingMatrix[1][2] = -0.5f * shearingScale;
				}
				if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
					modelPos.y += deltaTime * modelSpeed;
				if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
					modelPos.y -= deltaTime * modelSpeed;
				break;

			// incremental translation of 1 grid unit length of 0.1;
		case (true) :
				if ((A_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_A))) {
					modelShearingMatrix[1][0] = 0.5f * shearingScale;
					modelPos.x -= gridUnit;

				}	A_KEY = glfwGetKey(window, GLFW_KEY_A);

				if ((D_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_D))) {
					modelShearingMatrix[1][0] = -0.5f * shearingScale;
					modelPos.x += gridUnit;

				}	D_KEY = glfwGetKey(window, GLFW_KEY_D);

				if ((W_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_W))) {
					modelShearingMatrix[1][2] = 0.5f * shearingScale;
					modelPos.z -= gridUnit;

				}	W_KEY = glfwGetKey(window, GLFW_KEY_W);

				if ((S_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_S))) {
					modelShearingMatrix[1][2] = -0.5f * shearingScale;
					modelPos.z += gridUnit;

				}	S_KEY = glfwGetKey(window, GLFW_KEY_S);

				if ((SPACE_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_SPACE))) {
					modelPos.y += gridUnit;

				}	SPACE_KEY = glfwGetKey(window, GLFW_KEY_SPACE);

				if ((X_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_X))) {
					modelPos.y -= gridUnit;

				}	X_KEY = glfwGetKey(window, GLFW_KEY_X);
				break;
		}
		

		// defined range range of random numbers (-5.0f to 5.0f)
		float randomPos_X = ((float(rand()) / RAND_MAX) * 10.0f) - 5.0f;
		float randomPos_Z = ((float(rand()) / RAND_MAX) * 10.0f) - 5.0f;
		if ((ALT_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_LEFT_ALT))) {
			std::cout << "Model position randomized - X: " << randomPos_X <<" Z: "<< randomPos_Z << std::endl;
			modelPos = { randomPos_X - initPos.x, 0.0f, randomPos_Z - initPos.z};

		}	ALT_KEY = glfwGetKey(window, GLFW_KEY_LEFT_ALT);

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
			modelPos = { 0.0f, 0.0f, 0.0f };
		if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
			modelSize = 0.0f;

		if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T)))	// rotate in +y axis
			rotation.x += glm::radians(ANGLE);
		T_KEY = glfwGetKey(window, GLFW_KEY_T);

		if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y)))	// rotate in -y axis
			rotation.x -= glm::radians(ANGLE);
		Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

		if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G))) 	// rotate in +x axis
			rotation.y += glm::radians(ANGLE);
		G_KEY = glfwGetKey(window, GLFW_KEY_G);

		if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H))) 	// rotate in -x axis
			rotation.y -= glm::radians(ANGLE);
		H_KEY = glfwGetKey(window, GLFW_KEY_H);

		if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B)))	// rotate in +z axis
			rotation.z += glm::radians(ANGLE);
		B_KEY = glfwGetKey(window, GLFW_KEY_B);

		if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
			rotation.z -= glm::radians(ANGLE);
		N_KEY = glfwGetKey(window, GLFW_KEY_N);

		if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS)	// reset model orientation
			rotation = { 0.0f, 0.0f, 0.0f };  // set theta back to zero

		rotX = {
								  1,            0,             0, 0,
								  0, cos(rotation.x), -sin(rotation.x), 0,
								  0, sin(rotation.x),  cos(rotation.x), 0,
								  0,            0,             0, 1,
		};

		rotY = {
								  cos(rotation.y),  0, sin(rotation.y), 0,
											 0,  1,            0, 0,
								 -sin(rotation.y),  0, cos(rotation.y), 0,
											 0,  0,            0, 1,
		};

		rotZ = {
					   cos(rotation.z), -sin(rotation.z), 0, 0,
					   sin(rotation.z),  cos(rotation.z), 0, 0,
								  0,             0, 1, 0,
								  0,             0, 0, 1,
		};


		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos.x + modelPos.x, initPos.y + modelPos.y, initPos.z + modelPos.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelSize + initSize, modelSize + initSize, modelSize + initSize));
		modelRotationMatrix = rotX * rotY * rotZ;
		worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelShearingMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	}
	

}

void drawAxisLines() {

	
    // red line
    glLineWidth(5);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    // green line
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    // blue line
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);
	
	
}

void drawGridlines() {
    glLineWidth(1);
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	//worldMatrix = worldOrientationMatrix * modelTranslationMatrix * modelScalingMatrix * modelRotationMatrix * partMatrix;
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2 * 2 * numGridLines);

	//worldMatrix = identityMatrix;
	
}
