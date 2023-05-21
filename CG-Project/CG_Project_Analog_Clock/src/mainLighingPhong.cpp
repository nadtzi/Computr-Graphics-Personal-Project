//-----------------------------------------------------------------------------
// mainOBJModelLoading.cpp
//
// - Demo loading OBJ files
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //added for transform

#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"


// Global Variables
const char* APP_TITLE = " Computer Graphics Project - Analog Clock";
int gWindowWidth = 1200;
int gWindowHeight = 900;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;




//FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(1.0, 1.0, 1.0));
FPSCamera fpsCamera(glm::vec3(0.0f, 3.0f, 10.0f));

const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void Print_OpenGL_Version_Information();

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	//setting shaders
	ShaderProgram shaderProgram, shaderProgramOneTex;
	shaderProgram.loadShaders("res/shaders/lighting-phong.vert", "res/shaders/lighting-phong.frag");
	//shaderProgramOneTex.loadShaders("res/shaders/camera.vert", "res/shaders/texture.frag");

	//shader for light(lamps)
	ShaderProgram lightShader;
	lightShader.loadShaders("res/shaders/lamp.vert", "res/shaders/lamp.frag");
	// Load meshes and textures
	const int numModels = 8;
	Mesh mesh[numModels];
	Texture texture[numModels];

	mesh[0].loadOBJ("res/models/lampPost.obj");

	mesh[1].loadOBJ("res/models/lampPost.obj");
	mesh[2].loadOBJ("res/models/lampPost.obj");
	mesh[3].loadOBJ("res/models/Clock.obj");

	mesh[4].loadOBJ("res/models/table.obj");
	mesh[5].loadOBJ("res/models/tablelamp.obj");
	mesh[6].loadOBJ("res/models/floor.obj");
	mesh[7].loadOBJ("res/models/book.obj");

	Mesh lightmesh;
	lightmesh.loadOBJ("res/models/light.obj");

	texture[0].loadTexture("res/models/wand.png", true);

	texture[1].loadTexture("res/models/lamp_post_diffuse.png", true);
	texture[2].loadTexture("res/models/light_diffuse.jpg", true);
	texture[3].loadTexture("res/models/UVImageClock.png", true);
	
	texture[4].loadTexture("res/models/woodcrate_diffuse.jpg", true); //my table
	texture[5].loadTexture("res/models/UVImageLamp.png", true); //my lamp
	texture[6].loadTexture("res/models/floorTexture.jpg", true); //my floor
	texture[7].loadTexture("res/models/book.bmp", true); //book

	// Model positions
	glm::vec3 modelPos[] = {
		glm::vec3(2.5f,0.0f, 0.0f),	// secondhand
		glm::vec3(2.5f,0.0f, 0.0f),	// minutehand
		glm::vec3(2.5f,0.0f, 0.0f),	// hourhand
		glm::vec3(2.5f, 0.0f, 0.0f),	// clockface
		glm::vec3(-2.5f, -8.5f, -5.5f),	// table
		glm::vec3(2.5f, -0.8f, -2.5f),	// tablelamp
		glm::vec3(0.0f, -8.5f, 0.0f),	// floor
		glm::vec3(4.5f, -0.8f, -2.5f)	// book
	};

	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(0.18f, 0.18f, 0.18f),	// secondhand
		glm::vec3(0.12f, 0.12f, 0.12f),		// minutehand
		glm::vec3(0.08f, 0.08f, 0.08f),		// hourhand
		glm::vec3(0.01f, 0.01f, 0.01f),	// clockface
		glm::vec3(0.05f, 0.05f, 0.05f),	// table
		glm::vec3(0.1f, 0.1f, 0.1f),	// lamp
		glm::vec3(10.0f, 1.0f, 10.0f), //floor
		glm::vec3(1.0f, 1.0f, 1.0f)	// book

	};

	double lastTime = glfwGetTime();
	float secondangle = 0.0f, minuteAngle = 0.0f, hourAngle = 0.0f;

	//print card info
	Print_OpenGL_Version_Information();

	// Rendering loop 
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		int hour = abs(newtime.tm_hour % 12);
		int minute = newtime.tm_min;
		int sec = newtime.tm_sec;
		float anglesec = 0.0f;
		float anglemin = 0.0f;
		GLfloat hourangle = (float)((hour * -30) + 90);
		float anglehour = 0.0f;
		// Poll for and process events
		glfwPollEvents();
		update(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f);

		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		//set the light properties
		glm::vec3 lightPos(0.0f, 1.0f, 10.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

		//move light 
		secondangle += (float)deltaTime * 50.0f;
		lightPos.x = 10.0f * sinf(glm::radians(secondangle));

		shaderProgram.use();

		// Pass the matrices to the shader

		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);
		//for the lighting
		shaderProgram.setUniform("viewPos", viewPos);
		shaderProgram.setUniform("lightPos", lightPos);
		shaderProgram.setUniform("lightColor", lightColor);

		
		//second Hand
		// // Render the scene
	
			anglesec = (float)((sec * -6) );

			model = glm::rotate(glm::mat4(1.0), glm::radians(anglesec), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::mat4(1.0), modelPos[0]) * glm::scale(glm::mat4(1.0), modelScale[0]);
	
			shaderProgram.setUniform("model", model);

			texture[0].bind(0);		// set the texture before drawing.  
			mesh[0].draw();			// Render the OBJ mesh
			texture[0].unbind(0);
		

		// Minute Hand
		// Render the scene
		
			anglemin = (float)((minute * -6) );
				
			model = glm::rotate(glm::mat4(1.0), glm::radians(anglemin), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::mat4(1.0), modelPos[1]) * glm::scale(glm::mat4(1.0), modelScale[1]);
				
			shaderProgram.setUniform("model", model);

			texture[1].bind(0);		// set the texture before drawing
			mesh[1].draw();			// Render the OBJ mesh
			texture[1].unbind(0);
		

		//Hour Hand
		// // Render the scene
			anglehour = hourangle + ((minute / 15) * -6);
			
			model = glm::rotate(glm::mat4(1.0), glm::radians(anglehour), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::mat4(1.0), modelPos[2]) * glm::scale(glm::mat4(1.0), modelScale[2]);

			shaderProgram.setUniform("model", model);

			texture[2].bind(0);		// set the texture before drawing. 
			mesh[2].draw();			// Render the OBJ mesh
			texture[2].unbind(0);


		//Clock
		// Render the scene
	
			model = glm::translate(glm::mat4(1.0), modelPos[3]) * glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), modelScale[3]);
			shaderProgram.setUniform("model", model);

			texture[3].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[3].draw();			// Render the OBJ mesh
			texture[3].unbind(0);
		
			//table
	// Render the scene

			model = glm::translate(glm::mat4(1.0), modelPos[4])  * glm::scale(glm::mat4(1.0), modelScale[4]);
			shaderProgram.setUniform("model", model);

			texture[4].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[4].draw();			// Render the OBJ mesh
			texture[4].unbind(0);

			//lamp
