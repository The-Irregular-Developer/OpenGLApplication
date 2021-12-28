#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, GLint yaw, GLint pitch, GLfloat moveSpeed, GLfloat turnSpeed)
{
	this->pos = pos;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;

	this->moveSpeed = moveSpeed;
	this->turnSpeed = turnSpeed;

	updateCamera();
}

Camera::Camera()
{
	
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = -90.0f;
	pitch = 0.0f;

	moveSpeed = 5.0f;
	turnSpeed = 0.5f;

	updateCamera();
}

void Camera::KeysMove(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		pos += front * velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		pos -= front * velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		pos += right * velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		pos -= right * velocity;
	}
	if (keys[GLFW_KEY_SPACE])
	{
		pos.y += 1.0f * velocity;
	}

	updateCamera();
}

void Camera::MouseMove(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	updateCamera();
}

void Camera::updateCamera()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::cross(worldUp, front);
	cameraUp = glm::cross(front, right);
}

glm::mat4 Camera::createLookAtMatrix()
{
	return glm::lookAt(pos, pos + front, worldUp);
}

glm::vec3 Camera::GetCameraPosition()
{
	return pos;
}

Camera::~Camera()
{
}
