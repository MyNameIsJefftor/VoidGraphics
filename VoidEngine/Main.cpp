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
#include "TestObj/Orb.h"
#include "TestObj/knight.h"

struct Scene {
	std::vector<Object*>* objs;
	std::vector<dirLight>* dirlight;
	std::vector<pointLight>* pointlight;
	std::vector<Shader*>* shaders;
	glm::vec3 ambi;
};


void framebuffer_size_callback(GLFWwindow* windo, int width, int height);
void processInput(GLFWwindow* window, Camera* cam);
void mouseCallback(GLFWwindow* window, double x, double y);
void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight> pointLights, glm::mat4 proj, glm::vec3 ambi);
Camera MainCamera;
double lastX = 0.0, lastY = 0.0;
bool firstPass = true, Mouse = true, grayScale = false;
float pitch = 0.0f, yaw = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int width = 800, height = 450;
int awidth = 16, aheight = 9;
int sceneNum = 0, maxScene = 2;
float aspect = awidth / aheight;

int main() {
	srand(14);
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
	Shader Flag("wave.vert", "normalFragShader.frag");
	Shader Instance("instance.vert", "normalFragShader.frag");
	Shader SkyboxShad("Skybox.vert", "Skybox.frag");

	//Arrays
	std::vector<Object*> mainObjects;
	std::vector<dirLight> maindirLights;
	std::vector<pointLight> mainpointLights;
	std::vector<Object*> secondObjects;
	std::vector<dirLight> seconddirLights;
	std::vector<pointLight> secondpointLights;
	std::vector<Shader*> Shaders;

	Shaders.push_back(&normal);
	Shaders.push_back(&Flag);
	Shaders.push_back(&Instance);

	//Convert obj from header to my format
#pragma region MyThemeImport
#pragma region CityImport
	std::vector<Vertex> CityImport;
	CityImport.resize(sizeof(city_triangulated_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(city_triangulated_data) / sizeof(OBJ_VERT); i++) {
		float x = city_triangulated_data[i].pos[0];
		float y = city_triangulated_data[i].pos[1];
		float z = city_triangulated_data[i].pos[2];
		CityImport[i].Position = glm::vec3(x, y, z);
		x = city_triangulated_data[i].nrm[0];
		y = city_triangulated_data[i].nrm[1];
		z = city_triangulated_data[i].nrm[2];
		CityImport[i].Normal = glm::vec3(x, y, z);
		x = city_triangulated_data[i].uvw[0];
		y = city_triangulated_data[i].uvw[1];
		CityImport[i].TexCord = glm::vec2(x, y);
		CityImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> CityInd;
	CityInd.resize(sizeof(city_triangulated_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(city_triangulated_indicies) / sizeof(unsigned int); i++) {
		CityInd[i] = city_triangulated_indicies[i];
	}
#pragma endregion
#pragma region ResourceImport
	std::vector<Vertex> ResImport;
	ResImport.resize(sizeof(resources_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(resources_data) / sizeof(OBJ_VERT); i++) {
		float x = resources_data[i].pos[0] * 0.2f;
		float y = resources_data[i].pos[1] * 0.2f;
		float z = resources_data[i].pos[2] * 0.2f;
		ResImport[i].Position = glm::vec3(x, y, z);
		x = resources_data[i].nrm[0];
		y = resources_data[i].nrm[1];
		z = resources_data[i].nrm[2];
		ResImport[i].Normal = glm::vec3(x, y, z);
		x = resources_data[i].uvw[0];
		y = resources_data[i].uvw[1];
		ResImport[i].TexCord = glm::vec2(x, y);
		ResImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> ResourceInd;
	ResourceInd.resize(sizeof(resources_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(resources_indicies) / sizeof(unsigned int); i++) {
		ResourceInd[i] = resources_indicies[i];
	}
#pragma endregion
#pragma region OrbImport
	std::vector<Vertex> OrbImport;
	OrbImport.resize(sizeof(Orb_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(Orb_data) / sizeof(OBJ_VERT); i++) {
		float x = Orb_data[i].pos[0] * 0.2f;
		float y = Orb_data[i].pos[1] * 0.2f;
		float z = Orb_data[i].pos[2] * 0.2f;
		OrbImport[i].Position = glm::vec3(x, y, z);
		x = Orb_data[i].nrm[0];
		y = Orb_data[i].nrm[1];
		z = Orb_data[i].nrm[2];
		OrbImport[i].Normal = glm::vec3(x, y, z);
		x = Orb_data[i].uvw[0];
		y = Orb_data[i].uvw[1];
		OrbImport[i].TexCord = glm::vec2(x, y);
		OrbImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> OrbInd;
	OrbInd.resize(sizeof(Orb_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(Orb_indicies) / sizeof(unsigned int); i++) {
		OrbInd[i] = Orb_indicies[i];
	}
#pragma endregion
#pragma endregion

#pragma region HolyGrailImport
#pragma region KnightImport
	std::vector<Vertex> KnightImport;
	KnightImport.resize(sizeof(knight_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(knight_data) / sizeof(OBJ_VERT); i++) {
		float x = knight_data[i].pos[0];
		float y = knight_data[i].pos[1];
		float z = knight_data[i].pos[2];
		KnightImport[i].Position = glm::vec3(x, y, z);
		x = knight_data[i].nrm[0];
		y = knight_data[i].nrm[1];
		z = knight_data[i].nrm[2];
		KnightImport[i].Normal = glm::vec3(x, y, z);
		x = knight_data[i].uvw[0];
		y = knight_data[i].uvw[1];
		KnightImport[i].TexCord = glm::vec2(x, y);
		KnightImport[i].Color = glm::vec3(0.5f, 0.5f, 0.0f);
	}
	std::vector<unsigned int> KnightInd;
	KnightInd.resize(sizeof(knight_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(knight_indicies) / sizeof(unsigned int); i++) {
		KnightInd[i] = knight_indicies[i];
	}
#pragma endregion
#pragma endregion
	// load and create a texture 
	// -------------------------

	Skybox Skybox = CreateSkybox("Texture/Skybox.ktx", SkyboxShad);

	Mesh ResourceMesh = Mesh(ResImport, ResourceInd, CreateTexture("Texture/Resource_diff.ktx", "Texture/Resource_spec.ktx"));
	Mesh OrbMesh(OrbImport, OrbInd, CreateTexture("Texture/planet_diff.ktx", "Texture/planet_spec.ktx"));
	Object City ( Mesh(CityImport, CityInd, CreateTexture("Texture/citydiff.ktx","Texture/cityspec.ktx")), &normal );
	Object Orb (OrbMesh, &normal);
	City.myPos = glm::translate(City.myPos, glm::vec3(0.0f, 1.08f, 0.0f));
	City.myPos = glm::rotate(City.myPos, glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	City.myPos = glm::scale(City.myPos, glm::vec3(0.25f));
	Orb.myPos = glm::translate(Orb.myPos, glm::vec3(0.0f, -0.5f, 0.0f));
	Orb.myPos = glm::scale(Orb.myPos, glm::vec3(8.0f));
	mainObjects.push_back(&Orb);
	mainObjects.push_back(&City);
	Mesh basicCube = CreateCube(CreateTexture("Texture/crate_diff.ktx", "Texture/Resource_spec.ktx"), 0.125f);
	Object RCube(OrbMesh, &normal);
	Object GCube(OrbMesh, &normal);
	Object BCube(OrbMesh, &normal);

	Object KnightObj(Mesh(KnightImport, KnightInd, CreateTexture("Texture/knight_diff.ktx", "Texture/knight_spec.ktx")), &normal);
	Object FlagObj(generatePlane(CreateTexture("Texture/banner_diff.ktx", "Texture/banner_spec.ktx"), 5), &Flag);
	Object ground(generatePlane(CreateTexture("Texture/grass_diff.ktx", "Texture/cityspec.ktx"), 5), &normal);
	ground.myPos = glm::translate(ground.myPos, glm::vec3(0.0f, -2.45f, 0.0f));
	FlagObj.myPos = glm::rotate(FlagObj.myPos, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	FlagObj.myPos = glm::translate(FlagObj.myPos, glm::vec3(0.0f, 2.0f, 0.0f));
	KnightObj.myPos = glm::scale(KnightObj.myPos, glm::vec3(0.25f));
	secondObjects.push_back(&KnightObj);
	secondObjects.push_back(&FlagObj);
	secondObjects.push_back(&ground);

	//Tell gl to put the universe into perspective
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);

	//create lights
	dirLight mainSun = {
		glm::normalize(glm::vec3(-0.25f, -1.0f, -0.25f)),
		glm::vec3(0.8f,0.4f,0.2f),
		glm::vec3(0.3f,0.2f,0.1f)
	};
	dirLight mainMoon = {
		glm::normalize(glm::vec3(0.25f, 1.0f, 0.25f)),
		glm::vec3(0.2f,0.3f,0.4f),
		glm::vec3(0.2f,0.3f,0.4f)
	};
	maindirLights.push_back(mainSun);
	maindirLights.push_back(mainMoon);
	dirLight secondSun = {
		glm::normalize(glm::vec3(-0.25f, -1.0f, -0.25f)),
		glm::vec3(1.0f,0.8f,0.5f),
		glm::vec3(1.0f,0.8f,0.5f)
	};
	seconddirLights.push_back(secondSun);

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
	pointLight purpFairy = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.8f, 0.1f, 0.9f),
		1.0f,
		0.7f,
		5.0f,
		glm::vec3(0.8f,0.1f,0.9f)
	};
	mainpointLights.push_back(red);
	mainpointLights.push_back(green);
	mainpointLights.push_back(blue);
	secondpointLights.push_back(purpFairy);
	
	glm::mat4* instanceTest = genRandomPos(glm::vec3(0.0f), 2.0f, 100, 0.0625f);
	instancedObj cubes( ResourceMesh, &Instance, instanceTest);
	mainObjects.push_back(&cubes);
	mainObjects.push_back(&RCube);
	mainObjects.push_back(&GCube);
	mainObjects.push_back(&BCube);
	Scene Main = { &mainObjects, &maindirLights, &mainpointLights, &Shaders, glm::vec3(0.01f) };
	Scene Secondary = { &secondObjects, &seconddirLights, &secondpointLights, &Shaders, glm::vec3(0.015f) };
	std::vector<Scene> scenes;
	scenes.push_back(Main);
	scenes.push_back(Secondary);
	while (!glfwWindowShouldClose(window)) {
		//Calc delta
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Get input
		processInput(window, &MainCamera);
		float sinlightz = 2.5f * sin((float)glfwGetTime());
		float sinlighty = 2.5f * sin((float)glfwGetTime());
		float coslighty = 2.5f * cos((float)glfwGetTime());
		float coslightx = 2.5f * cos((float)glfwGetTime());
		mainpointLights[0].position = glm::vec3(coslightx, sinlighty, 0.0f);
		mainpointLights[1].position = glm::vec3(coslightx, 0.0f, sinlightz);
		mainpointLights[2].position = glm::vec3(0.0f, coslighty, sinlightz);
		RCube.myPos = glm::translate(glm::mat4(1.0f), mainpointLights[0].position);
		GCube.myPos = glm::translate(glm::mat4(1.0f), mainpointLights[1].position);
		BCube.myPos = glm::translate(glm::mat4(1.0f), mainpointLights[2].position);
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
		setShaders(*scenes[sceneNum].shaders, *scenes[sceneNum].dirlight, *scenes[sceneNum].pointlight, proj, scenes[sceneNum].ambi);
		for (int i = 0; i < scenes[sceneNum].objs->size(); i++) {
			scenes[sceneNum].objs[0][i]->draw();
		}
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
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		sceneNum = 1;
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		sceneNum = 0;
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

void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight> pointLights, glm::mat4 proj, glm::vec3 ambi) {
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
		shaders[shad]->setVec3("ambient", ambi);
		shaders[shad]->setVec3("camPos", MainCamera.pos);
	}
}