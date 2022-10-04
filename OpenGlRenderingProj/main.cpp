
#include<iostream>
#include<glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<random>
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include <vector>
#include "New_Fluid.h"

//Declare global variables
const int arraysize = N;
const unsigned int width = 1920;
const unsigned int height = 1080;
float rotation;
float rotationvert;
float translateX;
float translateY;
float translateZ;

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
float vertexArray[] = {
	 1.0f,  1.0f, 0.0f,  // top right
	 1.0f, -1.0f, 0.0f,  // bottom right
	-1.0f, -1.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f   // top left 
};
unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};


int main()
{
	// Initialize GLFW
	glfwInit();
	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create a GLFWwindow object of desired width and height
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Fluid Simulation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 1920, y = 1080
	glViewport(0, 0, 1920, 1080);
	//Vertex Array Object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//VertexBuffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
	//Element Buffer Array
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//Set Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(0);
	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

double prevTime = glfwGetTime();

// Enables the Depth Buffer
glEnable(GL_DEPTH_TEST);
// set n from globals
const int n = arraysize;
//INIT FLUID
// update sim timestep
float dt = 0.01f;
// interval between each grid
float dx = 0.9f;
float damping = 0.9999999999f;
// number of iteration in Gauss-Sidel method
int p_solver_iters = 100;
//Vector field and scalar field struct type declarations, defined in New_Fluid.h
VectorField vel;
VectorField new_vel;
VectorField vorticity_force;
ScalarField divergence;
ScalarField curl;
ScalarField pressure;
ScalarField new_pressure;
ScalarField dye;
ScalarField new_dye;
// Main while loop


while (!glfwWindowShouldClose(window))
{
	// Simple timer
	double crntTime = glfwGetTime();
	//Create velocity source
	
	//Create dye source
	dye.v[IXY(n / 2, n/2 + 1, n / 2, n)] = 1;
	vel.y[IXY((n / 2), n/2, (n / 2), n)] = -200;
	//vorticity_confinement(n, vel, curl, vorticity_force, dt);
		
	
			
		
	//Pushes velocity vectors 'downstream'
	advection_velocity(n, vel, new_vel, dt, damping);
	//Pushes dye scalar 'downstream'
	advection(n, vel, dye,new_dye, dt, damping);
	vel.swap(new_vel);
	dye.swap(new_dye);
	
	//get divergence for pressure gradient
	get_divergence(N, vel, divergence);
	
	
	
	for (int i = 0; i < p_solver_iters; i++)
	{
		pressure_gauss_sidel(n, divergence, pressure, new_pressure);
		pressure.swap(new_pressure);
	}
	// apply pressure to velocity field
    subtract_gradient(n ,vel, pressure);
	//Iterative solver to remove divergence and get curl

	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static GLuint volume_tex = 0;

	//create grid of random numbers:
	int tex_width = N;
	int tex_height = N;
	int tex_depth = N;
	int tex_size = tex_width * tex_height * tex_depth;
	float* volume_data = new float[tex_size];
	for (int i = 0; i < tex_size; i++) {
		volume_data[i] =  dye.v[i] * 1; 
	}
	
	//create and bind texture
	glGenTextures(1, &volume_tex);
	glBindTexture(GL_TEXTURE_3D, volume_tex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, N, N,N, 0, GL_RED, GL_FLOAT, volume_data);
	


	
	glGenerateMipmap(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_3D, volume_tex);
	

	shaderProgram.Activate();

	GLint texlocation = glGetUniformLocation(shaderProgram.ID, "volume_data");
	glUniform1i(texlocation, 0);
	GLint iResoulutionLocation = glGetUniformLocation(shaderProgram.ID, "iResolution");
	glUniform2f(iResoulutionLocation, 1920, 1080);
	GLint TimeLocation = glGetUniformLocation(shaderProgram.ID, "time");
	glUniform1f(TimeLocation, crntTime);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	// Swap the back buffer with the front buffer
	glfwSwapBuffers(window);
	// Take care of all GLFW events
	glfwPollEvents();
	std::cout << "frame end";
}
shaderProgram.Delete();
// Delete window before ending the program
glfwDestroyWindow(window);
// Terminate GLFW before ending the program
glfwTerminate();
return 0;
}
//Keyboard input
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const GLfloat rotationspeed = 2;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			rotationvert += rotationspeed;
			break;
		case GLFW_KEY_DOWN:
			rotationvert -= rotationspeed;
			break;
		case GLFW_KEY_RIGHT:
			rotation += rotationspeed;
			break;
		case GLFW_KEY_LEFT:
			rotation -= rotationspeed;
			break;
		case GLFW_KEY_D:
			translateX += 0.1;
			break;
		case GLFW_KEY_A:
			translateX -= 0.1;
			break;
		case GLFW_KEY_W:
			translateY += 0.1;
			break;
		case GLFW_KEY_S:
			translateY -= 0.1;
			break;
		case GLFW_KEY_Q:
			translateZ += 0.1;
			break;
		case GLFW_KEY_E:
			translateZ -= 0.1;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}