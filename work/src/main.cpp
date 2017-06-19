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
#include "mesh.hpp"
#include "tree.cpp"

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
void renderSphere();
void renderCube();
void renderPlane();
void renderModel(unsigned int mod);
void renderCylinder(float base_radius, float top_radius, float height, int slices, int stacks, bool wire);

World *g_world = nullptr;
std::vector<mesh> geometryList;


std::vector<float> humanModel;
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
//animation
bool firstRun = true;
bool animating = true;

bool firstMouse = true;
bool g_leftMouseDown;
int selectedItem = -1;
bool R_held = false;
bool addingAgent = false;
bool addingBench = false;
float sunAngle = 90;
float sunYAngle = 0.0f;




//****************************************************************************************************************************
//************************************************* OBJECTS ******************************************************************
//****************************************************************************************************************************






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
		if (selectedItem >= g_world->getAgents()->size()) {
			g_world->getParkObjects()->at(selectedItem).changeRotation(xoffset);
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
	if (addingAgent) {
		Agent a = Agent(human);
		a.setPosition(ground);
		g_world->addAgent(a);
		return;
	}
	else if (addingBench) {
		ParkObject a = ParkObject(bench, ground);
		g_world->addObject(a);
		return;
	}
	vector<Agent> *agents = g_world->getAgents();
	if (selectedItem == -1 || selectedItem >= agents->size()) {
		float smallestDist = 3.402823466e+38F;
		for (int i = 0; i < agents->size(); i++) {
			float dist = length((*agents)[i].getPosition() - ground);
			if (dist < smallestDist) {
				smallestDist = dist;
				selectedItem = i;
			}
		}
		vector<ParkObject> *objs = g_world->getParkObjects();
		for (int i = 0; i < objs->size(); i++) {
			float dist = length((*objs)[i].getPosition() - ground);
			if (dist < smallestDist) {
				smallestDist = dist;
				selectedItem = i + agents->size();
			}
		}
	}
	else {//agent already selected
		(*agents)[selectedItem].setIsRandom(false);
		(*agents)[selectedItem].setTarget(ground);
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
	//
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
	pbrShader.setVec3("albedo", 1.0f, 1.0f, 1.0f);
	pbrShader.setFloat("ao", 1.0f);

	backgroundShader.use();
	backgroundShader.setInt("environmentMap", 0);



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


	g_world = new World();
	g_world->init(-10.0f, -10.0f, 10.0f, 10.0f);



	geometryList.push_back(mesh("./work/res/models/park_bench.obj"));
	geometryList.push_back(mesh("./work/res/models/person.obj"));




	//****************************************************************************************************************************
	//************************************** Set Up Coordinate Systems ***********************************************************
	//****************************************************************************************************************************    





	//****************************************************************************************************************************
	//************************************** Begin Main Loop *********************************************************************
	//****************************************************************************************************************************

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int hdrTexture;

	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0); // note how we specify the texture's data value to be float

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);

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

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
	pbrShader.use();
	pbrShader.setMat4("projection", projection);
	backgroundShader.use();
	backgroundShader.setMat4("projection", projection);
	skyShader.use();
	skyShader.setMat4("projection", projection);



	Tree tree = Tree();

	glm::vec3 sunPos = glm::vec3(0.1f, 0.1f, -1.0f);

	//This is the main loop where all the action happens
	while (!glfwWindowShouldClose(window)) {








		if (firstRun || animating) {


			skyShader.use();
			skyShader.setMat4("projection", captureProjection);
			skyShader.setVec3("vPosition", glm::vec3(0.0f, 0.0f, 0.0f)); // view position
			sunPos = glm::vec3(0.1f, sin(glfwGetTime() / 10), cos(glfwGetTime() / 10));

			//glm::vec3 sunPos = glm::vec3(0.1f, 0.1f, -1.0f);
			skyShader.setVec3("uSunPos", sunPos);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);

			glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			for (unsigned int i = 0; i < 6; ++i)
			{
				skyShader.setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				renderCube();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);



			// then before rendering, configure the viewport to the original framebuffer's screen dimensions
			glViewport(0, 0, width, height);
			firstRun = false;
		}

		//Update per frame info
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//cout << deltaTime << endl;



		processInput(window);

		//clear the pixel buffer and set bg to a color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
























		//Select shader program to use
		pbrShader.use();


		glm::mat4 view = camera.GetViewMatrix();;
		pbrShader.setMat4("view", view);
		pbrShader.setVec3("camPos", camera.Position);
		pbrShader.setVec3("albedo", glm::vec3(0.5f, 0.5f, 0.5f));

		g_world->update();
		glm::mat4 model;
		glm::mat4 tempModel = model;
		vector<Agent> *agents = g_world->getAgents();
		for (int i = 0; i < agents->size(); i++) {
			vec2 pos = (*agents)[i].getPosition();
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(
				pos.x,
				-2.0f,
				pos.y
			));
			pbrShader.setMat4("model", model);
			//renderSphere();
			model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
			renderModel(1);
		}


		vector<ParkObject> *parkObjects = g_world->getParkObjects();
		for (int i = 0; i < parkObjects->size(); i++) {
			vec2 pos = (*parkObjects)[i].getPosition();
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(
				pos.x,
				1.0f,
				pos.y
			));
			pbrShader.setMat4("model", model);
			renderModel(0);
		}

		vector<Agent> *agentsList = g_world->getAgents();
		for (int i = 0; i < agentsList->size(); i++) {
			vec2 pos = (*agentsList)[i].getPosition();
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(
				pos.x,
				1.0f,
				pos.y
			));
			pbrShader.setMat4("model", model);
			renderModel(0);
		}



		
		//tree.show();
		model = tempModel;
		if(!tree.finishedGrowing){
			tree.grow();
		}
		for (int i = 0; i < tree.branches.size(); i++) {
			
			Branch b = tree.branches[i];
		if (b.parent != nullptr) {
			//glPushMatrix();
			model = glm::mat4();
			// ... Rotate branch
			vec3 dir = tree.branches[i].parent->position - tree.branches[i].position;
			vec3 target_dir = normalize( dir); // normalise the bones direction
			vec3 z_axis = vec3(0,0,1); // the default direction of a bone
			float rot_angle = acos((target_dir.x*z_axis.x) + (target_dir.y*z_axis.y) + (target_dir.z*z_axis.z)); //inverse of the dot product
			model = glm::translate(model, glm::vec3(tree.branches[i].position.x, tree.branches[i].position.y, tree.branches[i].position.z));
			//glTranslatef(tree.branches[i].position.x, tree.branches[i].position.y, tree.branches[i].position.z);
			cout << tree.branches[i].position.x << ", "  << tree.branches[i].position.y << ", " << tree.branches[i].position.z << endl;
			if( fabs(rot_angle) > 0) {
				vec3 cross = vec3((target_dir.y*z_axis.z)-(target_dir.z*z_axis.y), (target_dir.z*z_axis.x)-(target_dir.x*z_axis.z), (target_dir.x*z_axis.y)-(target_dir.y*z_axis.x)); // cross product
				vec3 rot_axis = normalize(cross); // normalise the cross product of the direction and staring vectors
				float rotateAmount = -rot_angle;
				cout << rotateAmount << endl;
				model = glm::rotate(model, glm::radians(rotateAmount), glm::vec3(rot_axis.x, rot_axis.y, rot_axis.z) );
				//glRotatef( -rot_angle/math::pi()*180, rot_axis.x, rot_axis.y, rot_axis.z ); // rotate so the branch draws in right direction
			}
			renderCylinder(1, 0.1, tree.branches[i].length, 6, 6, false);
			//cgraCylinder(0.1,0.1, tree.branches[i].length, 6, 6, false);
			model = glm::mat4();
		}
	}






		// for (int row = 0; row < nrRows; ++row)
		// {
		// 	pbrShader.setFloat("metallic", (float)row / (float)nrRows);
		// 	for (int col = 0; col < nrColumns; ++col)
		// 	{
		// 		// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
		// 		// on direct lighting.
		// 		pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

		// 		model = glm::mat4();
		// 		model = glm::translate(model, glm::vec3(
		// 			(float)(col - (nrColumns / 2)) * spacing,
		// 			-2.0f,
		// 			(float)(row - (nrRows / 2)) * spacing
		// 		));
		// 		pbrShader.setMat4("model", model);
		// 		renderSphere();

		// 	}
		// }


		//renderModel();
		//renderCylinder(0.5,0.5,4,10,10,false);

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		pbrShader.setVec3("albedo", glm::vec3(0.4f,0.8f,0.4f));
		pbrShader.setFloat("metallic", 0.1f);
		model = glm::mat4();
		renderPlane();


		skyShader.use();
		skyShader.setMat4("view", view);

		skyShader.setVec3("vPosition", camera.Position);
		//glm::vec3 sunPos = glm::vec3(0.1f, sin(glfwGetTime()), cos(glfwGetTime()));
		//glm::vec3 sunPos = glm::vec3(0.1f, 0.1f, -1.0f);
		skyShader.setVec3("uSunPos", sunPos);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		renderCube();
		//renderModel();





		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();


	}

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		animating = !animating;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		selectedItem = -1;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		addingAgent = true;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
		addingAgent = false;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		addingBench = true;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
		addingBench = false;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (selectedItem > -1) {
			if (selectedItem >= g_world->getAgents()->size()) {
				g_world->getParkObjects()->erase(g_world->getParkObjects()->begin() + (selectedItem - g_world->getAgents()->size()));
			}
			else {
				g_world->getAgents()->erase(g_world->getAgents()->begin() + selectedItem);
			}
			selectedItem = -1;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
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

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}



		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		// cout << sphereVAO << endl;
		// cout << vbo << endl;
	}

	glBindVertexArray(sphereVAO);
	//cout << "set everything Up" << endl;
	//glDrawArrays(GL_QUAD_STRIP,1,10608/8);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int modelVAO[5];
