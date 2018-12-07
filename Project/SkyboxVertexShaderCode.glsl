#version 460

in layout(location=0) vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 M;

out vec3 TexCoords;

void main(){
	vec4 pos = projection * view * M * vec4(position, 1.0f);
	gl_Position = pos;

	TexCoords = position;

}