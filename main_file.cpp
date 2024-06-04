/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#define TINYOBJLOADER_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <chrono>

#include "constants.h"
#include "lodepng.h"
#include "myCube.h"
#include "myTeapot.h"
#include "shaderprogram.h"

#include "camera.h"
#include "VAO.h"
#include "EBO.h"
#include "particle.h"

#include "tiny_obj_loader.h"
#include "physics.h"
#include "hitbox.h"

#define PERSISTANCE 0.5
#define OCTAVES 4
#define FREQUENCY 0.3
#define TERRAIN_SIZE 30
#define TRIANGLE_SIDE 1.0f

#define ALPHA_ANGLE 89.0
#define BETA_ANGLE 0.0

using namespace std;
using namespace glm;

Camera* camera;

ShaderProgram* sp;
ShaderProgram* sp_light;
ShaderProgram* sp_particle;

VAO* terrain_VAO;
GLuint terrain_indi_size;
GLuint grass_base, grass_spec;

VAO* vao;
VBO* vbo;
VBO* vbo2;
VBO* vbo3;
EBO* ebo;

VAO* light_vao;
VBO* light_vbo;
EBO* light_ebo;

ParticleSystem* explosionParticles;

float speed_x = 0;
float speed_y = 0;

float velocity = 7.0;
float time_total = 0.0;

//std::vector<GLfloat> vertices = {};
//std::vector<GLfloat> normals = {};
//std::vector<GLfloat> texCoords = {};
//std::vector<GLuint> indices = {};

// Trójkąt 2D
//std::vector<GLfloat> vertices = {
//	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
//	0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
//	0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
//};
//GLfloat colors[] = {
//	1.0f, 0.0f, 0.0f, 1.0f,
//	0.0f, 1.0f, 0.0f, 1.0f,
//	0.0f, 0.0f, 1.0f, 1.0f
//};
//std::vector<GLuint> indices = {
//	0, 1, 2
//};

// Plaszczyzna 2D
//std::vector<GLfloat> vertices = {
//	-1.0f, -1.0f, 0.0f,
//	1.0f, -1.0f, 0.0f,
//	1.0f, 1.0f, 0.0f,
//	-1.0f, 1.0f, 0.0f
//};
//std::vector<GLfloat> normals = {
//	0.0f, 1.0f, 0.0f,
//	0.0f, 1.0f, 0.0f,
//	0.0f, 1.0f, 0.0f,
//	0.0f, 1.0f, 0.0f
//};
//std::vector<GLfloat> texCoords = {
//	1.0f, 1.0f,
//	0.0f, 1.0f,
//	0.0f, 0.0f,
//	1.0f, 0.0f
//};
//std::vector<GLuint> indices = {
//	0, 1, 2,
//	2, 3, 0
//};

//// piramida 3D
//GLfloat vertices[] = {
//-0.5f, 0.0f, 0.5f, 1.0f,
//-0.5f, 0.0f, -0.5f, 1.0f,
//0.5f, 0.0f, -0.5f, 1.0f,
//0.5f, 0.0f, 0.5f, 1.0f,
//0.0f, 0.8f, 0.0f, 1.0f
//};
//GLfloat colors[] = {
//1.0f, 0.0f, 0.0f, 1.0f,
//0.0f, 1.0f, 0.0f, 1.0f,
//0.0f, 0.0f, 1.0f, 1.0f,
//1.0f, 1.0f, 0.0f, 1.0f,
//0.0f, 1.0f, 1.0f, 1.0f
//};
//GLfloat texCoords[] = {
//2.0f, 2.0f,
//2.0f, 0.0f,
//0.0f, 0.0f,
//2.0f, 0.0f,
//1.0f, 2.0f
//};
//GLuint indices[] = {
//2, 1, 0,
//3, 2, 0,
//0, 1, 4,
//1, 2, 4,
//2, 3, 4,
//3, 0, 4
//};

