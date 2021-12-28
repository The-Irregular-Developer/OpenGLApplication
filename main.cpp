#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

GLuint width = 800;
GLuint height = 600;
bool keys[1024];
GLuint VAO, VBO, VBO1, IBO, shader, model, projection, view, colorLocation, ambientIntensityLocation, diffuseIntensityLocation, specularIntensityLocation, diffusePosition, cameraPos;

Texture myTexture;

Light light;

int imageChannels;

bool mouseFirstMove = true;
GLfloat lastXPos;
GLfloat lastYPos;
GLfloat xChange;
GLfloat yChange;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

float radians = 3.14159265359f / 180.0f;

Camera camera;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

const char* vertShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
layout (location = 1) in vec2 textureCoordinates;										  \n\
layout (location = 2) in vec3 normals;										  \n\
layout (location = 3) in vec3 colors;										  \n\
																			  \n\
out vec4 vCol;																  \n\
out vec2 texCoord;															  \n\
out vec3 vertexNormals;														  \n\
out vec3 fragPos;															  \n\
out vec3 vertexColor;														  \n\
                                                                              \n\
uniform mat4 modelMatrix;                                                           \n\
uniform mat4 projectionMatrix;                                                \n\
uniform mat4 viewMatrix;													  \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);						  \n\
	texCoord = textureCoordinates;											  \n\
	fragPos = vec3(modelMatrix * vec4(pos, 1.0));								  \n\
	vertexColor = colors;													  \n\
	vertexNormals = normals;														  \n\
	vCol = vec4(pos, 1.0);								  \n\
}";

const char* fragShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
in vec4 vCol;																  \n\
uniform sampler2D theTexture;													  \n\
in vec2 texCoord;															  \n\
in vec3 vertexNormals;														  \n\
in vec3 fragPos;															  \n\
in vec3 vertexColor;														  \n\
																			  \n\
uniform vec3 lightColor;													  \n\
uniform float ambientIntensity;												  \n\
uniform float diffuseIntensity;												  \n\
uniform float specularIntensity;											  \n\
uniform vec3 lightPos;														  \n\
uniform vec3 cameraPos;														  \n\
                                                                              \n\
out vec4 colour;                                                              \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
	vec4 ambientColour = vec4(lightColor, 1.0f) * ambientIntensity;			  \n\
	vec3 norm = normalize(vertexNormals);									  \n\
																			  \n\
	vec3 lightDirection = normalize(vec3(0.0, 1.0, 0.0));								  \n\
	float diffuseFactor =  max(dot(norm, lightDirection), 0.0);		  \n\
	vec4 diffuseColour = vec4(lightColor, 1.0f) * diffuseIntensity * diffuseFactor;		\n\
																						\n\
	vec4 specularColour = vec4(0,0,0,0);												\n\
	if(diffuseFactor > 0.0f) {															\n\
		vec3 reflection = normalize(reflect(lightDirection, norm));									\n\
		vec3 viewDirection = normalize(cameraPos - fragPos);								\n\
		float specularFactor = dot(viewDirection, reflection);								\n\
																						\n\
		if(specularFactor > 0.0f)															\n\
		{																					\n\
			specularFactor = pow(specularFactor, 32);											\n\
			specularColour = vec4(specularFactor * lightColor * specularIntensity, 1.0f);				\n\
		}																									\n\
	}																									\n\
    colour = vec4(vertexColor, 1.0) * (ambientColour + diffuseColour + specularColour);					  \n\
}";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, -1.0f
	};

	GLfloat verticesColor[]
	{
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0] * 3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0] * 5)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, NULL, NULL);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void addShader(GLenum shaderType, GLuint program, const char* shaderSourceCode)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theSourceCode[1];

	theSourceCode[0] = shaderSourceCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderSourceCode);
	
	glShaderSource(theShader, 1, theSourceCode, codeLength);
	glCompileShader(theShader);

	GLint result;
	GLchar log[1000];
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(log), NULL, log);
		std::cout << "Error compiling with the " << shaderType << " shader:" << log << std::endl;
		return;
	}

	glAttachShader(program, theShader);
	
}

