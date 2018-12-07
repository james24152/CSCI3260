#version 460

in vec3 TexCoords;


uniform samplerCube skybox;

uniform vec3 fog_Color;

out vec4 fogfinalColor;


void main(){
	vec4 finalColor = texture(skybox, TexCoords);
	fogfinalColor = finalColor;
}