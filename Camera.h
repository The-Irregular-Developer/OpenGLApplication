#pragma once
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 up, GLint yaw, GLint pitch, GLfloat moveSpeed, GLfloat turnSpeed);
	Camera();
	void updateCamera();
	void KeysMove(bool* keys, GLfloat deltaTime);
	void MouseMove(GLfloat xChange, GLfloat yChange);
	glm::mat4 createLookAtMatrix();
	glm::vec3 GetCameraPosition();
	~Camera();

private:
	glm::vec3 pos;
	glm::vec3 worldUp;
	glm::vec3 cameraUp;
	glm::vec3 front;
	glm::vec3 right;
	GLfloat yaw, pitch;
	GLfloat moveSpeed;
	GLfloat turnSpeed;
};

