#ifndef SCENE_H
#define SCENE_H

#define WORLD_UNITS_PER_METER 1.10f

// in meters
#define GRID_SIZE 11
#define WALL_DIST 4.0f
#define WALL_HEIGHT 4
#define WALL_LENGTH 10

#ifndef METERS_PER_INCH
#define METERS_PER_INCH 0.0254f
#endif

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MatrixStack.h"
#include "Shader.h"
#include "Mesh.h"
#include "Square.h"
#include "Cube.h"

using namespace std;
using namespace glm;

class Scene {

public:
	Scene();
	~Scene();
	bool init();
	void update(float elapsedTime);
	bool draw(Shader &shader, MatrixStack &mvs, const mat4 &proj, const ivec2 &size, const float time);
	void moveLight(float x, float z);
	void adjustStoolHeight(float amount);
	void setTextureShader(Shader *shader);

private:
	Cube cube;

	Square painting;
	Shader *textureShader;

	vec4 lightPos;
	vec3 lightDiffuse;
	vec3 lightAmbient;
	vec3 lightSpecular;

	vec3 adjLightDiff;
	vec3 adjLightAmb;
	vec3 adjLightSpec;
};


#endif