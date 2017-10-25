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
	  {"Sun", 1392000, 0.0, 0.0},
	  {"Mercury", 4879, 47.4, 57.91},
	  {"Venus", 12100, 35.0, 108.2},
	  {"Earth", 12740, 29.8, 149.6},
	  {"Moon", 3474, 0.2, 0.384},
	  {"Mars", 6779, 24.1, 227.9},
	  {"Jupiter", 139800, 13.1, 778.6},
	  {"Saturn", 116500, 9.7, 1433.0},
	  {"Uranus", 50720, 6.8, 2877.0},
	  {"Neptune", 49250, 5.4, 4503.0}
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

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
};

#endif