unsigned int modelIndexCount[5];
void renderModel(unsigned int mod)
{
	if (modelVAO[mod] == 0)
	{
		glGenVertexArrays(1, &modelVAO[mod]);

		unsigned int modelVBO, modelEBO;
		glGenBuffers(1, &modelVBO);
		glGenBuffers(1, &modelEBO);

		std::vector<cgra::vec3> positions = geometryList[mod].m_points;
		std::vector<cgra::vec2> uv = geometryList[mod].m_uvs;
		std::vector<cgra::vec3> normals = geometryList[mod].m_normals;
		std::vector<unsigned int> indices;

		std::vector<float> data;
		for (int i = 0; i < positions.size(); i++)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);

			data.push_back(uv[i].x);
			data.push_back(uv[i].y);


		}

		for (triangle t : geometryList[mod].m_triangles) {
			// cout << t.v.size() << endl;
			for (int i = 0; i < 3; i++) {
				indices.push_back(t.v[i].p);

			}

		}

		modelIndexCount[mod] = indices.size();
		//cout << modelIndexCount[mod] << endl;
		glBindVertexArray(modelVAO[mod]);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelIndexCount[mod] * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 3 + 2) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

	}



	glBindVertexArray(modelVAO[mod]);
	//cout << "set everything Up" << endl;
	//glDrawArrays(GL_TRIANGLE_STRIP,0,10608);
	//cout << modelIndexCount[mod] << endl;
	glDrawElements(GL_TRIANGLES, modelIndexCount[mod], GL_UNSIGNED_INT, 0);
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
			-10000.0f, -1.0f, -1000.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 10000.0f, -1.0f, -1000.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 10000.0f, -1.0f,  1000.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 10000.0f, -1.0f,  1000.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-10000.0f, -1.0f,  1000.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-10000.0f, -1.0f, -1000.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right

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
	// render Plane
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}