// Render the scene

			model = glm::translate(glm::mat4(1.0), modelPos[5]) * glm::scale(glm::mat4(1.0), modelScale[5]);
			shaderProgram.setUniform("model", model);

			texture[5].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[5].draw();			// Render the OBJ mesh
			texture[5].unbind(0);


			//floor
// Render the scene

			model = glm::translate(glm::mat4(1.0), modelPos[6]) * glm::scale(glm::mat4(1.0), modelScale[6]);
			shaderProgram.setUniform("model", model);

			texture[6].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[6].draw();			// Render the OBJ mesh
			texture[6].unbind(0);

			//book
// Render the scene

			model = glm::translate(glm::mat4(1.0), modelPos[7]) * glm::scale(glm::mat4(1.0), modelScale[7]);
			shaderProgram.setUniform("model", model);

			texture[7].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[7].draw();			// Render the OBJ mesh
			texture[7].unbind(0);


		//draw the light
		model = glm::translate(glm::mat4(1.0), lightPos);
		lightShader.use();
		lightShader.setUniform("model", model);
		lightShader.setUniform("view", view);
		lightShader.setUniform("projection", projection);
		lightShader.setUniform("lightColor", lightColor);

		lightmesh.draw();

		glBindVertexArray(0);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}

	glfwTerminate();

	return 0;
}

//delete moved to mesh


//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	
	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.3f, 0.4f, 0.6f, 1.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Camera FPS movement

	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// Strafe left/right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	// Up/down
	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}
// Print OpenGL version information
void Print_OpenGL_Version_Information()
{
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion =
		glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);
}
