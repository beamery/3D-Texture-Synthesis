/*
 * Brian Murray
 * CS 559 Project 2
 */
#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"
#include "Shader.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Utils.h"
#include "TextureManager.h"

using namespace std;
using namespace glm;

struct Window {
	int window_handle;
	ivec2 size;
	float window_aspect;
	vector<string> instructions;
	float fov;
} window;

struct Options {
	int shader;
	float rotX, rotY;
	bool wireframe;
	int view;
	Shader *currentShader;
	float oldTime;
	float startTime;
} options;

Shader *textureShader;

struct Camera {
	vec3 position;
	vec3 up;
	vec3 right;
} camera;


Scene scene;
vector<Shader*> shaders;
MatrixStack mvs;

void DisplayFunc() {


	if (window.window_handle == -1)
		return;

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, options.wireframe ? GL_LINE : GL_FILL);
	glViewport(0, 0, window.size.x, window.size.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvs.push();

	// time since start of program
	float newTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f - options.startTime;
	// time since last frame
	float elapsedTime = newTime - options.oldTime;
	options.oldTime = newTime;

	mat4 projection = perspective(window.fov, window.window_aspect, 0.1f, 1000.0f);

	// PERFORM CAMERA TRANSFORMS
	mvs.active = translate(mvs.active, vec3(0.0f, 0.0f, -3.0f));
	// put camera 4m above the scene and pull it back 10m
	//mvs.active = translate(mvs.active, vec3(0.0f, -4.0f, -10.0f));
	//mvs.active = rotate(mvs.active, 30.0f, vec3(1.0f, 0.0f, 0.0f));
	mvs.active = rotate(mvs.active, options.rotX, vec3(1.0f, 0.0f, 0.0f));
	mvs.active = rotate(mvs.active, options.rotY, vec3(0.0f, 1.0f, 0.0f));
	// END CAMERA TRANSFORMS

	// setup the current shader
	options.currentShader = shaders[options.shader];
	options.currentShader->use();
	textureShader->use();

	// update the scene with the amount of time since the last frame
	scene.update(elapsedTime);

	// draw the scene using the total elapsed time
	scene.draw(*options.currentShader, mvs, projection, window.size, newTime);
	
	mvs.pop();
	
	//DisplayInstructions();
	glutSwapBuffers();
	glutPostRedisplay();
}


void ReshapeFunc(int w, int h) {
	if (window.window_handle != -1 &&  h > 0)
	{
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);
	}
}

void KeyboardFunc(unsigned char c, int x, int y) {
	if (window.window_handle == -1)
		return;

	switch (c) {
	case 'j':
		break;
	case 'k':
		break;
	case 'i':
		window.fov--;
		break;
	case 'o':
		window.fov++;
		break;
	case 'p':
		Mesh::drawPoints = !Mesh::drawPoints;
		break;
	case 's':
		options.shader = (options.shader + 1) % shaders.size();
		break;
	case 'w':
		options.wireframe = !options.wireframe;
		break;
	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}
}

void SpecialFunc(int c, int x, int y) {
	if (window.window_handle == -1)
		return;

	switch (c) {
	case GLUT_KEY_RIGHT:
		//scene.moveLight(1.0f, 0.0f);
		options.rotY -= 2;
		break;
	case GLUT_KEY_LEFT:
		//scene.moveLight(-1.0f, 0.0f);
		options.rotY += 2;
		break;
	case GLUT_KEY_UP:
		//scene.moveLight(0.0f, -1.0f);
		options.rotX += 2;
		break;
	case GLUT_KEY_DOWN:
		//scene.moveLight(0.0f, 1.0f);
		options.rotX -= 2;
		break;
	}

}

void initShaders() {
	Shader *flatShader =  new Shader();
	flatShader->init("flat.vert", "flat.frag");
	flatShader->getUniformLocation("lightPosition");
	flatShader->getUniformLocation("Kd");
	flatShader->getUniformLocation("Ld");
	flatShader->getUniformLocation("Ka");
	flatShader->getUniformLocation("La");
	flatShader->getUniformLocation("Ks");
	flatShader->getUniformLocation("Ls");
	flatShader->getUniformLocation("shine");
	flatShader->getUniformLocation("view");
	flatShader->getUniformLocation("time");
	shaders.push_back(flatShader);

	Shader *gouraudShader = new Shader();
	gouraudShader->init("gouraud.vert", "gouraud.frag");
	gouraudShader->getUniformLocation("lightPosition");
	gouraudShader->getUniformLocation("Kd");
	gouraudShader->getUniformLocation("Ld");
	gouraudShader->getUniformLocation("Ka");
	gouraudShader->getUniformLocation("La");
	gouraudShader->getUniformLocation("Ks");
	gouraudShader->getUniformLocation("Ls");
	gouraudShader->getUniformLocation("shine");
	gouraudShader->getUniformLocation("view");
	shaders.push_back(gouraudShader);

	Shader *phongShader =  new Shader();
	phongShader->init("phong.vert", "phong.frag");
	phongShader->getUniformLocation("lightPosition");
	phongShader->getUniformLocation("Kd");
	phongShader->getUniformLocation("Ld");
	phongShader->getUniformLocation("Ka");
	phongShader->getUniformLocation("La");
	phongShader->getUniformLocation("Ks");
	phongShader->getUniformLocation("Ls");
	phongShader->getUniformLocation("shine");
	phongShader->getUniformLocation("view");
	shaders.push_back(phongShader);
}

void initTextures() {
	TextureManager *textureManager = new TextureManager(true);	
	TextureManager::get()->loadTexture("brick.png", "brick");
	TextureManager::get()->loadTexture("water_droplet.png", "water_droplet");
	TextureManager::get()->loadTexture("fire.png", "fire");
	TextureManager::get()->loadTexture("starry_night.png", "starry_night");

	textureShader = new Shader();
	textureShader->init("texture.vert", "texture.frag");
	textureShader->getTextureLocation("spriteTexture");
	textureShader->getUniformLocation("lightPosition");
	textureShader->getUniformLocation("Ld");
	textureShader->getUniformLocation("La");
	textureShader->getUniformLocation("Ls");
	textureShader->getUniformLocation("view");
	scene.setTextureShader(textureShader);
}


void initScene() {
	scene.init();
}

void setWindowInstructions() {
	window.instructions.push_back("Brian Murray - CS 559 Project 2");
	window.instructions.push_back("Herp Derp");
}


int main(int argc, char * argv[]) {
	setWindowInstructions();
	options.shader = 2;
	options.rotX = 0.0f;
	options.rotY = -45.0f;
	options.wireframe = false;
	options.view = 0;
	options.oldTime = 0.0f;
	window.fov = 50.0f;
	
	glutInit(&argc, argv);
	glutInitWindowSize(1280, 720);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("Modern Dalfred");
	glutReshapeFunc(ReshapeFunc);
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);

	window.instructions.push_back("Brian Murray - CS 559 Project 2");

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}
	initShaders();
	initTextures();
	initScene();

	options.startTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	glutMainLoop();
}
