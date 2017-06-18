#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "opengl.hpp"
#include "shader_program.hpp"
#include "simple_image.hpp"
#include "camera.hpp"
#include "world.hpp"
#include "agent.hpp"
#include "parkobject.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "quat.hh"

using namespace std;
using namespace cgra;

//Main window
GLFWwindow* window;
int width, height;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void returnNoiseLayout();
void renderSphere();
void renderCube();
void renderPlane();

World *g_world = nullptr;

vector<float> humanModel;
//****************************************************************************************************************************
//************************************** Set Up Camera System ****************************************************************
//****************************************************************************************************************************

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX, lastY;

//****************************************************************************************************************************
//************************************** Set up Animation System *************************************************************
//****************************************************************************************************************************

//timing
float deltaTime = 0.0f; //Time between current frame and last frame
float lastFrame = 0.0f;

bool firstMouse = true;
bool g_leftMouseDown;
int selectedItem = -1;
bool R_held = false;
float sunAngle = 90;
float sunYAngle = 0.0f;




//****************************************************************************************************************************
//************************************************* OBJECTS ******************************************************************
//****************************************************************************************************************************

//Set of Triangle vertices
// GLfloat vertices[] = {
//     // Positions          // Colors           // Texture Coords
//      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
//      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
//     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
// };

glm::vec3 baseTerrain[100][100];

std::vector<float> verts;
float vertList[100 * 100 * 3 * 3];

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,     0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

GLuint indices[] = {  // Note that we start from 0!
	0, 1, 3,   // First Triangle
	1, 2, 3    // Second Triangle
};


//****************************************************************************************************************************
//************************************************* METHODS ******************************************************************
//****************************************************************************************************************************


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}


}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (R_held) {
		if (selectedItem >= g_world->getAgents().size()) {
			g_world->getParkObjects()[selectedItem].changeRotation(xoffset);
		}
		return;
	}

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void processMouseClick() {
	// author: Jiaheng Wang
	vec3 camPos = vec3(camera.Position.x, camera.Position.y, camera.Position.z);
	vec3 dir = vec3(camera.Front.x, camera.Front.y, camera.Front.z);
	vec3 tg = camPos - (camPos.y / dir.y)*dir;
	vec2 ground = vec2(tg.x, tg.z);
	vector<Agent> *agents = &g_world->getAgents();
	if (selectedItem == -1 || selectedItem >= agents->size()) {
		float smallestDist = 3.402823466e+38F;
		for (int i = 0; i < agents->size(); i++) {
			float dist = length(agents->at(i).getPosition() - ground);
			if (dist < smallestDist) {
				smallestDist = dist;
				selectedItem = i;
			}
		}
		vector<ParkObject> objs = g_world->getParkObjects();
		for (int i = 0; i < objs.size(); i++) {
			float dist = length(objs[i].getPosition() - ground);
			if (dist < smallestDist) {
				smallestDist = dist;
				selectedItem = i + agents->size();
			}
		}
	}
	else {//agent already selected
		agents->at(selectedItem).setTarget(ground);
		agents->at(selectedItem).setIsRandom(false);
	}

}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	// cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		processMouseClick();
	}
}

void setupListeners() {
	//set the window to listen to key inputs
	glfwSetKeyCallback(window, key_callback);
	//Set up the cursor position listener
	glfwSetCursorPosCallback(window, mouse_callback);
	//Set up scroll listener
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

int setupMainWindow() {
	//Choose major and minor version of opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Use the core profile (less bits from old versions)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Required for mac to be forward compatible
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//Make the window NOT be resizeable
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);



	//Creates the window, and stores it.
	window = glfwCreateWindow(800, 600, "Physical Sky Dome", nullptr, nullptr);
	//If it fails to create then an error is thrown and the program stops
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Select the main window as the context with which to do things
	glfwMakeContextCurrent(window);
	//Make the window keep the cursor inside it and invisible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Use experimental/modern parts of glew and make sure it initialises
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}


	glfwGetFramebufferSize(window, &width, &height);
	//Create a viewport the size of the width and hieght of the frame buffer  
	glViewport(0, 0, width, height);
	lastX = width / 2.0f;
	lastY = height / 2.0f;

	setupListeners();



	return 0;
}


