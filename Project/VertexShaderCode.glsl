#version 460

in layout(location=0) vec3 position;
in layout(location=1) vec2 uv;
in layout(location=2) vec3 normal;

uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;
uniform vec3 ambientLight;
uniform float ka;

out vec3 theColor;
out vec2 UV;
out vec3 normalWorld;
out vec3 vertexPositionWorld; //use for calculating light pos and vertex vector

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 new_position = modelTransformMatrix * v;
	vec4 projectedPosition = projectionMatrix * new_position;
	gl_Position = projectedPosition;

	vec4 normal_temp = modelTransformMatrix * vec4(normal, 0);
	normalWorld = normal_temp.xyz;
	vertexPositionWorld = new_position.xyz;
	float tempka = clamp(ka, 0.0, 1.0);
	theColor = ambientLight * tempka;
	UV = uv;
}