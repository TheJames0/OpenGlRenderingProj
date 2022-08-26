#include <glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include <iostream>
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void InitGL();
GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;


// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1f, 1f, 0f, 1.0f);\n"
"}\n\0";


int main() {
	bool open = true;
	const int SCREEN_WIDTH = 1920;
	const int SCREEN_HEIGHT = 1080;
	GLFWwindow* window;
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fluid Sim", NULL, NULL);
	
	glfwSetKeyCallback(window, keycallback);
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	
	glfwMakeContextCurrent(window);
	
	gladLoadGL();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	
	GLfloat verts[] =
	{
		-0.5,-0.5 * float(sqrt(3) / 3),0.0,
		0.5,-0.5 * float(sqrt(3) / 3),0.0,
		0.0f,0.5f * float(sqrt(3) *2 / 3),0.0
	};

	
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	glfwSwapBuffers(window);
	while (!glfwWindowShouldClose(window))
	{

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glRotatef(rotationX, 0, 0, 1);
		glRotatef(rotationY, 0, 1, 0);
		glUseProgram(shaderProgram);
		glBindVertexArray(VBO);
		glfwSwapBuffers(window);
		
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VBO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const GLfloat rotationspeed = 10;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			rotationX -= rotationspeed;
			break;
		case GLFW_KEY_DOWN:
			rotationX += rotationspeed;
			break;
		case GLFW_KEY_RIGHT:
			rotationY += rotationspeed;
			break;
		case GLFW_KEY_LEFT:
			rotationY -= rotationspeed;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}