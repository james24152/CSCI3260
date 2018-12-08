/*********************************************************
FILE : main.cpp (csci3260 2018-2019 Assignment 2)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155077549
Student Name: Chan Cheuk Pong
*********************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using glm::vec3;
using glm::mat4;

GLint programID, skyboxProgram;
GLuint vao[5];
GLuint vaoSkybox;
GLsizei drawSize;
GLsizei drawSize2;
GLsizei drawSize3;
GLsizei drawSize4;
GLsizei drawSize5;
GLsizei drawSizeSkyBox;
GLuint texture;
GLuint texture0;
GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;
GLuint textureSky; //bind with texture5
GLuint texture6;
glm::vec3 SCTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec4 SC_world_Front_Direction;
glm::vec4 SC_world_Right_Direction;
glm::vec3 SCInitialPos = glm::vec3(60.0f, 0.0f, 60.0f); //initial position of the Space Craft
glm::mat4 SC_Rot_M;
glm::mat4 Cam_Rot_M;
glm::mat4 Cam_Pt_Rot_M;
glm::mat4 SC_TransformMatrix;
glm::mat4 Camera_TransformMatrix;
glm::vec4 Camera_world_position;
glm::vec4 SC_world_pos;
glm::vec4 Cam_point_pos;
glm::vec3 SC_local_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 SC_local_right = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 SC_local_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Camera_local_position = glm::vec3(5.0f, 5.0f, 5.0f);
float kd = 0.3;
float ks = 1.0;
float ka = 1.0;
float kd1 = 0.0;
float ks1 = 0.0;
float ka1 = 0.0;
int textureNum = 0;
float angle = 0.0;
float x_pos = 0.0;
float y_pos = 0.0;
int forwards = 0;
int spin = 0;
float SC_spinAngle = -110.0f;
float Cam_spinAngle = 25.0f;
float planetSpinAngle = 0.0f;
glm::vec2 mousePosition = glm::vec2(256.0,256.0);
float oldx = 256.0;
float roll = 0.0;
float yaw = 0.0;
float pitch = 0.0;
int cam = 0;
// Could define the Vao&Vbo and interaction parameter here


//a series utilities for setting shader parameters 
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

int installShaders(char* vertexShader, char* fragmentShader)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode(vertexShader);
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode(fragmentShader);
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID)) {
		printf("Cannot create program with %s, %s\n", vertexShader, fragmentShader);
		return -1;
	}

	int program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);
	glLinkProgram(program);

	if (!checkProgramStatus(program)) {
		printf("Cannot create program with %s, %s\n", vertexShader, fragmentShader);
		return -1;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return program;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') {
		kd -= 0.1;
	}
	if (key == 'w') {
		kd += 0.1;
	}
	if (key == 'z') {
		ks -= 0.1;
	}
	if (key == 'x') {
		ks += 0.1;
	}
	if (key == 'a') {
		ka -= 0.1;
	}
	if (key == 's') {
		ka += 0.1;
	}
	//second light source
	if (key == 'o') {
		kd1 -= 0.1;
	}
	if (key == 'p') {
		kd1 += 0.1;
	}
	if (key == 'n') {
		ks1 -= 0.1;
	}
	if (key == 'm') {
		ks1 += 0.1;
	}
	if (key == 'k') {
		ka1 -= 0.1;
	}
	if (key == 'l') {
		ka1 += 0.1;
	}
}

void move(int key, int x, int y) 
{
	if (key == GLUT_KEY_DOWN)
	{
		SCTranslation[0] = SCTranslation[0] + 0.5* SC_world_Front_Direction[0];
		SCTranslation[2] = SCTranslation[2] + 0.5* SC_world_Front_Direction[2];
	}
	if (key == GLUT_KEY_UP)
	{
		SCTranslation[0] = SCTranslation[0] - 0.5* SC_world_Front_Direction[0];
		SCTranslation[2] = SCTranslation[2] - 0.5* SC_world_Front_Direction[2];
	}
	if (key == GLUT_KEY_LEFT)
	{
		SCTranslation[0] = SCTranslation[0] + 0.5* SC_world_Right_Direction[0];
		SCTranslation[2] = SCTranslation[2] + 0.5* SC_world_Right_Direction[2];
	}
	if (key == GLUT_KEY_RIGHT)
	{
		SCTranslation[0] = SCTranslation[0] - 0.5* SC_world_Right_Direction[0];
		SCTranslation[2] = SCTranslation[2] - 0.5* SC_world_Right_Direction[2];
	}
}

void PassiveMouse(int x, int y)
{
	if (x<oldx)
	{
		SC_spinAngle += 1.0f;
		SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(SC_spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Cam_spinAngle += 1.0f;
		Cam_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(Cam_spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Cam_Pt_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(Cam_spinAngle+45), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (x>oldx)
	{
		SC_spinAngle -= 1.0f;
		SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(SC_spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Cam_spinAngle -= 1.0f;
		Cam_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(Cam_spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Cam_Pt_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(Cam_spinAngle + 45), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	oldx = x;


}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; 
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;
}



GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3; 
	if (dataPos == 0)      dataPos = 54; 

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	
	GLuint textureID;
	//TODO: Create one OpenGL texture and set the texture parameter 
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
		GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	// OpenGL has now copied the data. Free our own version
	delete[] data;
	
	
	return textureID;
}

GLuint loadCubemap(vector<const GLchar*> faces) {
	unsigned int width, height;
	const GLchar* imagepath;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE5); //we use texture 5 for skybox
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (GLuint i = 0; i < faces.size(); i++) {
		imagepath = faces.at(i);
		printf("Reading image %s\n", imagepath);

		unsigned char header[54];
		unsigned int dataPos;
		unsigned int imageSize;
		unsigned char * data;

		FILE * file = fopen(imagepath, "rb");
		if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

		if (fread(header, 1, 54, file) != 54) {
			printf("Not a correct BMP file\n");
			return 0;
		}
		if (header[0] != 'B' || header[1] != 'M') {
			printf("Not a correct BMP file\n");
			return 0;
		}
		if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
		if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);
		if (imageSize == 0) imageSize = width*height * 3;
		if (dataPos == 0) dataPos = 54;

		data = new unsigned char[imageSize];
		fread(data, 1, imageSize, file);
		fclose(file);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		delete[] data;
	}


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

void sendDataToOpenGL()
{
	//skybox start
	GLfloat skyboxVertices[] =
	{
		-1.0f, +1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,

		-1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,

		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,

		-1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f
	};
	GLuint vboSkybox;
	glGenVertexArrays(1, &vaoSkybox); //vao and vbo for skybox
	glGenBuffers(1, &vboSkybox);
	glBindVertexArray(vaoSkybox);
	glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	drawSizeSkyBox = GLint(sizeof(skyboxVertices));
	vector<const GLchar*> Skybox_faces;
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_rt.bmp");
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_lf.bmp");
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_up.bmp");
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_dn.bmp");
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_bk.bmp");
	Skybox_faces.push_back("texture/universe_skybox/purplenebula_ft.bmp");
	textureSky = loadCubemap(Skybox_faces);


	//end of skybox

	
	//space craft model
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("spaceCraft.obj", vertices, uvs, normals);
	texture = loadBMP_custom("texture/spacecraftTexture.bmp");
	texture0 = loadBMP_custom("texture/ringTexture.bmp");
	texture1 = loadBMP_custom("texture/earthTexture.bmp");
	texture2 = loadBMP_custom("theme2.bmp");
	texture3 = loadBMP_custom("theme3.bmp");
	texture4 = loadBMP_custom("block_texture.bmp");
	texture6 = loadBMP_custom("Green.bmp");
	glGenVertexArrays(5, vao);
	GLuint vbo[15];
	glGenBuffers(15, vbo);


	glBindVertexArray(vao[0]); 


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
		&vertices[0], GL_STATIC_DRAW);
	//vbo for vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//vbo[1]
	//vbo for uv
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0],
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	
	//vbo[2] for normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
		&normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	drawSize = vertices.size();

	

	//first ring
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	bool res2 = loadOBJ("Ring.obj", vertices2, uvs2, normals2);

	glBindVertexArray(vao[1]);  //VAO for ring model

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3),
		&vertices2[0], GL_STATIC_DRAW);
	//vbo for vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//vbo[1]
	//vbo for uv
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0],
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//vbo[2] for normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3),
		&normals2[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize2 = vertices2.size();

	//second ring
	std::vector<glm::vec3> vertices3;
	std::vector<glm::vec2> uvs3;
	std::vector<glm::vec3> normals3;
	bool res3 = loadOBJ("Ring.obj", vertices3, uvs3, normals3);

	glBindVertexArray(vao[2]);  //VAO for ring model

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3),
		&vertices3[0], GL_STATIC_DRAW);
	//vbo for vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);


	//vbo for uv
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, uvs3.size() * sizeof(glm::vec2), &uvs3[0],
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//for normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(glm::vec3),
		&normals3[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize3 = vertices3.size();

	//third ring
	std::vector<glm::vec3> vertices4;
	std::vector<glm::vec2> uvs4;
	std::vector<glm::vec3> normals4;
	bool res4 = loadOBJ("Ring.obj", vertices4, uvs4, normals4);

	glBindVertexArray(vao[3]);  //VAO for ring model

	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(glm::vec3),
		&vertices4[0], GL_STATIC_DRAW);
	//vbo for vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);


	//vbo for uv
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, uvs4.size() * sizeof(glm::vec2), &uvs4[0],
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//for normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glBufferData(GL_ARRAY_BUFFER, normals4.size() * sizeof(glm::vec3),
		&normals4[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize4 = vertices4.size();

	//Earth
	std::vector<glm::vec3> vertices5;
	std::vector<glm::vec2> uvs5;
	std::vector<glm::vec3> normals5;
	bool res5 = loadOBJ("planet.obj", vertices5, uvs5, normals5);
	
	glBindVertexArray(vao[4]);  //VAO for Earth model

	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ARRAY_BUFFER, vertices5.size() * sizeof(glm::vec3),
		&vertices5[0], GL_STATIC_DRAW);
	//vbo for vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);


	//vbo for uv
	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, uvs5.size() * sizeof(glm::vec2), &uvs5[0],
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	//for normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glBufferData(GL_ARRAY_BUFFER, normals5.size() * sizeof(glm::vec3),
		&normals5[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize5 = vertices5.size();
}

void UpdateStatus() {
	float scale = 0.005;
	glm::mat4 SC_scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	glm::mat4 SC_trans_M = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(SCInitialPos[0] + SCTranslation[0], SCInitialPos[1] + SCTranslation[1], SCInitialPos[2] + SCTranslation[2])
	);
	glm::mat4 Camera_trans_M1 = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(Camera_local_position[0], Camera_local_position[1], Camera_local_position[2])
	);
	glm::mat4 Camera_trans_M2 = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(SC_trans_M[3].x, SC_trans_M[3].y, SC_trans_M[3].z)
	);
	glm::mat4 Camera_point_M1 = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, -10.0f)
	);
	glm::mat4 Camera_point_M2 = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(SC_trans_M[3].x, SC_trans_M[3].y, SC_trans_M[3].z)
	);
	//printf("%f, %f, %f, %f\n", Camera_trans_M[3].x, Camera_trans_M[3].y, Camera_trans_M[3].z, Camera_trans_M[3].w);
	SC_TransformMatrix = SC_trans_M*SC_Rot_M*SC_scale_M;
	Camera_TransformMatrix = Camera_trans_M2*Cam_Rot_M*Camera_trans_M1;
	Cam_point_pos = Camera_point_M2*Cam_Pt_Rot_M*Camera_point_M1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//printf("%f, %f, %f\n", Cam_point_pos.x, Cam_point_pos.y, Cam_point_pos.z);
	SC_world_pos = SC_trans_M * glm::vec4(SC_local_pos, 1.0f);
	SC_world_Front_Direction = SC_TransformMatrix * glm::vec4(SC_local_front, 0.0f);
	SC_world_Right_Direction = SC_TransformMatrix * glm::vec4(SC_local_right, 0.0f);
	SC_world_Front_Direction = normalize(SC_world_Front_Direction);
	SC_world_Right_Direction = normalize(SC_world_Right_Direction);
	Camera_world_position = Camera_TransformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//printf("%f, %f, %f\n", Camera_world_position.x, Camera_world_position.y, Camera_world_position.z);
	//camera
}


void paintGL(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //specify the white background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	//first define the view matrix and the projection matrix
	mat4 viewMatrix = glm::lookAt(glm::vec3(Camera_world_position), glm::vec3(Cam_point_pos), glm::vec3(0.0f, 1.0f, 0.0f));
	mat4 projectionMatrix = glm::perspective(20.0f, 1.0f, 1.0f, 200.0f);
	glUseProgram(skyboxProgram); //configure skybox using the skybox program
	

	glm::mat4 Skb_ModelMatrix = glm::mat4(1.0f);
	Skb_ModelMatrix = glm::scale(Skb_ModelMatrix, vec3(100.0f));
	//remove any translation component of the view matrix
	//glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	//glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenWidth, 0.1f, 100.0f);
	//glm::mat4 view = viewMatrix;//glm::mat4(glm::mat3(0.0f));
	//glm::mat4 projection = glm::perspective(1.0f, 1.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "M"), 1, GL_FALSE, &Skb_ModelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
	//skybox cube
	glBindVertexArray(vaoSkybox);
	glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 5);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureSky);
	glDrawArrays(GL_TRIANGLES, 0, drawSizeSkyBox);


	glDepthMask(GL_TRUE);
	glUseProgram(programID);


	mat4 fpsMatrix = glm::mat4(1.0f);

	glm::mat4 matRoll = glm::mat4(1.0f);//identity matrix; 
	glm::mat4 matPitch = glm::mat4(1.0f);//identity matrix
	glm::mat4 matYaw = glm::mat4(1.0f);//identity matrix

	//roll, pitch and yaw are used to store our angles in our class
	matRoll = glm::rotate(matRoll, roll, glm::vec3(0.0f, 0.0f, 1.0f));
	matPitch = glm::rotate(matPitch, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	matYaw = glm::rotate(matYaw, yaw, glm::vec3(0.0f, 1.0f, 0.0f));

	//order matters
	glm::mat4 rotatefps = matRoll * matPitch * matYaw;

	fpsMatrix = rotatefps;



	//mat4 combinedMatrix = projectionMatrix * rotatefps;
	mat4 combinedMatrix = projectionMatrix * viewMatrix;
	GLint projectionMatrixUniformLocation = glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &combinedMatrix[0][0]); //set projection view

	//first light source
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight"); //ambient light
	vec3 ambientLight(1.0f, 1.0f, 1.0f);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	//second light source
	GLint ambientLight1UniformLocation = glGetUniformLocation(programID, "ambientLight1"); //ambient light
	vec3 ambientLight1(1.0f, 0.0f, 0.0f);//red
	glUniform3fv(ambientLight1UniformLocation, 1, &ambientLight1[0]);


	//first light source
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld"); //diffuse light
	vec3 lightPosition;
	lightPosition = vec3(60.0f, 0.0f, 60.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	
	//second light source
	GLint lightPositionUniformLocation1 = glGetUniformLocation(programID, "lightPositionWorld1"); //diffuse light
	vec3 lightPosition1;
	lightPosition1 = vec3(-40.0f, 0.0f, 0.0f);
	glUniform3fv(lightPositionUniformLocation1, 1, &lightPosition1[0]);


	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition = vec3(Camera_world_position);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);

	GLint kdLocation = glGetUniformLocation(programID, "kd");
	glUniform1f(kdLocation, kd);

	GLint ksLocation = glGetUniformLocation(programID, "ks");
	glUniform1f(ksLocation, ks);

	GLint kaLocation = glGetUniformLocation(programID, "ka");
	glUniform1f(kaLocation, ka);

	GLint kd1Location = glGetUniformLocation(programID, "kd1");
	glUniform1f(kd1Location, kd1);

	GLint ks1Location = glGetUniformLocation(programID, "ks1");
	glUniform1f(ks1Location, ks1);

	GLint ka1Location = glGetUniformLocation(programID, "ka1");
	glUniform1f(ka1Location, ka1);

	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix2 = glm::mat4(1.0f);
	glm::mat4 realTranslationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 translationMatrix2 = glm::mat4(1.0f);
	glm::mat4 fullRotationMatrix = glm::mat4(1.0f);
	glm::mat4 translationRefMatrix = glm::mat4(1.0f);
	glm::mat4 translationRef2Matrix = glm::mat4(1.0f);
	glm::mat4 rotationRefMatrix = glm::mat4(1.0f);
	glm::vec4 finalRefMatrix = glm::vec4(1.0f);
	glm::vec4 forwardVector = glm::vec4(1.0f);
	glm::mat4 forwardMatrix = glm::mat4(1.0f);
	glm::vec4 finalVector = glm::vec4(1.0f);

	GLint modelTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "modelTransformMatrix"); //get location of transform matrix
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	glBindVertexArray(vao[0]);
	
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler"); //texture handling
	if ((glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, 0.0f)) < 10.0f) || (glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, -30.0f)) < 10.0f) || (glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, -60.0f)) < 10.0f)) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glUniform1i(TextureID, 6);
	}
	else {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(TextureID, 0);
	}
	

	UpdateStatus();//update the transform matrix of space craft
	
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &SC_TransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSize);


	//draw for first ring vao[1]
	glBindVertexArray(vao[1]);

	GLuint TextureID1 = glGetUniformLocation(programID, "myTextureSampler"); //texture handling
	if (glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, 0.0f)) < 10.0f) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glUniform1i(TextureID1, 6);
	}
	else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glUniform1i(TextureID1, 1);
	}

	translationMatrix = glm::translate(glm::mat4(),
		glm::vec3(0.0f, 0.0f, 0.0f));;
	rotationMatrix = glm::rotate(mat4(), 1.57f, vec3(1, 0, 0));
	rotationMatrix2 = glm::rotate(mat4(), glm::radians(planetSpinAngle), vec3(0, 1, 0));
	scaleMatrix = glm::scale(glm::mat4(), glm::vec3(0.25f, 0.25f, 0.25f));
	modelTransformMatrix = translationMatrix * scaleMatrix * rotationMatrix2 * rotationMatrix;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSize2);

	glBindVertexArray(vao[2]);

	GLuint TextureID2 = glGetUniformLocation(programID, "myTextureSampler"); //texture handling
	if (glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, -30.0f)) < 10.0f) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glUniform1i(TextureID2, 6);
	}
	else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glUniform1i(TextureID2, 1);
	}

	translationMatrix = glm::translate(glm::mat4(),
		glm::vec3(0.0f, 0.0f, -30.0f));;
	rotationMatrix = glm::rotate(mat4(), 1.57f, vec3(1, 0, 0));
	rotationMatrix2 = glm::rotate(mat4(), glm::radians(planetSpinAngle), vec3(0, 1, 0));
	scaleMatrix = glm::scale(glm::mat4(), glm::vec3(0.25f, 0.25f, 0.25f));
	modelTransformMatrix = translationMatrix * scaleMatrix * rotationMatrix2 * rotationMatrix;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSize3);

	glBindVertexArray(vao[3]);

	GLuint TextureID3 = glGetUniformLocation(programID, "myTextureSampler"); //texture handling
	if (glm::distance(glm::vec3(SC_world_pos), glm::vec3(0.0f, 0.0f, -60.0f)) < 10.0f) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glUniform1i(TextureID3, 6);
	}
	else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glUniform1i(TextureID3, 1);
	}

	translationMatrix = glm::translate(glm::mat4(),
		glm::vec3(0.0f, 0.0f, -60.0f));;
	rotationMatrix = glm::rotate(mat4(), 1.57f, vec3(1, 0, 0));
	rotationMatrix2 = glm::rotate(mat4(), glm::radians(planetSpinAngle), vec3(0, 1, 0));
	scaleMatrix = glm::scale(glm::mat4(), glm::vec3(0.25f, 0.25f, 0.25f));
	modelTransformMatrix = translationMatrix * scaleMatrix * rotationMatrix2 * rotationMatrix;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSize4);

	//Earth 
	glBindVertexArray(vao[4]);

	GLuint TextureID4 = glGetUniformLocation(programID, "myTextureSampler"); //texture handling
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glUniform1i(TextureID4, 2);
	
	translationMatrix = glm::translate(glm::mat4(),
		glm::vec3(0.0f, 0.0f, -150.0f));;
	rotationMatrix2 = glm::rotate(mat4(), glm::radians(planetSpinAngle), vec3(0, 1, 0));
	scaleMatrix = glm::scale(glm::mat4(), glm::vec3(15.0f, 15.0f, 15.0f));
	modelTransformMatrix = translationMatrix * rotationMatrix2 * scaleMatrix;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSize5);
	planetSpinAngle += 0.005f;
	
	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	programID = installShaders("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	skyboxProgram = installShaders("SkyboxVertexShaderCode.glsl", "SkyboxFragmentShaderCode.glsl");
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(1080, 720);
	glutCreateWindow("Assignment 2");
	
	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();
	glutDisplayFunc(paintGL);
	
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);
	
	glutMainLoop();

	return 0;
}