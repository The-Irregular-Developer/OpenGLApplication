#pragma once
#include <GL/glew.h>
#include <glm/vec3.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat blue, GLfloat green, GLfloat ambientStrength, glm::vec3 directionLocation, float diffuseStrength, GLfloat specularStrength);
	void useLight(GLuint colorLocation, GLuint ambientLocation, GLuint diffuseStrengthLocation, GLuint diffusePositionLocation, GLfloat specularStrengthLocation);
	
private:
	float ambientStrength;
	glm::vec3 lightColor;
	glm::vec3 directionLocation;
	float diffuseStrength;
	float specularStrength;
};