glm::vec3 initial_position(-2.0f, -1.0f, 0.0f);

//light cube
std::vector<GLfloat> light_vert = {
	-0.1f, -0.1f, 0.1f,
	-0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, -0.1f,
	0.1f, 0.1f, -0.1f,
	0.1f, 0.1f, 0.1f
};
std::vector<GLuint> light_indi = {
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4,
	0, 4, 7,
	7, 3, 0,
	1, 5, 6,
	6, 2, 1,
	0, 1, 5,
	5, 4, 0,
	3, 2, 6,
	6, 7, 3
};
glm::vec4 lightColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);



// Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void mousePosCallback(GLFWwindow* window, double x, double y) {
	static double lastX = 0;
	static double lastY = 0;

	double dx = lastX - x;
	double dy = lastY - y;

	lastX = x;
	lastY = y;

	camera->rotate(window, dx, dy);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_SPACE) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, camera->width / 2, camera->height / 2);
			glfwSetCursorPosCallback(window, mousePosCallback);
		}
		if (key == GLFW_KEY_W) camera->speed.x = PI;
		if (key == GLFW_KEY_S) camera->speed.x = -PI;
		if (key == GLFW_KEY_D) camera->speed.z = PI;
		if (key == GLFW_KEY_A) camera->speed.z = -PI;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_SPACE) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(window, NULL);
			camera->firstMouse = true;
		}
		if (key == GLFW_KEY_W) camera->speed.x = 0;
		if (key == GLFW_KEY_S) camera->speed.x = 0;
		if (key == GLFW_KEY_D) camera->speed.z = 0;
		if (key == GLFW_KEY_A) camera->speed.z = 0;
		if (key == GLFW_KEY_P) explosionParticles->emit(100);
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	glViewport(0, 0, width, height);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	// Wczytanie do pamięci komputera
	std::vector<unsigned char> image;  // Alokuj wektor do wczytania obrazka
	unsigned width, height;            // Zmienne do których wczytamy wymiary obrazka
	// Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	// Import do pamięci karty graficznej
	glGenTextures(1, &tex);             // Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex);  // Uaktywnij uchwyt
	// Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wygeneruj mipmapy
	glGenerateMipmap(GL_TEXTURE_2D);

	return tex;
}

//bool readModel(const char* filename) {
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string err;
//
//	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename)) {
//		fprintf(stderr, "Nie można wczytać modelu %s: %s\n", filename, err.c_str());
//		return false;
//	}
//
//	for (const auto& shape : shapes) {
//		size_t index_offset = 0;
//		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
//			size_t fv = shape.mesh.num_face_vertices[f];
//			for (size_t v = 0; v < fv; v++) {
//				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
//				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
//				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
//				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
//
//				if (idx.normal_index >= 0) {
//					normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
//					normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
//					normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
//				}
//
//				if (idx.texcoord_index >= 0) {
//					texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
//					texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
//				}
//				indices.push_back(index_offset + v);
//			}
//			index_offset += fv;
//		}
//	}
//
//	return true;
//}

float perlinNoise(float x, float y, float frequency, int octaves, float persistence) {
	float total = 0.0f;
	float amplitude = 1.0f;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin(vec2(x * frequency, y * frequency)) * amplitude;
		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total;
}

vector<GLfloat> generatePlane(int div)
{
	vector<GLfloat> plane;

	float triangleSide = TRIANGLE_SIDE;
	for (int row = 0; row < div + 1; row++)
	{
		for (int col = 0; col < div + 1; col++)
		{
			vec3 crntVec = vec3(col * triangleSide, row * -triangleSide, perlinNoise(row, col, FREQUENCY, OCTAVES, PERSISTANCE));
			plane.push_back(crntVec.x);
			plane.push_back(crntVec.y);
			plane.push_back(crntVec.z);
		}
	}

	return plane;
}

