#version 460 //GLSL version your computer supports

out vec4 daColor;
in vec3 theColor;
in vec3 theColor1;
in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform sampler2D myTextureSampler;
uniform vec3 lightPositionWorld;
uniform vec3 lightPositionWorld1;
uniform vec3 eyePositionWorld;
uniform float kd;
uniform float kd1;
uniform float ks;
uniform float ks1;

void main()
{
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	brightness = clamp(brightness, 0.0, 1.0);
	float kdtemp = clamp(kd, 0.0, 1.0);
	vec4 diffuse = brightness * kdtemp * vec4(1.0f,1.0f,1.0f,1.0f);

	//second light diffuse
	vec3 lightVectorWorld1 = normalize(lightPositionWorld1 - vertexPositionWorld);
	float brightness1 = dot(lightVectorWorld1, normalize(normalWorld));
	brightness1 = clamp(brightness1, 0.0, 1.0);
	float kdtemp1 = clamp(kd1, 0.0, 1.0);
	vec4 diffuse1 = brightness1 * kdtemp1 * vec4(1.0f,0.0f,0.0f,1.0f); //red diffuse

	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalize(normalWorld));
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float temp = dot(reflectedLightVectorWorld, eyeVectorWorld);
	float s = clamp(temp, 0.0, 1.0);
	s = pow(s, 50);
	float kstemp = clamp(ks, 0.0, 1.0);
	vec4 specularLight =  s * kstemp * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//second light specular
	vec3 reflectedLightVectorWorld1 = reflect(-lightVectorWorld1, normalize(normalWorld));
	vec3 eyeVectorWorld1 = normalize(eyePositionWorld - vertexPositionWorld);
	float temp1 = dot(reflectedLightVectorWorld1, eyeVectorWorld1);
	float s1 = clamp(temp1, 0.0, 1.0);
	s1 = pow(s1, 50);
	float kstemp1 = clamp(ks1, 0.0, 1.0);
	vec4 specularLight1 =  s1 * kstemp1 * vec4(1.0f, 0.0f, 0.0f, 1.0f);

	vec4 realColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	
	//daColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	//daColor = vec4(theColor,1.0) * realColor;
	daColor =  vec4(theColor,1.0) * realColor + diffuse + specularLight;
	daColor = daColor + vec4(theColor1,1.0) * realColor + diffuse1 + specularLight1;
	//daColor = vec4(theColor,1.0) + vec4(1.0,0.0,0.0,1.0) * diffuse + specularLight;
}
