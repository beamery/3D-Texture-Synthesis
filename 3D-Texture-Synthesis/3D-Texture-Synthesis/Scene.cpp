#include "Scene.h"

/*
 * Initializes the ambient, diffuse, and specular components of the scene's objects
 * as well as setting the specular hotspot size (smaller number = wider hotspot).
 * Also initialize the lights for the scene as well as their ADS components.
 */
Scene::Scene() : 
	cube(vec3(0.4f, 0.4f, 0.4f), vec3(1.0f, 1.0f, 1.0f), vec3(0.3f, 0.3f, 0.3f), 10.0f),
	painting("starry_night")
{
	// light position in world space
	lightPos = vec4(3.0f, 1.0f, 1.0f, 1.0f);
	lightDiffuse = vec3(0.5f, 0.5f, 0.5f);
	lightAmbient = vec3(0.9f, 0.9f, 0.9f);
	lightSpecular = vec3(0.65f, 0.5f, 0.5f);

	// add stools to surround the table
}

/*
 * Release any pointers allocated for the scene
 */
Scene::~Scene() {
}


/*
 * Initialize the meshes for the objects in the scene.
 */
bool Scene::init() {
	bool success = cube.init();
	if (!success) return false;

	success = painting.init(); 
	if (!success) return false;

	return true;
}


void Scene::update(float elapsedTime) {
}


bool Scene::draw(Shader &shader, MatrixStack &mvs, const mat4 &proj, 
				 const ivec2 &size, const float time) {

	shader.use();
	shader.setUniform("time", time);
	//shader.setUniform("size", size);

	mvs.push();

	mvs.active = scale(mvs.active, vec3(WORLD_UNITS_PER_METER, WORLD_UNITS_PER_METER, WORLD_UNITS_PER_METER));
	mvs.push();
	//mvs.active = rotate(mvs.active, 20 * time, vec3(1.0f, 0.0f, 0.0f));
	vec4 eyeLightPos = mvs.active * lightPos;
	mvs.pop();
	//mvs.active = rotate(mvs.active, 10 * time, vec3(0.0f, 1.0f, 0.0f));

	// on random intervals change the light color slightly to emulate the
	// flickering of light given off by flames
	if (rand() % 10 < 5) {
		adjLightAmb = lightAmbient;
		adjLightDiff = lightDiffuse;
		adjLightSpec = lightSpecular;

		float adjustment = mix(-0.02f, 0.02f, float(rand()) / RAND_MAX);
		adjLightAmb += adjustment;
		adjLightDiff += adjustment;
		adjLightSpec += adjustment;
	}


	// push the light properties to the shader
	shader.setUniform("lightPosition", eyeLightPos);
	shader.setUniform("La", lightAmbient);
	shader.setUniform("Ld", lightDiffuse);
	shader.setUniform("Ls", lightSpecular);

	// push light properties to the texture shader
	textureShader->use();
	textureShader->setUniform("lightPosition", eyeLightPos);
	textureShader->setUniform("Ld", lightDiffuse);
	textureShader->setUniform("La", lightAmbient);
	textureShader->setUniform("Ls", lightSpecular);

	mvs.push();
	//mvs.active = translate(mvs.active, vec3(-WALL_DIST + 0.001f, 1.5f, -1.5f));
	mvs.active = rotate(mvs.active, 90.0f, vec3(0.0f, 1.0f, 0.0f));
	mvs.active = scale(mvs.active, vec3(1.28f * 1.5f, 1.014f * 1.5f, 1.0f));
	painting.draw(*textureShader, mvs, proj);
	mvs.pop();

	mvs.push();
	mvs.active = rotate(mvs.active, time * 45.0f, vec3(0.0f, 1.0f, 0.0f));
	cube.draw(shader, mvs.active, proj);
	mvs.pop();

	// draw scene objects
	bool success = true;
	//cube.draw(shader, mvs.active, proj);

	mvs.pop();

	return true;
}

void Scene::moveLight(float x, float z) {
	lightPos.z += z;
	lightPos.x += x;
}

void Scene::setTextureShader(Shader *shader) {
	textureShader = shader;	
}