void CompileShader()
{
	shader = glCreateProgram();

	addShader(GL_VERTEX_SHADER, shader, vertShader);
	addShader(GL_FRAGMENT_SHADER, shader, fragShader);

	GLint result;
	GLchar log[1000];
	
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(log), NULL, log);
		std::cout << "Error Linking program, log: " << log << std::endl;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(log), NULL, log);
		std::cout << "Error Validating program, log: " << log << std::endl;
	}

	model = glGetUniformLocation(shader, "modelMatrix");
	projection = glGetUniformLocation(shader, "projectionMatrix");
	view = glGetUniformLocation(shader, "viewMatrix");
	colorLocation = glGetUniformLocation(shader, "lightColor");
	ambientIntensityLocation = glGetUniformLocation(shader, "ambientIntensity");
	diffuseIntensityLocation = glGetUniformLocation(shader, "diffuseIntensity");
	diffusePosition = glGetUniformLocation(shader, "lightPos");
	cameraPos = glGetUniformLocation(shader, "cameraPos");
	specularIntensityLocation = glGetUniformLocation(shader, "specularIntensity");
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if(action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (mouseFirstMove)
	{
		lastXPos = xPos;
		lastYPos = yPos;
		mouseFirstMove = false;
	}

	xChange = xPos - lastXPos;
	yChange = lastYPos - yPos;

	lastXPos = xPos;
	lastYPos = yPos;
}

int main()
{
	if (!glfwInit())
		{
			std::cout << "Unable to initialize GLFW" << std::endl;
			glfwTerminate();
			return 1;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		GLFWwindow* window = glfwCreateWindow(width, height, "OpenGLApplication", NULL, NULL);

		if (!window)
		{
			std::cout << "Problem Initializing Window" << std::endl;
			glfwTerminate();
			return 1;
		}

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Unable to initialize GLEW" << std::endl;
		}

		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, bufferWidth, bufferHeight);

		GLFWimage icons[1];
		icons[0].pixels = stbi_load("Images/GP.png", &icons[0].width, &icons[0].height, &imageChannels, 0);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCursorPosCallback(window, MouseMoveCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetWindowIcon(window, 1, icons);
		GLFWimage image[1];
		image[0].pixels = stbi_load("Images/Pointer.png", &icons[0].width, &icons[0].height, &imageChannels, 0);
		image[0].width = 50;
		image[0].height = 50;

		GLFWcursor* cursor = glfwCreateCursor(image, 0, 0);
		light = Light(1.0f, 1.0f, 1.0f, 0.1f, glm::vec3(0.0f, 0.0f, -1.0f), 0.3f, 1.0f);
		myTexture = Texture("Images/GP.png");
		myTexture.loadTexture();
		glfwSetCursor(window, cursor);
		CreateTriangle();
		CompileShader();

		for (int i = 0; i < 1024; i++)
		{
			keys[i] = false;
		}

		camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);
		glm::mat4 projectionMatrix(1.0f);
		projectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

		auto changeXY = [](GLfloat& xyChange) mutable
		{
			GLfloat theChange = xyChange;
			xyChange = 0.0f;
			return theChange;
		};

		while (!glfwWindowShouldClose(window))
		{
			GLfloat now = glfwGetTime();
			deltaTime = now - lastTime;
			lastTime = now;

			glfwPollEvents();

			camera.KeysMove(keys, deltaTime);
			camera.MouseMove(changeXY(xChange), changeXY(yChange));

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(shader);

			myTexture.useTexture();

			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -2.5f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.4, 1.0));

			light.useLight(colorLocation, ambientIntensityLocation, diffuseIntensityLocation, diffusePosition, specularIntensityLocation);

			glUniform3f(cameraPos, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);

			glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.createLookAtMatrix()));

			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(0);

			glfwSwapBuffers(window);
		}

		stbi_image_free(icons[0].pixels);
		stbi_image_free(image[0].pixels);
}