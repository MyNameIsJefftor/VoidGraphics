#include <iostream>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include "ShaderReader.h"


void framebuffer_size_callback(GLFWwindow* windo, int width, int height);
void processInput(GLFWwindow* window);
void renderTriangle();


float tVertices[] = {
	//Pos							//colors
	 0.0f,  0.5f, 0.0f,  /* top */	1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,  /*right*/	0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,  /*left */	0.0f, 0.0f, 1.0f
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "++Void", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failure to create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failure to load GLAD(os)" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//ShaderTime

	Shader basic("BasicVertShader.vs", "BasicFragShader.fs");

	////VertexShader
	//unsigned int vertexShader;
	//vertexShader = glCreateShader(GL_VERTEX_SHADER); //tell GL that this is a vertexShader

	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//Set the shader to the one we wrote.
	//glCompileShader(vertexShader); //Compile the Shader code

	////FragShader/ColorShader
	//unsigned int fragShader;
	//fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//glShaderSource(fragShader, 1, &FragColorSource, NULL);
	//glCompileShader(fragShader);

	////Create the program
	//unsigned int shaderProgram;
	//shaderProgram = glCreateProgram();

	//glAttachShader(shaderProgram, vertexShader);//attach vertex and kermit de frag.
	//glAttachShader(shaderProgram, fragShader);
	//glLinkProgram(shaderProgram); //Link


	//glUseProgram(shaderProgram); //Tell gl to use our new shader

	//glDeleteShader(vertexShader); //Don't need these anymore. The program has them now.
	//glDeleteShader(fragShader);

	//Create Triangle
	unsigned int VBO; //Vertex Buffer Object
	glGenBuffers(1, &VBO); //Give the object an id

	unsigned int VAO; //Vertex Array object
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);

	//Tell gl how to read our vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Tell gl how to read our combined colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	basic.use();

	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);

		//Perform WRENDER
		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Swap the buffs. Check for events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* windo, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void renderTriangle() {
	unsigned int VBO; //Vertex Buffer Object
	glGenBuffers(1, &VBO); //Give the object an id

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//bind VBO to an array buffer

	glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
}