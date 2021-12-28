#version 330

in vec4 vCol;
uniform sampler2D theTexture;
in vec2 texCoord;
in vec3 vertexNormals;
in vec3 fragPos;
in vec3 vertexColor;

uniform vec3 lightColor;
uniform float ambientIntensity;
uniform float diffuseIntensity;
uniform float specularIntensity;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 colour;

void main()
{
	vec4 ambientColour = vec4(lightColor, 1.0f) * ambientIntensity;
	vec3 norm = normalize(vertexNormals);
	
	vec3 lightDirection = normalize(vec3(0.0, 1.0, 0.0));
	float diffuseFactor =  max(dot(norm, lightDirection), 0.0);
	vec4 diffuseColour = vec4(lightColor, 1.0f) * diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0,0,0,0);
	if(diffuseFactor > 0.0f) {
		vec3 reflection = normalize(reflect(lightDirection, norm));
		vec3 viewDirection = normalize(cameraPos - fragPos);
		float specularFactor = dot(viewDirection, reflection);

		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, 32);
			specularColour = vec4(specularFactor * lightColor * specularIntensity, 1.0f);
		}
	}
    colour = vec4(vertexColor, 1.0) * (ambientColour + diffuseColour + specularColour);
}
