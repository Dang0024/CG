#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"	// _ ass4

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <math.h>	// log function _ ass1
#include <string>	// texture file name _ ass4

const int NUM_STARS = 1000;	// number of stars

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
	:Application{ resource_path }
	, planet_object{}
	, star_object{}
	, tex_object{}	// _ ass4
{
	initializeGeometry();
	initializeShaderPrograms();
	initializeTextures(); // _ ass4
}

void ApplicationSolar::render() const {

	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);

	// iterate over all planets to draw _ ass1
	for (int i = 0; i < 10; i++) {

		// to be dealt together with 'Earth' 
		if (planets[i].name == "Moon")
			continue;

		// Texture Usage _ ass4
		// activate the proper Texture Unit
		glActiveTexture(tex_object[i].target);
		// bind the proper texture object
		glBindTexture(GL_TEXTURE_2D, tex_object[i].handle);
		// location of the sampler uniform for shader
		int color_sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "ColorTex");
		// upload index of unit to sampler
		glUniform1i(color_sampler_location, i);

		// calculates and uploads the Model & Normal Matrix
		upload_planet_transforms(planets[i]);
	}

	// draw stars _ ass2
	makeStars();

	// create sky sphere _ ass4
	createSky();
}

// calculates and uploads the Model & Normal Matrix _ ass1
void ApplicationSolar::upload_planet_transforms(planet aPlanet) const {

	// scale the planet values to fit in the screen
	double planetSize = log(aPlanet.size); planetSize = planetSize * planetSize / 200;
	float planetDist = float(log(aPlanet.dist)); planetDist = planetDist * planetDist / 2 - 5;
	float planetSpeed = float(log(aPlanet.speed) / 3);

	// preventing the values of the sun to be infinity
	if (aPlanet.name == "Sun") {
		planetDist = 0.0f;
		planetSpeed = 0.0f;
	}

	// get model matrix
	glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * planetSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, planetDist });
	model_matrix = glm::scale(model_matrix, glm::fvec3{ planetSize, planetSize, planetSize });
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

	// add 'Moon' in case of 'Earth'
	if (aPlanet.name == "Earth") {

		// Texture Usage _ ass4
		// activate the proper Texture Unit
		glActiveTexture(tex_object[4].target);
		// bind Texture Object to the unit
		glBindTexture(GL_TEXTURE_2D, tex_object[4].handle);
		// get location of the uniform
		int color_sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "ColorTex");
		glUseProgram(m_shaders.at("planet").handle);
		// upload index of unit to sampler
		glUniform1i(color_sampler_location, 4);

		// scale the moon values
		float moonSpeed = float(planets[4].speed * 2);
		double moonSize = log(planets[4].size) / 20;
		float moonDist = float(planets[4].dist * 5);

		// get model matrix
		glm::fmat4 moon_matrix = glm::rotate(model_matrix, float(glfwGetTime() * -moonSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
		moon_matrix = glm::translate(moon_matrix, glm::fvec3{ 0.0f, 0.0f, moonDist });
		moon_matrix = glm::scale(moon_matrix, glm::fvec3{ moonSize, moonSize, moonSize });
		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
			1, GL_FALSE, glm::value_ptr(moon_matrix));

		// bind the VAO to draw
		glBindVertexArray(planet_object.vertex_AO);

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	}
}

