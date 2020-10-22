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
//#include "TestObj/resources.h"
//#include "TestObj/Building.h"
//#include "TestObj/city_triangulated.h"
//#include "TestObj/groundplane.h"
//#include "TestObj/Orb.h"
//#include "TestObj/knight.h"
#include "Assets/Models/redgem.h"
#include "Assets/Models/greengem.h"
#include "Assets/Models/bluegem.h"
#include "Assets/Models/skullsword.h"

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
void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight*> pointLights, glm::mat4 proj, glm::vec3 ambi);
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
	Shader SkyboxShad("Skybox.vert", "Skybox.frag");

	Shader gemShader("normalVertShader.vert", "gem.frag");
	Shader swordShader("normalVertShader.vert", "sword.frag");

	//Arrays
	std::vector<Object*> Objects;
	std::vector<dirLight> dirLights;
	std::vector<pointLight*> pointLights;
	std::vector<Shader*> Shaders;

	Shaders.push_back(&normal);
	Shaders.push_back(&gemShader);
	Shaders.push_back(&swordShader);

	Skybox Skybox = CreateSkybox("Assets/Textures/FinalSky.ktx", SkyboxShad);
	std::vector<Texture> empty;
	std::vector<Texture> swordTex;
	swordTex.push_back(CreateCubeMap("Assets/Textures/FinalReflection.ktx"));
