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

//header obj
#include "TestObj/resources.h"
#include "TestObj/test pyramid.h"
#include "TestObj/Building.h"


void framebuffer_size_callback(GLFWwindow* windo, int width, int height);
void processInput(GLFWwindow* window, Camera* cam);
void mouseCallback(GLFWwindow* window, double x, double y);
Camera MainCamera;
double lastX = 0.0, lastY = 0.0;
bool firstPass = true, Mouse = true;
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

	//Convert obj from header to my format
	
	std::vector<Vertex> headerImport;
	headerImport.resize(sizeof(Building_data)/sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(Building_data) / sizeof(OBJ_VERT); i++) {
		float x = Building_data[i].pos[0];
		float y = Building_data[i].pos[1];
		float z = Building_data[i].pos[2];
		headerImport[i].Position = glm::vec3(x,y,z);
		x = Building_data[i].nrm[0];
		y = Building_data[i].nrm[1];
		z = Building_data[i].nrm[2];
		headerImport[i].Normal = glm::vec3(x, y, z);
		x = Building_data[i].uvw[0];
		y = Building_data[i].uvw[1];
		headerImport[i].TexCord = glm::vec2(x, y);
		headerImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> tempInd;
	tempInd.resize(sizeof(Building_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(Building_indicies)/sizeof(unsigned int); i++) {
		tempInd[i] = Building_indicies[i];
	}
	// load and create a texture 
	// -------------------------
	std::vector<Texture> Resource;
	Texture temp = CreateDiff("Texture/BuildingDiff.ktx");
	Resource.push_back(temp);
	temp = CreateSpec("Texture/Resource_Spec.ktx");
	Resource.push_back(temp);

	Skybox Skybox = CreateSkybox("Texture/Skybox.ktx", SkyboxShad);

	Object cube = { CreateCube(Resource), glm::mat4(1.0f)};
	Object plane = { generatePlane(Resource,3), glm::mat4(1.0f) };
	Object resource = { Mesh(headerImport, tempInd, Resource), glm::mat4(1.0f) };
	plane.myPos = glm::translate(plane.myPos, glm::vec3(0.0f, -1.0f, 0.0f));
	resource.myPos = glm::scale(resource.myPos, glm::vec3(0.05f, 0.05f, 0.05f));
	Objects.push_back(resource);
	//Objects.push_back(cube);

	Object RCube { CreateCube(Resource,0.125f), glm::mat4(1.0f) };
	Object GCube { CreateCube(Resource,0.125f), glm::mat4(1.0f) };
	Object BCube { CreateCube(Resource,0.125f), glm::mat4(1.0f) };

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
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
		SkyboxShad.setMat("view", MainCamera.getViewMat());
		SkyboxShad.setMat("projection", proj);
		glBindVertexArray(Skybox.cube.VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox.tex.id);
		glDrawElements(GL_TRIANGLES, Skybox.cube.index.size(), GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		MainCamera.pos = temp;

		//basic.use();
		//update the lights
		normal.use();
		normal.setInt("numDirLights", (int)dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			normal.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			normal.setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			normal.setVec3(uniform, dirLights[i].specular);
		}
		normal.setInt("numPointLights", (int)pointLights.size());
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
		normal.setMat("view", MainCamera.getViewMat());
		normal.setMat("projection", proj);
		normal.setFloat("Time", (float)glfwGetTime());
		normal.setVec3("ambiant", glm::vec3(0.2f,0.2f,0.2f));
		normal.setVec3("camPos", MainCamera.pos);

		for (int i = 0; i < Objects.size(); i++) {
			normal.setMat("model", Objects[i].myPos);
			Objects[i].myMesh.Draw(normal);
		}
		normal.setMat("model", RCube.myPos);
		RCube.myMesh.Draw(normal);

		normal.setMat("model", GCube.myPos);
		GCube.myMesh.Draw(normal);

		normal.setMat("model", BCube.myPos);
		BCube.myMesh.Draw(normal);

		Flag.use();
		Flag.setInt("numDirLights", (int)dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			Flag.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			Flag.setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			Flag.setVec3(uniform, dirLights[i].specular);
		}
		Flag.setInt("numPointLights", (int)pointLights.size());
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
		Flag.setMat("view", MainCamera.getViewMat());
		Flag.setMat("projection", proj);
		Flag.setFloat("Time", (float)glfwGetTime());
		Flag.setVec3("ambiant", glm::vec3(0.2f, 0.2f, 0.2f));
		Flag.setVec3("camPos", MainCamera.pos);
		Flag.setMat("projection", proj);
		Flag.setMat("model", plane.myPos);
		plane.myMesh.DrawnUnIndex(Flag);

		Instance.use();
		Instance.setInt("numDirLights", (int)dirLights.size());
		for (int i = 0; i < dirLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "dirlight[%i].direction", i);
			Flag.setVec3(uniform, dirLights[i].direction);

			sprintf_s(uniform, "dirlight[%i].color", i);
			Flag.setVec3(uniform, dirLights[i].color);

			sprintf_s(uniform, "dirlight[%i].specular", i);
			Flag.setVec3(uniform, dirLights[i].specular);
		}
		Instance.setInt("numPointLights", (int)pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			char uniform[64];

			sprintf_s(uniform, "pointlight[%i].pos", i);
			Instance.setVec3(uniform, pointLights[i].position);

			sprintf_s(uniform, "pointlight[%i].color", i);
			Instance.setVec3(uniform, pointLights[i].color);

			sprintf_s(uniform, "pointlight[%i].constant", i);
			Instance.setFloat(uniform, pointLights[i].constant);

			sprintf_s(uniform, "pointlight[%i].linear", i);
			Instance.setFloat(uniform, pointLights[i].linear);

			sprintf_s(uniform, "pointlight[%i].quadratic", i);
			Instance.setFloat(uniform, pointLights[i].quadratic);

			sprintf_s(uniform, "pointlight[%i].specular", i);
			Instance.setVec3(uniform, pointLights[i].specular);
		}
		Instance.setMat("view", MainCamera.getViewMat());
		Instance.setMat("projection", proj);
		Instance.setFloat("Time", (float)glfwGetTime());
		Instance.setVec3("ambiant", glm::vec3(0.2f, 0.2f, 0.2f));
		Instance.setVec3("camPos", MainCamera.pos);
		Instance.setMat("projection", proj);
		Instance.setMat("model", plane.myPos);
		drawInstancedObj(instanceTest, cube);
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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		MainCamera = Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		if (Mouse){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Mouse = false;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			Mouse = true;
		}
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