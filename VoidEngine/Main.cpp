#include <iostream>

//External tools
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

//my stuff
#include "ShaderReader.h"
#include "primativeGen.h"
#include "TestObj/test pyramid.h"
#include "Light.h"


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
	Shader normal("normalShader.vs", "normalFragShader.fs");

	//Convert obj from header to my format
	std::vector<Vertex> temp;
	temp.resize(sizeof(test_pyramid_data)/sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(test_pyramid_data) / sizeof(OBJ_VERT); i++) {
		float x = test_pyramid_data[i].pos[0];
		float y = test_pyramid_data[i].pos[1];
		float z = test_pyramid_data[i].pos[2];
		temp[i].Position = glm::vec3(x,y,z);
		x = test_pyramid_data[i].nrm[0];
		y = test_pyramid_data[i].nrm[1];
		z = test_pyramid_data[i].nrm[2];
		temp[i].Normal = glm::vec3(x, y, z);
		x = test_pyramid_data[i].uvw[0];
		y = test_pyramid_data[i].uvw[1];
		temp[i].TexCord = glm::vec2(x, y);
		temp[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> tempInd;
	tempInd.resize(sizeof(test_pyramid_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(test_pyramid_indicies)/sizeof(unsigned int); i++) {
		tempInd[i] = test_pyramid_indicies[i];
	}
	// load and create a texture 
	// -------------------------
	unsigned int brick;
	glGenTextures(1, &brick);
	glBindTexture(GL_TEXTURE_2D, brick); //all upcoming GL_TEXTURE_2D ops effect this texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, numColors;
	unsigned char* data = stbi_load("Texture/bricks2.jpg", &width, &height, &numColors, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	Mesh pyramid = Mesh(temp, tempInd, brick);

	//Mesh cube = CreateCube();

	//Mesh Plane = generatePlane();


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

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	view = glm::translate(view, glm::vec3(0.0f, 0.0f,-3.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate_slow(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::vec3 objColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 LightColor = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(2.0f, 5.0f, 2.0f);

	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);


		//Perform WRENDER
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//basic.use();
		normal.use();
		model = glm::rotate_slow(model, glm::radians(0.001f), glm::vec3(0.0f, 1.0f, 0.0f));
		normal.setMat("model", model);
		normal.setMat("view", view);
		normal.setMat("projection", proj);
		normal.setVec3("lightColor", LightColor);
		normal.setVec3("lightPos", lightPos);
		normal.setFloat("Time", glfwGetTime());
		normal.setVec3("ambiantLightColor", glm::vec3(0.2f,0.2f,0.2f));
		pyramid.Draw(normal);
		//floor.Draw(basic);
		//glBindVertexArray(VAO);
		////glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);

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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void renderTriangle() {
	unsigned int VBO; //Vertex Buffer Object
	glGenBuffers(1, &VBO); //Give the object an id

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//bind VBO to an array buffer

	//glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
}