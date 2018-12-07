#version 460 //GLSL version your computer supports

out vec4 daColor;
in vec3 theColor;
in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform sampler2D myTextureSampler;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform float kd;
uniform float ks;

void main()
{
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	brightness = clamp(brightness, 0.0, 1.0);
	float kdtemp = clamp(kd, 0.0, 1.0);
	vec4 diffuse = brightness * kdtemp * vec4(1.0f,1.0f,1.0f,1.0f);
	float distanceToLight = length(lightPositionWorld - vertexPositionWorld);

	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalize(normalWorld));
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float temp = dot(reflectedLightVectorWorld, eyeVectorWorld);
	float s = clamp(temp, 0.0, 1.0);
	s = pow(s, 50);
	float kstemp = clamp(ks, 0.0, 1.0);
	vec4 specularLight =  s * kstemp * vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vec4 realColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	
	//daColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	//daColor = vec4(theColor,1.0) * realColor;
	daColor =  vec4(theColor,1.0) + realColor * diffuse + specularLight;
	//daColor = vec4(theColor,1.0) + vec4(1.0,0.0,0.0,1.0) * diffuse + specularLight;
}
