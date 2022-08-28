//------- Ignore this ----------
#include<filesystem>
//namespace fs = std::filesystem;
//------------------------------

#include<iostream>
#include<glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include "MarchingCubesLookUp.cpp"
//3 dimensional grid arraysize
const int arraysize = 5;

const unsigned int width = 800;
const unsigned int height = 800;
float rotation;
float rotationvert;

int configIndex = 0;

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cubeMarch();

// Vertices coordinates

GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};
struct array3D {
	int n = arraysize;
	float a[arraysize][arraysize][arraysize];
};


void GenTestArray(array3D& arr)
{
	int total = 0;
	for (int x=0; x < arr.n; ++x)
	{
		for (int y=0; x < y; ++y)
		{
			for (int z=0; z < arr.n; ++z)
			{
				total++;
				if (total % 2 == 0)
				{
					arr.a[x][y][z] = 1;
				}
				else
				{
					arr.a[x][y][z] = 0;
				}
			}
		}
	}
}
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

	
	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "Pyramidz", NULL, NULL);
	// Error check if the window fails to create
	glfwSetKeyCallback(window, keycallback);
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
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");



	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	/*
	* I'm doing this relative path thing in order to centralize all the resources into one folder and not
	* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
	* folder and then give a relative path from this folder to whatever resource you want to get to.
	* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
	*/

	//std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	//std::string texPath = "/Resources/YoutubeOpenGL 7 - Going 3D/";

	// Texture
	
	//
	//Texture brickTex((parentDir + texPath + "brick.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	//brickTex.texUnit(shaderProgram, "tex0", 0);

	// Original code from the tutorial
	/*Texture brickTex("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);*/

	// Variables that help the rotation of the pyramid

	double prevTime = glfwGetTime();

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


	//GENERATE 3D Test ARRAY
	// 
	const int n = 5;
	array3D array;
	GenTestArray(array);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Simple timer
		double crntTime = glfwGetTime();
		

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Assigns different transformations to each matrix
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotationvert), glm::vec3(0.0f, 0.0f, 1.0f));
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(60.0f), (float)width / height, 0.1f, 100.0f);

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		//MarchCube
		cubeMarch();
		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
		// Binds texture so that is appears in rendering
		//brickTex.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	//brickTex.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void cubeMarch()
{
	
	float surfacelevel = 0.5;
	/*
	for(int i = 0;i< 8; ++i)
	{
		if(CornerTable)
	}
	*/
	//Test Area

	if (configIndex == 0 || configIndex == 255)
	{
		return;
	}
	int edgeIndex = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int p = 0; p < 3; p++)
		{
			int *indice = TriangleTable[configIndex, edgeIndex];

			if (*indice == -1)
				return;
			
			int vert1pos[3] = { EdgeTable[*indice][0][1],EdgeTable[*indice][0][2],EdgeTable[*indice][0][3] };
			int vert2pos[3] = { EdgeTable[*indice][1][1],EdgeTable[*indice][1][2],EdgeTable[*indice][1][3] };
			
			//Final pos to be added
			int vert3pos[3] = { ((vert1pos[0] + vert2pos[0])) / 2,((vert1pos[1] + vert2pos[1])) / 2 ,((vert1pos[2] + vert2pos[2])) / 2 };
			
		}
	}












	//
	//XYZ gridspace render loop
	/*
	for (int x; x < arr.n; ++x)
	{
		for (int y; y < arr.n; ++y)
		{
			
			for (int z; z < arr.n; ++z)
			{
				

				

			}
		}
	}
	*/
	
}
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const GLfloat rotationspeed = 10;

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
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}