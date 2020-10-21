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
#include "Skybox.h"
#include "Object.h"
#include "lights.h"
#include "Camera.h"
#include "shadows.h"

//header obj
#include "TestObj/resources.h"
#include "TestObj/Building.h"
#include "TestObj/city_triangulated.h"
#include "TestObj/groundplane.h"



void framebuffer_size_callback(GLFWwindow* windo, int width, int height);
void processInput(GLFWwindow* window, Camera* cam);
void mouseCallback(GLFWwindow* window, double x, double y);
void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight> pointLights, glm::mat4 proj);
Camera MainCamera;
double lastX = 0.0, lastY = 0.0;
bool firstPass = true, Mouse = true, grayScale = false;
float pitch = 0.0f, yaw = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int width = 800, height = 450;
int awidth = 16, aheight = 9;
float aspect = awidth / aheight;

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
	glfwSetWindowAspectRatio(window, awidth, aheight);
	glm::vec3 cameraPos = glm::vec3(1.0f, 2.0f, -5.0f);
	MainCamera = Camera(cameraPos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouseCallback);

	//capture that mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Shader Flag("wave.vert", "normalFragShader.frag", "VertsToTriangles.geom");
	Shader Instance("instance.vert", "normalFragShader.frag");
	Shader SkyboxShad("Skybox.vert", "Skybox.frag");

	//Arrays
	std::vector<Object> Objects;
	std::vector<dirLight> dirLights;
	std::vector<pointLight> pointLights;
	std::vector<Shader*> Shaders;

	Shaders.push_back(&normal);
	Shaders.push_back(&Flag);
	Shaders.push_back(&Instance);

	//Convert obj from header to my format
	
	std::vector<Vertex> headerImport;
	headerImport.resize(sizeof(city_triangulated_data)/sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(city_triangulated_data) / sizeof(OBJ_VERT); i++) {
		float x = city_triangulated_data[i].pos[0];
		float y = city_triangulated_data[i].pos[1];
		float z = city_triangulated_data[i].pos[2];
		headerImport[i].Position = glm::vec3(x,y,z);
		x = city_triangulated_data[i].nrm[0];
		y = city_triangulated_data[i].nrm[1];
		z = city_triangulated_data[i].nrm[2];
		headerImport[i].Normal = glm::vec3(x, y, z);
		x = city_triangulated_data[i].uvw[0];
		y = city_triangulated_data[i].uvw[1];
		headerImport[i].TexCord = glm::vec2(x, y)*3.0f;
		headerImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> tempInd;
	tempInd.resize(sizeof(city_triangulated_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(city_triangulated_indicies)/sizeof(unsigned int); i++) {
		tempInd[i] = city_triangulated_indicies[i];
	}
	// load and create a texture 
	// -------------------------
	std::vector<Texture> Resource;
	Texture temp = CreateDiff("Texture/Resource.ktx");
	Resource.push_back(temp);
	temp = CreateSpec("Texture/Resource_Spec.ktx");
	Resource.push_back(temp);

	Skybox Skybox = CreateSkybox("Texture/Skybox.ktx", SkyboxShad);

	Object cube = { CreateCube(Resource), glm::mat4(1.0f), &Instance};
	Object plane = { generatePlane(Resource,3), glm::mat4(1.0f) , &Flag};
	Object Buildings = { Mesh(headerImport, tempInd, CreateTexture("Texture/citydiff.ktx","Texture/cityspec.ktx")), glm::mat4(1.0), &normal };
	plane.myPos = glm::translate(plane.myPos, glm::vec3(0.0f, -1.0f, 0.0f));
	Buildings.myPos = glm::scale(Buildings.myPos, glm::vec3(2.0f));
	Objects.push_back(Buildings);
	Objects.push_back(plane);

	Object RCube { CreateCube(Resource,0.125f), glm::mat4(1.0f), &normal };
	Object GCube { CreateCube(Resource,0.125f), glm::mat4(1.0f), &normal };
	Object BCube { CreateCube(Resource,0.125f), glm::mat4(1.0f), &normal };

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);

	//create lights
	dirLight sun = {
		glm::normalize(glm::vec3(-0.25f, -1.0f, -0.25f)),
		glm::vec3(0.8f,0.4f,0.2f),
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
		glm::vec3(1.0f,0.0f,0.0f),
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

	glm::mat4* instanceTest = genRandomPos(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 100, 0.0625f);

	while (!glfwWindowShouldClose(window)) {
		//Calc delta
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Get input
		processInput(window, &MainCamera);
		float sinlightz = 1.0f * sin((float)glfwGetTime());
		float sinlighty = 1.0f * sin((float)glfwGetTime());
		float coslighty = 1.0f * cos((float)glfwGetTime());
		float coslightx = 1.0f * cos((float)glfwGetTime());
		pointLights[0].position = glm::vec3(coslightx, sinlighty, 0.0f);
		pointLights[1].position = glm::vec3(coslightx, 0.0f, sinlightz);
		pointLights[2].position = glm::vec3(0.0f, coslighty, sinlightz);
		RCube.myPos = glm::translate(glm::mat4(1.0f), pointLights[0].position);
		GCube.myPos = glm::translate(glm::mat4(1.0f), pointLights[1].position);
		BCube.myPos = glm::translate(glm::mat4(1.0f), pointLights[2].position);
		//Perform WRENDER
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//DrawSkybox
		//DrawSkybox(Skybox, MainCamera.getViewMat(), proj);
		glm::vec3 temp = MainCamera.pos;
		MainCamera.pos = glm::vec3(0.0f);
		glDepthMask(GL_FALSE);
		SkyboxShad.use();
		SkyboxShad.setBool("grayScale", grayScale);
		SkyboxShad.setMat("view", MainCamera.getViewMat());
		SkyboxShad.setMat("projection", proj);
		glBindVertexArray(Skybox.cube.VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox.tex.id);
		glDrawElements(GL_TRIANGLES, Skybox.cube.index.size(), GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		MainCamera.pos = temp;

		//basic.use();
		//update the lights
		setShaders(Shaders, dirLights, pointLights, proj);

		for (int i = 0; i < Objects.size(); i++) {
			Objects[i].draw();
		}
		RCube.draw();

		GCube.draw();

		BCube.draw();

		drawInstancedObj(instanceTest, cube, Instance);

		//Swap the buffs. Check for events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* windo, int nwidth, int nheight) {
	width = nwidth;
	height = nheight;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera* cam) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grayScale = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		MainCamera = Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !grayScale)
		grayScale = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && Mouse) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Mouse = false;
	}
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		Mouse = true;
	}
	//camera control
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MainCamera.keyboardInput(Forward, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MainCamera.keyboardInput(Backward, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		MainCamera.keyboardInput(Left, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		MainCamera.keyboardInput(Right, deltaTime);
	}
}

void mouseCallback(GLFWwindow* window, double x, double y) {
	if (firstPass) {
		lastX = x;
		lastY = y;
		firstPass = false;
	}
	float xoff = (float)(x - lastX);
	float yoff = (float)(lastY - y);
	lastX = x;
	lastY = y;
	if(Mouse)
		MainCamera.mouseInput(xoff, yoff);

}

void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight> pointLights, glm::mat4 proj) {
	for (int shad = 0; shad < shaders.size(); shad++) {
		shaders[shad]->use();
		shaders[shad]->setBool("grayScale", grayScale);
		shaders[shad]->setInt("numDirLights", (int)dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			shaders[shad]->setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			shaders[shad]->setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			shaders[shad]->setVec3(uniform, dirLights[i].specular);
		}
		shaders[shad]->setInt("numPointLights", (int)pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "pointlight[%i].pos", i);
			shaders[shad]->setVec3(uniform, pointLights[i].position);

			sprintf_s(uniform, "pointlight[%i].color", i);
			shaders[shad]->setVec3(uniform, pointLights[i].color);

			sprintf_s(uniform, "pointlight[%i].constant", i);
			shaders[shad]->setFloat(uniform, pointLights[i].constant);

			sprintf_s(uniform, "pointlight[%i].linear", i);
			shaders[shad]->setFloat(uniform, pointLights[i].linear);

			sprintf_s(uniform, "pointlight[%i].quadratic", i);
			shaders[shad]->setFloat(uniform, pointLights[i].quadratic);

			sprintf_s(uniform, "pointlight[%i].specular", i);
			shaders[shad]->setVec3(uniform, pointLights[i].specular);
		}
		shaders[shad]->setMat("view", MainCamera.getViewMat());
		shaders[shad]->setMat("projection", proj);
		shaders[shad]->setFloat("Time", (float)glfwGetTime());
		shaders[shad]->setVec3("ambiant", glm::vec3(0.2f, 0.2f, 0.2f));
		shaders[shad]->setVec3("camPos", MainCamera.pos);
	}
}