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




int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "++Void", NULL, NULL);
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

	glViewport(0, 0, 800, 800);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//ShaderTime
	Shader basic("BasicVertShader.vs", "BasicFragShader.fs");

	//Create Cube
	float color[] = {
		0.5f,0.5f,0.5f
	};
	cube test = cube(color);


	unsigned int VBO; //Vertex Buffer Object
	glGenBuffers(1, &VBO); //Give the object an id

	unsigned int VAO; //Vertex Array object
	glGenVertexArrays(1, &VAO);

	//unsigned int EBO; //Vertex Array object
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(test.verts), test.verts, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(test.index), test.index, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);*/

	//Tell gl how to read our vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Tell gl how to read our combined colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	basic.use();

	////do a transform
	/*glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::rotate(translate, glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
	translate = glm::translate(translate, glm::vec3(0.25f, 0.25f, 0.25f));
	basic.setMat("transform", translate);*/
	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);

		//Perform WRENDER
		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		basic.use();
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

	//glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
}