vector<GLuint> getPlaneInd(int div)
{
	vector<GLuint> indices;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;

			indices.push_back(index);
			indices.push_back(index + (div + 1) + 1);
			indices.push_back(index + (div + 1));

			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (div + 1) + 1);
		}
	}
	return indices;
}

vector<GLfloat> getPlaneNormals(const vector<GLfloat>& plane, int div)
{
	vector<GLfloat> normals;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;

			vec3 a1(plane[3 * (index + (div + 1))] - plane[3 * index],
				plane[3 * (index + (div + 1)) + 1] - plane[3 * index + 1],
				plane[3 * (index + (div + 1)) + 2] - plane[3 * index + 2]);

			vec3 b(plane[3 * (index + (div + 1) + 1)] - plane[3 * index],
				plane[3 * (index + (div + 1) + 1) + 1] - plane[3 * index + 1],
				plane[3 * (index + (div + 1) + 1) + 2] - plane[3 * index + 2]);

			vec3 a2(plane[3 * (index + 1)] - plane[3 * index],
				plane[3 * (index + 1) + 1] - plane[3 * index + 1],
				plane[3 * (index + 1) + 2] - plane[3 * index + 2]);

			vec3 normal1 = normalize(cross(a1, b));
			vec3 normal2 = normalize(cross(b, a2));

			normals.push_back(normal1.x);
			normals.push_back(normal1.y);
			normals.push_back(normal1.z);

			normals.push_back(normal2.x);
			normals.push_back(normal2.y);
			normals.push_back(normal2.z);
		}
	}
	return normals;
}

// Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	// Ustawienie koloru czyszczenia
	glClearColor(0.13, 0.13, 0.13, 1);
	glEnable(GL_DEPTH_TEST);

	// Ustawienie cullface
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Callback funkcje
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	// Wczytanie programu cieniującego upostępnionego przez prowadzącego
	sp = new ShaderProgram("v_shader.glsl", NULL, "f_shader.glsl");
	//sp_light = new ShaderProgram("v_light_shader.glsl", NULL, "f_light_shader.glsl");
	//sp_particle = new ShaderProgram("v_particle_shader.glsl", NULL, "f_particle_shader.glsl");

	// Wczytanie tekstur
	grass_base = readTexture("/textures/grass_base.jpg");
	grass_spec = readTexture("/textures/grass_h.jpg");
	// Wczytanie modelu
	//readModel("models/cornellbox.obj");

	// Utworzenie obiektu kamery
	camera = new Camera(800, 600, glm::vec3(0.0f, 0.0f, 4.0f));

	// Utworzenie planszy
	std::vector<GLfloat> verts = {
	-1.0f,  0.0f,-1.0f,
	1.0f,  0.0f,-1.0f,
	1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 1.0f,
	};
	std::vector<GLfloat> norms = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	std::vector<GLfloat> texCoords = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
	std::vector<GLuint> inds = {
		0, 1, 2,
		2, 3, 0
	};
	//vector<GLfloat> verts = generatePlane(TERRAIN_SIZE);
	//vector<GLuint> inds = getPlaneInd(6);
	//vector<GLfloat> norms = getPlaneNormals(verts, TERRAIN_SIZE);

	terrain_indi_size = inds.size();
	terrain_VAO = new VAO();
	terrain_VAO->Bind();
	VBO terrain_Verts = VBO(&verts[0], verts.size() * sizeof(verts[0]));
	VBO terrain_Normals = VBO(&norms[0], norms.size() * sizeof(norms[0]));
	VBO terrain_TexCoords = VBO(&texCoords[0], texCoords.size() * sizeof(texCoords[0]));
	EBO terrain_Inds = EBO(&inds[0], inds.size() * sizeof(inds[0]));
	terrain_VAO->LinkAttrib(&terrain_Verts, 0, 3, GL_FLOAT, 0, (void*)0);
	terrain_VAO->LinkAttrib(&terrain_Normals, 1, 3, GL_FLOAT, 0, (void*)0);
	terrain_VAO->LinkAttrib(&terrain_TexCoords, 2, 2, GL_FLOAT, 0, (void*)0);
	terrain_Verts.Unbind();
	terrain_Normals.Unbind();
	terrain_TexCoords.Unbind();
	terrain_VAO->Unbind();
	terrain_Inds.Unbind();

	//// Wczytanie modelu
	//vao = new VAO();
	//vao->Bind();
	//vbo = new VBO(&light_vert[0], light_vert.size() * sizeof(light_vert[0]));
	//vbo2 = new VBO(myCubeNormals, sizeof(myCubeNormals));
	//vbo3 = new VBO(&texCoords[0], texCoords.size() * sizeof(texCoords[0]));
	//ebo = new EBO(&light_indi[0], light_indi.size() * sizeof(light_indi[0]));
	//vao->LinkAttrib(vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	//vao->LinkAttrib(vbo2, 1, 3, GL_FLOAT, 0, (void*)0);
	//vao->LinkAttrib(vbo3, 2, 2, GL_FLOAT, 0, (void*)0);
	//vbo->Unbind();
	//vbo2->Unbind();
	//vbo3->Unbind();
	//vao->Unbind();
	//ebo->Unbind();

	// Utworzenie obiektu światła
	light_vao = new VAO();
	light_vao->Bind();
	light_vbo = new VBO(&light_vert[0], light_vert.size() * sizeof(light_vert[0]));
	light_ebo = new EBO(&light_indi[0], light_indi.size() * sizeof(light_indi[0]));
	light_vao->LinkAttrib(light_vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	light_vbo->Unbind();
	light_vao->Unbind();
	light_ebo->Unbind();

	// Utworzenie particle system
	ParticleInfo info;
	info.position = glm::vec3(-1.0f, 1.0f, 0.0f);
	info.sizeEnd = 1.0f;
	info.ttl = 10.0f;
	info.gravity = glm::vec3(0.0f, -9.0f, 0.0f);
	info.velocity = glm::vec3(0.0f, 2.0f, 0.0f);
	info.velocityVariation = glm::vec3(1.0f, 1.0f, 1.0f);
	explosionParticles = new ParticleSystem(info);
	explosionParticles->emit(100);
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	// Usunięcie programu cieniującego
	delete sp;
	delete sp_light;

	// Usunięcie obiektu kamery
	delete camera;

	// Usunięcie obiektu VAO
	vao->Delete();
	delete vao;

	// Usunięcie obiektu VBO
	vbo->Delete();
	delete vbo;

	vbo2->Delete();
	delete vbo2;

	vbo3->Delete();
	delete vbo3;

	// Usunięcie obiektu EBO
	ebo->Delete();
	delete ebo;

	// Usunięcie światła
	light_vao->Delete();
	delete light_vao;

	light_vbo->Delete();
	delete light_vbo;

	light_ebo->Delete();
	delete light_ebo;

	// Usunięcie terenu
	terrain_VAO->Delete();
	delete terrain_VAO;

	// Usunięcie particle system
	delete explosionParticles;
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	// Wyczyść bufery
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Aktualizacja macierzy widoku
	camera->updateMatrix(60.0f, 0.01f, 50.0f);

	//// Rysowanie pocisku
	//sp->use();

	//glm::mat4 M = glm::mat4(1.0f);
	//vec3 direction = getMovementCoords(velocity, ALPHA_ANGLE, BETA_ANGLE, time_total, initial_position);
	//M = glm::translate(M, direction);

	//glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	//glUniform4fv(sp->u("lightColor"), 1, glm::value_ptr(lightColor));
	//camera->shaderMatrix(*sp);

	//vao->Bind();

	//glDrawElements(GL_TRIANGLES, inds.size(), GL_UNSIGNED_INT, NULL);

	//// Rysowanie światła
	//glm::mat4 LM = glm::mat4(1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	//LM = glm::translate(LM, lightPos);

	//sp_light->use();

	//glUniformMatrix4fv(sp_light->u("M"), 1, false, glm::value_ptr(LM));
	//glUniform4fv(sp_light->u("lightColor"), 1, glm::value_ptr(lightColor));
	//glUniform3fv(sp_light->u("lightPos"), 1, glm::value_ptr(lightPos));
	//glUniform3fv(sp_light->u("viewPos"), 1, glm::value_ptr(camera->position));
	//camera->shaderMatrix(*sp_light);

	//light_vao->Bind();

	//glDrawElements(GL_TRIANGLES, light_indi.size(), GL_UNSIGNED_INT, NULL);

	//// Rysowanie particle system
	//explosionParticles->render(*sp_particle, *camera);

	// Rysowanie terenu
	sp->use();
	//Grass Base
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, grass_base);
	//glUniform1i(sp->u("tex0"), 0);
	////Grass Specular
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, grass_spec);
	//glUniform1i(sp->u("tex1"), 0);
	//Light and View uniforms
	glUniform4fv(sp->u("lightColor"), 1, glm::value_ptr(lightColor));
	glUniform3fv(sp->u("lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(sp->u("viewPos"), 1, glm::value_ptr(camera->position));
	//Model matrix
	glm::mat4 TM = glm::mat4(1.0f);
	TM = glm::translate(TM, glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(TM));
	//Draw terrain
	terrain_VAO->Bind();
	glDrawElements(GL_TRIANGLES, terrain_indi_size, GL_UNSIGNED_INT, NULL);



	// Uaktywnij teksturę
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, defaultTexture);
	//glUniform1i(sp->u("tex0"), 0);

	// Przerzuć tylny bufor na przedni
	glfwSwapBuffers(window);
}

