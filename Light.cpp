#include "Light.h"

Light::Light()
{
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientStrength = 1.0f;

}

Light::Light(GLfloat red, GLfloat blue, GLfloat green, GLfloat ambientStrength, glm::vec3 directionLocation, GLfloat diffuseStrength, GLfloat specularStrength)
{
	lightColor = glm::vec3(red, green, blue);
	this->ambientStrength = ambientStrength;
	this->directionLocation = directionLocation;
	this->diffuseStrength = diffuseStrength;
	this->specularStrength = specularStrength;
}

void Light::useLight(GLuint colorLocation, GLuint ambientLocation, GLuint diffuseStrengthLocation, GLuint diffusePositionLocation, GLfloat specularStrengthLocation)
{
	glUniform3f(colorLocation, lightColor.x, lightColor.y, lightColor.z);
	glUniform1f(ambientLocation, ambientStrength);

	glUniform1f(diffuseStrengthLocation, diffuseStrength);
	glUniform3f(diffusePositionLocation, directionLocation.x, directionLocation.y, directionLocation.z);

	glUniform1f(specularStrengthLocation, specularStrength);
}
