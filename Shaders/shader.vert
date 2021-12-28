#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoordinates;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 colors;

out vec4 vCol;
out vec2 texCoord;
out vec3 vertexNormals;
out vec3 fragPos;
out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);
	texCoord = textureCoordinates;
	fragPos = vec3(modelMatrix * vec4(pos, 1.0));
	vertexColor = colors;
	vertexNormals = normals;
	vCol = vec4(pos, 1.0);
}
