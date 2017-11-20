#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

#include <string>	// for planet name _ ass1

// planet structure _ ass1
struct planet {
	std::string name;	// name
	double size;	// diameter (km)
	double speed;	// rotation speed (km/s)
	double dist;	// distance from the axis to its origin (10^6km)
	float color[3];	// r, g, b values
};

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // planets in a solar system _ ass1
  // name, diameter(km), speed(km/s), distance(10^6km)

  planet planets[10] = { 
	  {"Sun", 1392000, 0.0, 0.0, { 0.96f, 0.56f, 0.22f }},
	  {"Mercury", 4879, 47.4, 57.91, { 0.35f, 0.33f, 0.3f }},
	  {"Venus", 12100, 35.0, 108.2, { 0.94f, 0.9f, 0.55f }},
	  {"Earth", 12740, 29.8, 149.6, { 0.0f, 0.32f, 0.65f }},
	  {"Moon", 3474, 0.2, 0.384,{ 1.0f, 0.99f, 0.84f } },
	  {"Mars", 6779, 24.1, 227.9,{ 0.5f, 0.0f, 0.0f }},
	  {"Jupiter", 139800, 13.1, 778.6,{ 0.9f, 0.58f, 0.0f }},
	  {"Saturn", 116500, 9.7, 1433.0,{ 0.9f, 0.75f, 0.54f }},
	  {"Uranus", 50720, 6.8, 2877.0, { 0.69f, 0.893f, 0.93f }},
	  {"Neptune", 49250, 5.4, 4503.0, {0.69f, 0.81f, 0.93f }}
  };
 
  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects _ ass1
  void render() const;
  // calculates and uploads the Model - and Normal Matrix _ ass1
  void upload_planet_transforms(planet aPlanet) const;
  // draw stars _ ass2
  void makeStars() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
};

#endif