int main() {

	//initialise opengl
	glfwInit();
	returnNoiseLayout();

	//****************************************************************************************************************************
	//************************************** Initialise windows ******************************************************************
	//****************************************************************************************************************************

	if (setupMainWindow() < 0) {
		//initialisation failed
		return -1;
	}
	//****************************************************************************************************************************
	//************************************** Initialise Depth Settings ***********************************************************
	//****************************************************************************************************************************


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	//****************************************************************************************************************************
	//************************************** Initialise shaders and Textures *****************************************************
	//****************************************************************************************************************************

		//initialise shaders
	Shader primaryShader("./work/res/shaders/basic_shader.vert", "./work/res/shaders/basic_shader.frag");
	//Physical Shader
	Shader pbrShader("./work/res/shaders/physShdr.vert", "./work/res/shaders/physShdr.frag");
	//Equi Rectangular to Cube map shader
	Shader equirectangularToCubemapShader("./work/res/shaders/cubeMapShdr.vert", "./work/res/shaders/equiRectToCubeMapShdr.frag");
	//Irradiance Shader
	Shader irradianceShader("./work/res/shaders/cubeMapShdr.vert", "./work/res/shaders/irradianceConvultion.frag");
	//Background/Sky Shader
	Shader skyShader("./work/res/shaders/skyShdr.vert", "./work/res/shaders/skyShdr.frag");
	//BG Env Shader
	Shader backgroundShader("./work/res/shaders/bgEnvShader.vert", "./work/res/shaders/bgEnvShader.frag");



	pbrShader.use();
	pbrShader.setInt("irradianceMap", 0);
	pbrShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
	pbrShader.setFloat("ao", 1.0f);

	backgroundShader.use();
	backgroundShader.setInt("environmentMap", 0);



	// Image img = Image("./work/res/textures/brick.jpg");

	// GLuint texture;
	// glGenTextures(1, &texture);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// glBindTexture(GL_TEXTURE_2D, texture);



	// glTexImage2D(GL_TEXTURE_2D, 0, img.glFormat(), img.w, img.h, 0, img.glFormat(), GL_UNSIGNED_BYTE, img.dataPointer());
	// glGenerateMipmap(GL_TEXTURE_2D);

	// lights
	// ------
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;


	//****************************************************************************************************************************
	//************************************** Initialise Buffer Objects ***********************************************************
	//****************************************************************************************************************************


		// pbr: setup framebuffer
		// ----------------------
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf("./work/res/textures/newport_loft.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}


	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setInt("equirectangularMap", 0);
	equirectangularToCubemapShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
	pbrShader.use();
	pbrShader.setMat4("projection", projection);
	backgroundShader.use();
	backgroundShader.setMat4("projection", projection);
	skyShader.use();
	skyShader.setMat4("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	g_world = new World();
	g_world->init(-10.0f, -10.0f, 10.0f, 10.0f);

	humanModel = g_world->getAgents()[0].getModel();

	// //Set up example Vertex Buffer Object of a triangle
	// GLuint VBO;
	// glGenBuffers(1, &VBO); 
	// //Set up a Vertex Array Object
	// GLuint VAO;
	// glGenVertexArrays(1, &VAO);
	// //Set up element array object
	// GLuint EBO;
	// glGenBuffers(1, &EBO);

	// //Bind the VAO
	// glBindVertexArray(VAO);
	// //copy vertices array into a buffer for opengl to use
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// //Copy element indices into element buffer
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	// //Set the vertex attributes pointers going to the shader
	// //Setting the position attribute 
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	// glEnableVertexAttribArray(0);
	// //Setting the color attribute
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	// glEnableVertexAttribArray(1);
	// //Setting the texture coords attribute
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	// glEnableVertexAttribArray(2);



	// //Set up cube VBO
	// GLuint cubeVBO;
	// glGenBuffers(1,&cubeVBO);
	// //Set up a Vertex Array Object
	// GLuint cubeVAO;
	// glGenVertexArrays(1, &cubeVAO);
	// //Set up element array object
	// GLuint cubeEBO;
	// glGenBuffers(1, &cubeEBO);
	// //Bind the VAO
	// glBindVertexArray(cubeVAO);
	// //copy vertices array into a buffer for opengl to use
	// glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	// //Copy element indices into element buffer
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	// //Set the vertex attributes pointers going to the shader
	// //Setting the position attribute 
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	// glEnableVertexAttribArray(0);
	// // //Setting the color attribute
	// // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	// // glEnableVertexAttribArray(1);
	// //Setting the texture coords attribute
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	// glEnableVertexAttribArray(1);

	//clean up by unbinding the VAO
	//glBindVertexArray(0);


//****************************************************************************************************************************
//************************************** Set Up Coordinate Systems ***********************************************************
//****************************************************************************************************************************    

	//Creating model matrix
	// glm::mat4 model;
	// model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));    

	// //Creating view matrix
	// glm::mat4 view;
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

	// //Creating projectoion matrix
	// glm::mat4 projection;
	// projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);



//****************************************************************************************************************************
//************************************** Begin Main Loop *********************************************************************
//****************************************************************************************************************************








	//This is the main loop where all the action happens
	while (!glfwWindowShouldClose(window)) {

		//Update per frame info
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;




		processInput(window);

		//clear the pixel buffer and set bg to a color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		//Select shader program to use
		pbrShader.use();


		glm::mat4 view = camera.GetViewMatrix();;
		pbrShader.setMat4("view", view);
		pbrShader.setVec3("camPos", camera.Position);

		g_world->update();
		glm::mat4 model;
		vector<Agent> agents = g_world->getAgents();
		for (int i = 0; i < agents.size(); i++) {
			vec2 pos = agents[i].getPosition();
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(
				pos.x,
				-2.0f,
				pos.y
			));
			pbrShader.setMat4("model", model);
			renderSphere();
		}

		// vector<ParkObject> parkObjects = g_world->getParkObjects();
		// for (int i = 0; i < parkObjects.size(); i++) {
		// 	vec2 pos = parkObjects[i].getPosition();
		// 	model = glm::mat4();
		// 	model = glm::translate(model, glm::vec3(
		// 		pos.x,
		// 		-2.0f,
		// 		pos.y
		// 	));
		// 	pbrShader.setMat4("model", model);
		// 	renderSphere();
		// }
		/*for (int row = 0; row < nrRows; ++row)
		{
			pbrShader.setFloat("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					-2.0f,
					(float)(row - (nrRows / 2)) * spacing
				));
				pbrShader.setMat4("model", model);
				renderSphere();
			}
		}
		*/



		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.





		glm::vec3 newPos = lightPositions[0] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[0];
		float radius = 6.95f;
		float lightX = sin(glm::radians(sunAngle)) * radius;
		float lightY = sin(glm::radians(sunYAngle)) * radius;
		float lightZ = cos(glm::radians(sunAngle)) * radius;
		newPos = glm::vec3(lightX, lightY, lightZ);

		pbrShader.setVec3("lightPositions[" + std::to_string(0) + "]", newPos);
		pbrShader.setVec3("lightColors[" + std::to_string(0) + "]", lightColors[0]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		pbrShader.setMat4("model", model);
		
		//renderSphere();


		// render skybox (render as last to prevent overdraw)
		// backgroundShader.use();
		// backgroundShader.setMat4("view", view);

		// backgroundShader.setVec3("vPosition",camera.Position);
		// glm::vec3 thing = glm::vec3(0.0f,0.1f,-1.0f);
		// backgroundShader.setVec3("uSunPos",thing);

		skyShader.use();
		skyShader.setMat4("view", view);

		skyShader.setVec3("vPosition", camera.Position);
		glm::vec3 sunPos = glm::vec3(0.1f, sin(glfwGetTime()), cos(glfwGetTime()));
		//glm::vec3 sunPos = glm::vec3(0.1f, 0.1f, -1.0f);
		skyShader.setVec3("uSunPos", sunPos);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		renderCube();


		pbrShader.setMat4("projection",projection*view*model);
		pbrShader.use();
		renderPlane();	


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		//cout << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << endl;


		//t = distance along view direction
		//t = (dotProd(camOrigin,)
		//define a normal facing direction`

		//glm::mat4 projection;
		//projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

		// pass transformation matrices to the shader
		// primaryShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		// primaryShader.setMat4("view", view);

		// render boxes
		// glBindVertexArray(VAO);
		// for (unsigned int i = 0; i < 1; i++)
		// {
		//     // calculate the model matrix for each object and pass it to shader before drawing
		//     glm::mat4 model;
		//     //model = glm::translate(model, cubePositions[i]);
		//     float angle = 20.0f * i;
		//     //model = glm::rotate(model, glm::radians(angle)+(float)sin(glfwGetTime()), glm::vec3(1.0f, 0.3f, 0.5f));
		//     primaryShader.setMat4("model", model);

		//     glDrawArrays(GL_TRIANGLES, 3, sizeof(vertices));
		// }
		// //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glBindVertexArray(0);


		// //Swaps the buffers so that a full buffer can be drawn to then redisplayed, meaning no flicker
		// glfwSwapBuffers(window);
		// glfwPollEvents();
	}

	// glDeleteVertexArrays(1, &VAO);
	// glDeleteBuffers(1, &VBO);
	// glDeleteBuffers(1, &EBO);
	//properly clean up the program and exit
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float camSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		sunAngle--;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		sunAngle++;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		sunYAngle++;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		sunYAngle--;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		R_held = true;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		R_held = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void returnNoiseLayout() {

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			//baseTerrain[j][i] = glm::vec3(i/100,j/100,(float)sin(glfwGetTime()));
			//        (D-C)/(B-A)
			//         (X-A)*r+C        
			float xRatio = (2.0f) / (10.0f);
			float mappedXVal = ((float)i)*xRatio - 1.0f;

			float yRatio = (2.0f) / (10.0f);
			float mappedYVal = ((float)j)*yRatio - 1.0f;

			//baseTerrain[j][i] = glm::vec3(mappedXVal+xRatio/2.0f,mappedYVal+yRatio/2,(float)sin(glfwGetTime()));

			int a = (1 + j)*(1 + i);
			int b = (1 + j)*(1 + i) + 1;
			int c = (1 + j)*(1 + i) + 2;

			//cout << a << " " << b << " " << c << " "; 

			//setting vert x, y and z
			vertList[i + j + (i * 3)] = mappedXVal + xRatio / 2.0f;
			vertList[i + j + 1 + (i * 3)] = mappedYVal + yRatio / 2.0f;
			vertList[i + j + 2 + (i * 3)] = 0.0f;//(float)sin(glfwGetTime()/100);

			//setting color of vert
			// vertList[i+j+3+(i*6)] = 1.0f;
			// vertList[i+j+4+(i*6)] = 1.0f;
			// vertList[i+j+5+(i*6)] = 1.0f;

			// verts.push_back(mappedXVal+xRatio/2.0f);
			// verts.push_back(mappedYVal+yRatio/2.0f);
			// verts.push_back(0.0f);
		}

	}


	cout << verts.size();
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		// std::vector<glm::vec3> positions;
		// std::vector<glm::vec2> uv;
		// std::vector<glm::vec3> normals;
		// std::vector<unsigned int> indices;

		// const unsigned int X_SEGMENTS = 64;
		// const unsigned int Y_SEGMENTS = 64;
		// const float PI = 3.14159265359;
		// for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		// {
		// 	for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		// 	{
		// 		float xSegment = (float)x / (float)X_SEGMENTS;
		// 		float ySegment = (float)y / (float)Y_SEGMENTS;
		// 		float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
		// 		float yPos = std::cos(ySegment * PI);
		// 		float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

		// 		positions.push_back(glm::vec3(xPos, yPos, zPos));
		// 		uv.push_back(glm::vec2(xSegment, ySegment));
		// 		normals.push_back(glm::vec3(xPos, yPos, zPos));
		// 	}
		// }

		// bool oddRow = false;
		// for (int y = 0; y < Y_SEGMENTS; ++y)
		// {
		// 	if (!oddRow) // even rows: y == 0, y == 2; and so on
		// 	{
		// 		for (int x = 0; x <= X_SEGMENTS; ++x)
		// 		{
		// 			indices.push_back(y       * (X_SEGMENTS + 1) + x);
		// 			indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
		// 		}
		// 	}
		// 	else
		// 	{
		// 		for (int x = X_SEGMENTS; x >= 0; --x)
		// 		{
		// 			indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
		// 			indices.push_back(y       * (X_SEGMENTS + 1) + x);
		// 		}
		// 	}
		// 	oddRow = !oddRow;
		// }
		// indexCount = indices.size();

		// std::vector<float> data;
		// for (int i = 0; i < positions.size(); ++i)
		// {
		// 	data.push_back(positions[i].x);
		// 	data.push_back(positions[i].y);
		// 	data.push_back(positions[i].z);
		// 	if (uv.size() > 0)
		// 	{
		// 		data.push_back(uv[i].x);
		// 		data.push_back(uv[i].y);
		// 	}
		// 	if (normals.size() > 0)
		// 	{
		// 		data.push_back(normals[i].x);
		// 		data.push_back(normals[i].y);
		// 		data.push_back(normals[i].z);
		// 	}
		// }



		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, humanModel.size() * sizeof(float), &humanModel[0], GL_STATIC_DRAW);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawArrays(GL_TRIANGLES,0,20);
	//glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int planeVAO = 0;
unsigned int planeVBO = 0;
void renderPlane()
{
	// initialize (if necessary)
	if (planeVAO == 0)
	{
		float vertices[] = {
			    
			// bottom face
			-100.0f, -10.0f, -100.0f,  0.0f, -10.0f,  0.0f, 0.0f, 100.0f, // top-right
			 100.0f, -10.0f, -100.0f,  0.0f, -10.0f,  0.0f, 100.0f, 100.0f, // top-left
			 100.0f, -10.0f,  100.0f,  0.0f, -10.0f,  0.0f, 100.0f, 0.0f, // bottom-left
			 100.0f, -10.0f,  100.0f,  0.0f, -10.0f,  0.0f, 100.0f, 0.0f, // bottom-left
			-100.0f, -10.0f,  100.0f,  0.0f, -10.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-100.0f, -10.0f, -100.0f,  0.0f, -10.0f,  0.0f, 0.0f, 100.0f, // top-right
			      
		};
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(planeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}