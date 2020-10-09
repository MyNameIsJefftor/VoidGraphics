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
#include <KTX/ktx.h>

//my stuff
#include "ShaderReader.h"
#include "Mesh.h"
#include "Object.h"
#include "lights.h"

//header obj
#include "TestObj/resources.h"
#include "TestObj/test pyramid.h"


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
	//Shader basic("BasicVertShader.vs", "BasicFragShader.fs");
	Shader normal("normalVertShader.vert", "normalFragShader.frag");
	Shader TEST("VertTest.vert", "fragTest.frag");

	//Arrays
	std::vector<Object> Objects;
	std::vector<dirLight> dirLights;
	std::vector<pointLight> pointLights;

	//Convert obj from header to my format
	std::vector<Vertex> temp;
	temp.resize(sizeof(resources_data)/sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(resources_data) / sizeof(OBJ_VERT); i++) {
		float x = resources_data[i].pos[0];
		float y = resources_data[i].pos[1];
		float z = resources_data[i].pos[2];
		temp[i].Position = glm::vec3(x,y,z);
		x = resources_data[i].nrm[0];
		y = resources_data[i].nrm[1];
		z = resources_data[i].nrm[2];
		temp[i].Normal = glm::vec3(x, y, z);
		x = resources_data[i].uvw[0];
		y = resources_data[i].uvw[1];
		temp[i].TexCord = glm::vec2(x, y);
		temp[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> tempInd;
	tempInd.resize(sizeof(resources_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(resources_indicies)/sizeof(unsigned int); i++) {
		tempInd[i] = resources_indicies[i];
	}
	// load and create a texture 
	// -------------------------
	ktxTexture* brickTex;
	KTX_error_code result;
	unsigned int texture = 0;
	GLenum target, glerror;

	result = ktxTexture_CreateFromNamedFile("Texture/brick.ktx", KTX_TEXTURE_CREATE_NO_FLAGS, &brickTex);

	glGenTextures(1, &texture);
	result = ktxTexture_GLUpload(brickTex, &texture, &target, &glerror);

	Object cube = { CreateCube(), glm::mat4(1.0f)};
	Object plane = { generatePlane(3), glm::mat4(1.0f) };
	Object resource = { Mesh(temp, tempInd, texture), glm::mat4(1.0f) };
	Object lightCube = { CreateCube(.2, glm::vec3(1,1,1)), glm::mat4(1.0f) };
	cube.myPos = glm::translate(cube.myPos, glm::vec3(-0.5f, 0.0f, 0.0f));
	plane.myPos = glm::translate(plane.myPos, glm::vec3(0.0f, -0.5f, 0.0f));
	resource.myPos = glm::scale(resource.myPos, glm::vec3(0.05f, 0.05f, 0.05f));
	Objects.push_back(resource);
	Objects.push_back(cube);
	Objects.push_back(plane);


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
		glm::vec3(1.0f, 2.0f, -5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);

	//create lights
	dirLight sun = {
		glm::normalize(glm::vec3(-0.25f, -1.0f, -0.25f)),
		glm::vec3(0.3f,0.2f,0.1f)
	};
	dirLight moon = {
		glm::normalize(glm::vec3(0.25f, 1.0f, 0.25f)),
		glm::vec3(0.2f,0.3f,0.4f)
	};
	dirLights.push_back(sun);
	dirLights.push_back(moon);

	pointLight test = {
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.6f,0.0f,0.6f),
		1,
		0.7,
		1.8
	};
	pointLights.push_back(test);
	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);
		//float lightz = .8f * sin(glfwGetTime());
		float lightx = 3 * cos(glfwGetTime());
		pointLights[0].position = glm::vec3(lightx, 0.0f, 0.0f);
		lightCube.myPos = glm::mat4(1.0);
		lightCube.myPos = glm::translate(lightCube.myPos, pointLights[0].position);
		//Perform WRENDER
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		//basic.use();
		//update the lights
		normal.setInt("numDirLights", dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			normal.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			normal.setVec3(uniform, dirLights[i].color);
		}
		normal.setInt("numPointLights", pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "pointlight[%i].pos", i);
			normal.setVec3(uniform, pointLights[i].position);

			sprintf_s(uniform, "pointlight[%i].color", i);
			normal.setVec3(uniform, pointLights[i].color);

			sprintf_s(uniform, "pointlight[%i].constant", i);
			normal.setFloat(uniform, pointLights[i].constant);

			sprintf_s(uniform, "pointlight[%i].linear", i);
			normal.setFloat(uniform, pointLights[i].linear);

			sprintf_s(uniform, "pointlight[%i].quadratic", i);
			normal.setFloat(uniform, pointLights[i].quadratic);
		}
		normal.setMat("view", view);
		normal.setMat("projection", proj);
		normal.setFloat("Time", glfwGetTime());
		normal.setVec3("ambiant", glm::vec3(0.2f,0.2f,0.2f));
		normal.use();
		normal.setMat("model", lightCube.myPos);

		lightCube.myMesh.Draw(normal);
		for (int i = 0; i < Objects.size(); i++) {
			normal.setMat("model", Objects[i].myPos);
			Objects[i].myMesh.Draw(normal);
		}
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