unsigned int cylinderVAO = 0;
unsigned int cylinderIndexCount;
void renderCylinder(float base_radius, float top_radius, float height, int slices = 10, int stacks = 10, bool wire = false)
{
	if (cylinderVAO == 0)
	{
		glGenVertexArrays(1, &cylinderVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;






		int dualslices = slices * 2;

		std::vector<float> sin_phi_vector;
		std::vector<float> cos_phi_vector;

		for (int slice_count = 0; slice_count <= dualslices; ++slice_count) {
			float phi = 2 * math::pi() * float(slice_count) / dualslices;
			sin_phi_vector.push_back(std::sin(phi));
			cos_phi_vector.push_back(std::cos(phi));
		}



		// thanks ben, you shall forever be immortalized
		float bens_theta = math::pi() / 2 * std::atan((base_radius - top_radius) / height);
		float sin_bens_theta = std::sin(bens_theta);
		float cos_bens_theta = std::cos(bens_theta);

		for (int stack_count = 0; stack_count <= stacks; ++stack_count) {
			float t = float(stack_count) / stacks;
			float z = height * t;
			float cylWidth = base_radius + (top_radius - base_radius) * t;

			for (int slice_count = 0; slice_count <= dualslices; ++slice_count) {
				// verts.push_back(vec3(
				// 	width * cos_phi_vector[slice_count],
				// 	width * sin_phi_vector[slice_count],
				// 	z));

				// norms.push_back(vec3(
				// 	cos_bens_theta * cos_phi_vector[slice_count],
				// 	cos_bens_theta * sin_phi_vector[slice_count],
				// 	sin_bens_theta));

				positions.push_back(glm::vec3(cylWidth * cos_phi_vector[slice_count], cylWidth * sin_phi_vector[slice_count], z));


				normals.push_back(glm::vec3(cos_bens_theta * cos_phi_vector[slice_count], cos_bens_theta * sin_phi_vector[slice_count], sin_bens_theta));


				uv.push_back(glm::vec2(0, 0));

			}
		}


		const unsigned int X_SEGMENTS = 128;
		const unsigned int Y_SEGMENTS = 128;
		const float PI = 3.14159265359;

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		cylinderIndexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			data.push_back(uv[i].x);
			data.push_back(uv[i].y);


			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);

		}




		glBindVertexArray(cylinderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		// cout << cylinderVAO << endl;
		// cout << vbo << endl;
	}

	glBindVertexArray(cylinderVAO);
	//cout << "set everything Up" << endl;
	//glDrawArrays(GL_QUAD_STRIP,1,10608/8);
	glDrawElements(GL_TRIANGLE_STRIP, cylinderIndexCount, GL_UNSIGNED_INT, 0);
}