#pragma region FinalImports
#pragma region RedGem
	std::vector<Vertex> redgemImport;
	redgemImport.resize(sizeof(redgem_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(redgem_data) / sizeof(OBJ_VERT); i++) {
		float x = redgem_data[i].pos[0];
		float y = redgem_data[i].pos[1];
		float z = redgem_data[i].pos[2];
		redgemImport[i].Position = glm::vec3(x, y, z);
		x = redgem_data[i].nrm[0];
		y = redgem_data[i].nrm[1];
		z = redgem_data[i].nrm[2];
		redgemImport[i].Normal = glm::vec3(x, y, z);
		x = redgem_data[i].uvw[0];
		y = redgem_data[i].uvw[1];
		redgemImport[i].TexCord = glm::vec2(x, y);
		redgemImport[i].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	std::vector<unsigned int> redgemInd;
	redgemInd.resize(sizeof(redgem_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(redgem_indicies) / sizeof(unsigned int); i++) {
		redgemInd[i] = redgem_indicies[i];
	}
#pragma endregion
#pragma region greenGem
	std::vector<Vertex> greengemImport;
	greengemImport.resize(sizeof(greengem_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(greengem_data) / sizeof(OBJ_VERT); i++) {
		float x = greengem_data[i].pos[0];
		float y = greengem_data[i].pos[1];
		float z = greengem_data[i].pos[2];
		greengemImport[i].Position = glm::vec3(x, y, z);
		x = greengem_data[i].nrm[0];
		y = greengem_data[i].nrm[1];
		z = greengem_data[i].nrm[2];
		greengemImport[i].Normal = glm::vec3(x, y, z);
		x = greengem_data[i].uvw[0];
		y = greengem_data[i].uvw[1];
		greengemImport[i].TexCord = glm::vec2(x, y);
		greengemImport[i].Color = glm::vec3(0, 1.0f, 0);
	}
	std::vector<unsigned int> greengemInd;
	greengemInd.resize(sizeof(greengem_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(greengem_indicies) / sizeof(unsigned int); i++) {
		greengemInd[i] = greengem_indicies[i];
	}
#pragma endregion
#pragma region bluegem
	std::vector<Vertex> bluegemImport;
	bluegemImport.resize(sizeof(bluegem_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(bluegem_data) / sizeof(OBJ_VERT); i++) {
		float x = bluegem_data[i].pos[0];
		float y = bluegem_data[i].pos[1];
		float z = bluegem_data[i].pos[2];
		bluegemImport[i].Position = glm::vec3(x, y, z);
		x = bluegem_data[i].nrm[0];
		y = bluegem_data[i].nrm[1];
		z = bluegem_data[i].nrm[2];
		bluegemImport[i].Normal = glm::vec3(x, y, z);
		x = bluegem_data[i].uvw[0];
		y = bluegem_data[i].uvw[1];
		bluegemImport[i].TexCord = glm::vec2(x, y);
		bluegemImport[i].Color = glm::vec3(0, 0, 1.0f);
	}
	std::vector<unsigned int> bluegemInd;
	bluegemInd.resize(sizeof(bluegem_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(bluegem_indicies) / sizeof(unsigned int); i++) {
		bluegemInd[i] = bluegem_indicies[i];
	}
#pragma endregion
#pragma region skullsword
	std::vector<Vertex> skullswordImport;
	skullswordImport.resize(sizeof(skullsword_data) / sizeof(OBJ_VERT));
	for (int i = 0; i < sizeof(skullsword_data) / sizeof(OBJ_VERT); i++) {
		float x = skullsword_data[i].pos[0];
		float y = skullsword_data[i].pos[1];
		float z = skullsword_data[i].pos[2];
		skullswordImport[i].Position = glm::vec3(x, y, z);
		x = skullsword_data[i].nrm[0];
		y = skullsword_data[i].nrm[1];
		z = skullsword_data[i].nrm[2];
		skullswordImport[i].Normal = glm::vec3(x, y, z);
		x = skullsword_data[i].uvw[0];
		y = skullsword_data[i].uvw[1];
		skullswordImport[i].TexCord = glm::vec2(x, y);
		skullswordImport[i].Color = glm::vec3(1.0f);
	}
	std::vector<unsigned int> skullswordInd;
	skullswordInd.resize(sizeof(skullsword_indicies) / sizeof(unsigned int));
	for (int i = 0; i < sizeof(skullsword_indicies) / sizeof(unsigned int); i++) {
		skullswordInd[i] = skullsword_indicies[i];
	}
#pragma endregion
#pragma endregion
	Object redGemObj(Mesh(redgemImport, redgemInd, empty), &gemShader);
	Object greenGemObj(Mesh(greengemImport, greengemInd, empty), &gemShader);
	Object blueGemObj(Mesh(bluegemImport, bluegemInd, empty), &gemShader);
	Object skullsword1(Mesh(skullswordImport, skullswordInd, swordTex), &swordShader);
	Object skullsword2(Mesh(skullswordImport, skullswordInd, swordTex), &swordShader);
	Object ground(generatePlane(CreateTexture("Assets/Textures/ground_diff.ktx", "Assets/Textures/ground_diff.ktx")), &normal);
	Objects.push_back(&redGemObj);
	Objects.push_back(&greenGemObj);
	Objects.push_back(&blueGemObj);
	Objects.push_back(&skullsword1);
	Objects.push_back(&skullsword2);
	skullsword1.mySca = glm::vec3(0.5f);
	skullsword2.mySca = glm::vec3(0.5f);
	ground.mySca = glm::vec3(120);
	ground.basicMat();
	Objects.push_back(&ground);

	//Setup lights
	pointLight redLight = {
		glm::vec3(0.0f),
		glm::vec3(1.0,0,0),
		1.0f,
		0.4f,
		0.01f,
		glm::vec3(0.5,0,0)
	};
	pointLight greenLight = {
		glm::vec3(0.0f),
		glm::vec3(0,1.0,0),
		1.0f,
		0.4f,
		0.01f,
		glm::vec3(0,0.5,0)
	};
	pointLight blueLight = {
		glm::vec3(0.0f),
		glm::vec3(0,0,1.0),
		1.0f,
		0.4f,
		0.01f,
		glm::vec3(0,0,0.5)
	};
	pointLights.push_back(&redLight);
	pointLights.push_back(&blueLight);
	pointLights.push_back(&greenLight);
#ifdef OLD


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
#endif
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//set defaults for everything
	float offset = 360 * (1 / 3.0f);
	redGemObj.setPos(glm::vec3(0,2,4));
	redGemObj.rotateY(offset);
	greenGemObj.setPos(glm::vec3(0, 2, 4));
	greenGemObj.rotateY(offset*2);
	blueGemObj.setPos(glm::vec3(0, 2, 4));
	blueGemObj.rotateY(offset*3);

	skullsword1.myRot = glm::vec3(0, 0, 45);
	skullsword2.myRot = glm::vec3(0, 0, -45);
	while (!glfwWindowShouldClose(window)) {
		//Calc delta
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Get input
		processInput(window, &MainCamera);

		//gems
		redGemObj.rotateY(1/2.0f);
		redGemObj.gemMat();
		glm::vec4 redpos = redGemObj.myMat * glm::vec4(redGemObj.myPos, 0);
		redLight.position = redpos;

		greenGemObj.rotateY(1 / 2.0f);
		greenGemObj.gemMat();
		glm::vec4 greenpos = greenGemObj.myMat * glm::vec4(greenGemObj.myPos, 0);
		greenLight.position = greenpos;

		blueGemObj.rotateY(1 / 2.0f);
		blueGemObj.gemMat();
		glm::vec4 bluepos = blueGemObj.myMat * glm::vec4(blueGemObj.myPos, 0);
		blueLight.position = bluepos;

		//Swords
		skullsword1.rotateY(1 / 2.0f);
		skullsword2.rotateY(1 / 2.0f);
		skullsword1.myPos = glm::vec3(0, glm::sin(glfwGetTime())+3.5, -0.5f);
		skullsword2.myPos = glm::vec3(0, glm::cos(glfwGetTime()) + 3.5, 0.5f);
		skullsword1.swordMat();
		skullsword2.swordMat();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//DrawSkybox
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
		setShaders(Shaders, dirLights, pointLights, proj, glm::vec3(0.2));
		for (int i = 0; i < Objects.size(); i++) {
			Objects[i]->draw();
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

void setShaders(std::vector<Shader*> shaders, std::vector<dirLight> dirLights, std::vector<pointLight*> pointLights, glm::mat4 proj, glm::vec3 ambi) {
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
			shaders[shad]->setVec3(uniform, pointLights[i]->position);

			sprintf_s(uniform, "pointlight[%i].color", i);
			shaders[shad]->setVec3(uniform, pointLights[i]->color);

			sprintf_s(uniform, "pointlight[%i].constant", i);
			shaders[shad]->setFloat(uniform, pointLights[i]->constant);

			sprintf_s(uniform, "pointlight[%i].linear", i);
			shaders[shad]->setFloat(uniform, pointLights[i]->linear);

			sprintf_s(uniform, "pointlight[%i].quadratic", i);
			shaders[shad]->setFloat(uniform, pointLights[i]->quadratic);

			sprintf_s(uniform, "pointlight[%i].specular", i);
			shaders[shad]->setVec3(uniform, pointLights[i]->specular);
		}
		shaders[shad]->setMat("view", MainCamera.getViewMat());
		shaders[shad]->setMat("projection", proj);
		shaders[shad]->setFloat("Time", (float)glfwGetTime());
		shaders[shad]->setVec3("ambient", ambi);
		shaders[shad]->setVec3("camPos", MainCamera.pos);
	}
}