int main(void) {
	using namespace std::chrono;
	GLFWwindow* window;

	// Zarejestruj procedurę obsługi błędów
	glfwSetErrorCallback(error_callback);

	// Zainicjuj bibliotekę GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	// Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	window = glfwCreateWindow(800, 600, "Maggots3D", NULL, NULL);

	// Jeżeli okna nie udało się utworzyć, to zamknij program
	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwMakeContextCurrent(window);

	// Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	// ustaw na 0 by wylaczyc Vsync, default 1
	glfwSwapInterval(1);

	// Zainicjuj bibliotekę GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	// Operacje inicjujące
	initOpenGLProgram(window);

	// Główna pętla
	float angle_x = 0;
	float angle_y = 0;
	float time = 0;
	float frames = 0;
	float avg_fps = 0;
	glfwSetTime(0);

	while (!glfwWindowShouldClose(window))
	{
		// Oblicz liczbę klatek na sekundę
		frames++;
		time += glfwGetTime();
		if (time >= 1.0 / 4.0) {
			avg_fps = (avg_fps + frames / time) / 2;
			float ms = time / frames * 1000.0;
			fprintf(stdout, "AVG FPS: %f, ms: %f\n", avg_fps, ms);
			time = 0;
			frames = 0;
		}

		angle_x += speed_x * glfwGetTime();
		angle_y += speed_y * glfwGetTime();

		// Przesuń kamerę
		camera->move(glfwGetTime());

		time_total += glfwGetTime();

		// Aktualizuj particle system
		explosionParticles->update(glfwGetTime());

		// Zeruj timer
		glfwSetTime(0);

		// Wykonaj procedurę rysującą
		drawScene(window, angle_x, angle_y);

		// Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
		glfwPollEvents();
	}

	// Zwolnij zasoby zajęte przez program
	freeOpenGLProgram(window);

	// Usuń kontekst OpenGL i okno
	glfwDestroyWindow(window);

	// Zwolnij zasoby zajęte przez GLFW
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
