#include <iostream>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderReader.h"
#include "primativeGen.h"


void framebuffer_size_callback(GLFWwindow* windo, int width, int height);
void processInput(GLFWwindow* window);
void renderTriangle();

int width = 800, height = 800;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "++Void", NULL, NULL);
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

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//ShaderTime
	Shader basic("BasicVertShader.vs", "BasicFragShader.fs");

	//Create Cube
	float color[] = {
		0.5f,0.5f,0.5f
	};
	oldcube test = oldcube();


	unsigned int VBO; //Vertex Buffer Object
	unsigned int VAO; //Vertex Array object

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //Give the object an id

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(test.verts), test.verts, GL_STATIC_DRAW);

	unsigned int EBO; //Vertex Array object
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(test.index), test.index, GL_STATIC_DRAW);

	//Tell gl how to read our vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Tell gl how to read our combined colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	basic.use();

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	//Camera Shenanigans
	//this is what look at does
	/*glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraTar = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDir = glm::normalize(cameraPos - cameraTar);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRig = glm::normalize(glm::cross(up, cameraDir));
	glm::vec3 cameraUp = glm::cross(cameraDir, cameraRig);*/

	glm::mat4 view;
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	view = glm::translate(view, glm::vec3(0.0f, 0.0f,-3.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.5f, 0.0f));

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);


		//Perform WRENDER
		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic.use();
		basic.setMat("model", model);
		basic.setMat("view", view);
		basic.setMat("projection", proj);
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//Swap the buffs. Check for events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* windo, int nwidth, int nheight) {
	//width = nwidth;
	//height = nheight;
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

	//glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
}