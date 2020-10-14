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
	//Shader GeomTest("VertTest.vert", "fragTest.frag", "test.geom");
	Shader Flag("wave.vert", "normalFragShader.frag");
	//Arrays
	std::vector<Object> Objects;
	std::vector<dirLight> dirLights;
	std::vector<pointLight> pointLights;

	//Convert obj from header to my format
	
	std::vector<Vertex> headerImport;
	headerImport.resize(sizeof(resources_data)/sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(resources_data) / sizeof(OBJ_VERT); i++) {
		float x = resources_data[i].pos[0];
		float y = resources_data[i].pos[1];
		float z = resources_data[i].pos[2];
		headerImport[i].Position = glm::vec3(x,y,z);
		x = resources_data[i].nrm[0];
		y = resources_data[i].nrm[1];
		z = resources_data[i].nrm[2];
		headerImport[i].Normal = glm::vec3(x, y, z);
		x = resources_data[i].uvw[0];
		y = resources_data[i].uvw[1];
		headerImport[i].TexCord = glm::vec2(x, y);
		headerImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> tempInd;
	tempInd.resize(sizeof(resources_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(resources_indicies)/sizeof(unsigned int); i++) {
		tempInd[i] = resources_indicies[i];
	}
	// load and create a texture 
	// -------------------------
	std::vector<unsigned int> resourceTex;
	ktxTexture* resourceTexDiff;
	ktxTexture* resourceTexSpec;
	KTX_error_code result;
	unsigned int resourcediffId = 0, resourcespecId = 0;
	GLenum target, glerror;

	result = ktxTexture_CreateFromNamedFile("Texture/resource.ktx", KTX_TEXTURE_CREATE_NO_FLAGS, &resourceTexDiff);
	glGenTextures(1, &resourcediffId);
	result = ktxTexture_GLUpload(resourceTexDiff, &resourcediffId, &target, &glerror);
	resourceTex.push_back(resourcediffId);

	result = ktxTexture_CreateFromNamedFile("Texture/resource_spec.ktx", KTX_TEXTURE_CREATE_NO_FLAGS, &resourceTexSpec);
	glGenTextures(1, &resourcespecId);
	result = ktxTexture_GLUpload(resourceTexSpec, &resourcespecId, &target, &glerror);
	resourceTex.push_back(resourcespecId);


	Object cube = { CreateCube(resourceTex), glm::mat4(1.0f)};
	Object plane = { generatePlane(resourceTex,3), glm::mat4(1.0f) };
	Object resource = { Mesh(headerImport, tempInd, resourceTex), glm::mat4(1.0f) };
	cube.myPos = glm::translate(cube.myPos, glm::vec3(-0.5f, 0.0f, 0.0f));
	plane.myPos = glm::translate(plane.myPos, glm::vec3(0.0f, -0.5f, 0.0f));
	resource.myPos = glm::scale(resource.myPos, glm::vec3(0.05f, 0.05f, 0.05f));
	Objects.push_back(resource);
	Objects.push_back(cube);
	//Objects.push_back(plane);


	//Camera Shenanigans
	//this is what look at does
	glm::vec3 cameraPos = glm::vec3(1.0f, 2.0f, -5.0f);
	/*glm::vec3 cameraTar = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDir = glm::normalize(cameraPos - cameraTar);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRig = glm::normalize(glm::cross(up, cameraDir));
	glm::vec3 cameraUp = glm::cross(cameraDir, cameraRig);*/

	glm::mat4 view;
	view = glm::lookAt(
		cameraPos,
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
		glm::vec3(0.3f,0.2f,0.1f),
		glm::vec3(0.3f,0.2f,0.1f)
	};
	dirLight moon = {
		glm::normalize(glm::vec3(0.25f, 1.0f, 0.25f)),
		glm::vec3(0.2f,0.3f,0.4f),
		glm::vec3(0.2f,0.3f,0.4f)
	};
	dirLights.push_back(sun);
	dirLights.push_back(moon);

	pointLight red = {
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,0.0f,0.0f),
		1.0f,
		0.7f,
		1.8f,
		glm::vec3(1.0f,0.0f,0.0f)
	};
	pointLight green = {
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f,0.1f,0.0f),
		1.0f,
		0.7f,
		1.8f,
		glm::vec3(0.0f,1.0f,0.0f)
	};
	pointLight blue = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		1.0f,
		0.7f,
		1.8f,
		glm::vec3(0.0f,0.0f,1.0f)
	};
	pointLights.push_back(red);
	pointLights.push_back(green);
	pointLights.push_back(blue);

	while (!glfwWindowShouldClose(window)) {
		//Get input
		processInput(window);
		//float lightz = .8f * sin(glfwGetTime());
		float pointlightx = 3.0f * cos((float)glfwGetTime());
		pointLights[0].position = glm::vec3(pointlightx, 0.0f, 0.0f);
		pointLights[1].position = glm::vec3(0.0f, 0.0f, pointlightx);
		pointLights[2].position = glm::vec3(0.0f, pointlightx, 0.0f);
		//Perform WRENDER
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		//basic.use();
		//update the lights
		normal.use();
		normal.setInt("numDirLights", dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			normal.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			normal.setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			normal.setVec3(uniform, dirLights[i].specular);
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

			sprintf_s(uniform, "pointlight[%i].specular", i);
			normal.setVec3(uniform, pointLights[i].specular);
		}
		normal.setMat("view", view);
		normal.setMat("projection", proj);
		normal.setFloat("Time", (float)glfwGetTime());
		normal.setVec3("ambiant", glm::vec3(0.2f,0.2f,0.2f));
		normal.setVec3("camPos", glm::vec3(1.0f, 2.0f, -5.0f));

		for (int i = 0; i < Objects.size(); i++) {
			normal.setMat("model", Objects[i].myPos);
			Objects[i].myMesh.Draw(normal);
		}
		//floor.Draw(basic);
		//glBindVertexArray(VAO);
		////glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
		Flag.use();
		Flag.setInt("numDirLights", dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			Flag.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			Flag.setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			Flag.setVec3(uniform, dirLights[i].specular);
		}
		Flag.setInt("numPointLights", pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "pointlight[%i].pos", i);
			Flag.setVec3(uniform, pointLights[i].position);

			sprintf_s(uniform, "pointlight[%i].color", i);
			Flag.setVec3(uniform, pointLights[i].color);

			sprintf_s(uniform, "pointlight[%i].constant", i);
			Flag.setFloat(uniform, pointLights[i].constant);

			sprintf_s(uniform, "pointlight[%i].linear", i);
			Flag.setFloat(uniform, pointLights[i].linear);

			sprintf_s(uniform, "pointlight[%i].quadratic", i);
			Flag.setFloat(uniform, pointLights[i].quadratic);

			sprintf_s(uniform, "pointlight[%i].specular", i);
			Flag.setVec3(uniform, pointLights[i].specular);
		}
		Flag.setMat("view", view);
		Flag.setMat("projection", proj);
		Flag.setFloat("Time", (float)glfwGetTime());
		Flag.setVec3("ambiant", glm::vec3(0.2f, 0.2f, 0.2f));
		Flag.setVec3("camPos", glm::vec3(1.0f, 2.0f, -5.0f));
		Flag.setMat("view", view);
		Flag.setMat("projection", proj);
		Flag.setMat("model", plane.myPos);
		plane.myMesh.DrawnUnIndex(Flag);

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