// draw stars _ ass2
void ApplicationSolar::makeStars() const {
	// bind star shader
	glUseProgram(m_shaders.at("star").handle);

	// bind the VAO to draw
	glBindVertexArray(star_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawArrays(GL_POINTS, 0, NUM_STARS);
}

void ApplicationSolar::updateView() {
	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);

	// bind star shader program _ ass2
	glUseProgram(m_shaders.at("planet").handle);

	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));

	// bind star shader program _ ass2
	glUseProgram(m_shaders.at("star").handle);

	// upload matrix to gpu for stars _ ass2
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {

	// bind planet shader program _ ass2
	glUseProgram(m_shaders.at("planet").handle);

	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));

	// bind star shader program _ ass2
	glUseProgram(m_shaders.at("star").handle);

	// upload matrix to gpu for stars _ ass2
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
	updateUniformLocations();

	// bind new shader
	glUseProgram(m_shaders.at("planet").handle);

	// setting the light source _ ass3
	glUniform3f(m_shaders.at("planet").u_locs.at("lightSrc"), 0.0f, 0.0f, 0.0f);
	glUniform1i(m_shaders.at("planet").u_locs.at("modeswitch"), 1);

	updateView();
	updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {

	//wider
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, -0.1f });
		updateView();
	}	// smaller
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 0.1f });
		updateView();
	}	// left  
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -1.0f, 0.0f, 0.0f });
		updateView();
	}	// right
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 1.0f, 0.0f, 0.0f });
		updateView();
	}	// down
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, -1.0f, 0.0f });
		updateView();
	}	// up
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 1.0f, 0.0f });
		updateView();
	}	// 1: blinn-phong reflection 
	else if (key == GLFW_KEY_1) {
		glUseProgram(m_shaders.at("planet").handle);
		glUniform1i(m_shaders.at("planet").u_locs.at("modeswitch"), 1);
		updateView();
	}	// 2: cel-shading
	else if (key == GLFW_KEY_2) {
		glUseProgram(m_shaders.at("planet").handle);
		glUniform1i(m_shaders.at("planet").u_locs.at("modeswitch"), 2);
		updateView();
	}
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
	// mouse handling
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
	// store shader program objects in container
	m_shaders.emplace("planet", shader_program{ m_resource_path + "shaders/simple.vert",
		m_resource_path + "shaders/simple.frag" });
	// request uniform locations for shader program
	m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("planet").u_locs["lightSrc"] = -1;	// for blinn-phong reflection _ ass3
	m_shaders.at("planet").u_locs["modeswitch"] = -1;	// mode switch _ ass3
	m_shaders.at("planet").u_locs["ColorTex"] = -1;	// texture color _ ass4

	// store star shader program objects in container _ ass2
	m_shaders.emplace("star", shader_program{ m_resource_path + "shaders/star.vert",
		m_resource_path + "shaders/star.frag" });
	// request uniform locations for star shader program _ ass2
	m_shaders.at("star").u_locs["ViewMatrix"] = -1;
	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
	// also load the text coordinates _ ass4
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

	// generate vertex array object
	glGenVertexArrays(1, &planet_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(planet_object.vertex_AO);

	// generate generic buffer
	glGenBuffers(1, &planet_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	// first attribute is 3 floats with no offset & stride
	glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	// second attribute is 3 floats with no offset & stride
	glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

	// Add texture coordinates as attribute _ ass4
	// activate third attribute on gpu
	glEnableVertexAttribArray(2);
	// third attribute is 3 floats with no offset & stride
	glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

	// generate generic buffer
	glGenBuffers(1, &planet_object.element_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
	// configure currently bound array buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

	// store type of primitive to draw
	planet_object.draw_mode = GL_TRIANGLES;
	// transfer number of indices to model object 
	planet_object.num_elements = GLsizei(planet_model.indices.size());


	// ************ load stars _ ass2 ************

	// container of floats for star positions and colors
	GLfloat starAttrib[2 * 3 * NUM_STARS];
	int idx = -1;	// array index

					// create random values for each star
	for (int i = 0; i < NUM_STARS; i++) {
		// 3 random position values for x, y, z
		for (int j = 0; j < 3; j++) {
			// position value from -20 to 20
			starAttrib[++idx] = (rand() % 41) - 20;
		}
		// 3 random color values for r, g, b
		for (int j = 0; j < 3; j++) {
			// color vale from 0.5 to 1.0 for bright colors
			starAttrib[++idx] = ((float)rand() / (float)RAND_MAX) / 2.0 + 0.5;
		}
	}

	// generate vertex array object
	glGenVertexArrays(1, &star_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(star_object.vertex_AO);

	// generate generic buffer
	glGenBuffers(1, &star_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(starAttrib), starAttrib, GL_STATIC_DRAW);

	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	// first attribute is 3 floats with offset of 0 & stride of 6 floats 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	// second attribute is 3 floats with offset & stride of 6 floats
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	// generate generic buffer
	glGenBuffers(1, &star_object.element_BO);

	// store type of primitive to draw
	star_object.draw_mode = GL_POINTS;
	// transfer number of indices to model object 
	star_object.num_elements = GLsizei(NUM_STARS);
}

// Texture Specification _ ass4
void ApplicationSolar::initializeTextures() {
	
	std::string filename;

	// for each planet + sky
	for (int i = 0; i < 11; i++) {


		// load image name
		if (i == 10)
			filename = "Sky";
		else
			filename = planets[i].name;

		// load image file
		pixel_data texData = texture_loader::file(m_resource_path + "textures/" + filename + ".png");

		// activate proper texture unit
		tex_object[i].target = GL_TEXTURE0 + i;
		glActiveTexture(tex_object[i].target);
		// create texture object for each planet
		glGenTextures(1, &tex_object[i].handle);
		// bind object
		glBindTexture(GL_TEXTURE_2D, tex_object[i].handle);

		// define sampling parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// define texture data and format
		glTexImage2D(GL_TEXTURE_2D, 0, texData.channels, texData.width, texData.height, 0, texData.channels, GL_UNSIGNED_BYTE, texData.ptr());
	}
}

// sky texture mapping _ ass4
void ApplicationSolar::createSky() const {

	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);
	
	// Texture Usage _ ass4
	// activate the proper Texture Unit
	glActiveTexture(tex_object[10].target);
	// bind the proper texture object
	glBindTexture(GL_TEXTURE_2D, tex_object[10].handle);
	// location of the sampler uniform for shader
	int color_sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "ColorTex");
	// upload index of unit to sampler
	glUniform1i(color_sampler_location, 10);

	// get model matrix
	glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * sky.speed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, sky.dist });
	model_matrix = glm::scale(model_matrix, glm::fvec3{ sky.size, sky.size, sky.size });
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

ApplicationSolar::~ApplicationSolar() {
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);

	glDeleteBuffers(1, &star_object.vertex_BO);
	glDeleteBuffers(1, &star_object.element_BO);
	glDeleteVertexArrays(1, &star_object.vertex_AO);

	for (int i = 0; i < 11; i++)
		glDeleteTextures(1, &tex_object[i].handle);	// _ ass4
}

// exe entry point
int main(int argc, char* argv[]) {
	Launcher::run<ApplicationSolar>(argc, argv);
}