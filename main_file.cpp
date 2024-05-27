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

#include "constants.h"
#include "lodepng.h"
#include "myCube.h"
#include "myTeapot.h"
#include "shaderprogram.h"

#include "camera.h"
#include "VAO.h"
#include "EBO.h"
#include "tiny_obj_loader.h"

Camera* camera;
ShaderProgram* sp;

VAO* vao;
VBO* vbo;
VBO* vbo2;
VBO* vbo3;
EBO* ebo;

float speed_x = 0;
float speed_y = 0;

std::vector<GLfloat> vertices = {};
std::vector<GLfloat> normals = {};
std::vector<GLfloat> texCoords = {};
std::vector<GLuint> indices = {};

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

////// Plaszczyzna 2D
//std::vector<GLfloat> vertices = {
//	-1.0f, -1.0f, 0.0f,
//	1.0f, -1.0f, 0.0f,
//	1.0f, 1.0f, 0.0f,
//	-1.0f, 1.0f, 0.0f
//};
//std::vector<GLfloat> colors = {
//	1.0f, 0.0f, 0.0f,
//	0.0f, 1.0f, 0.0f,
//	0.0f, 0.0f, 1.0f,
//	1.0f, 1.0f, 0.0f
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

GLuint defaultTexture;

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

bool readModel(const char* filename) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename)) {
		fprintf(stderr, "Nie można wczytać modelu %s: %s\n", filename, err.c_str());
		return false;
	}

	for (const auto& shape : shapes) {
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			size_t fv = shape.mesh.num_face_vertices[f];
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

				if (idx.normal_index >= 0) {
					normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
					normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
					normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
				}

				if (idx.texcoord_index >= 0) {
					texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
					texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
				}
				indices.push_back(index_offset + v);
			}
			index_offset += fv;
		}
	}

	return true;
}

// Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	// Ustawienie koloru czyszczenia
	glClearColor(0.13, 0.13, 0.13, 1);
	glEnable(GL_DEPTH_TEST);

	// Ustawienie cullface
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CCW);

	// Callback funkcje
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	// Wczytanie programu cieniującego upostępnionego przez prowadzącego
	sp = new ShaderProgram("v_shader.glsl", NULL, "f_shader.glsl");

	// Wczytanie tekstur
	defaultTexture = readTexture("texture0.png");

	// Wczytanie modelu
	readModel("models/bunny.obj");

	//vertices = new std::vector<GLfloat>{
	//-1.0f, -1.0f, 0.0f,
	//1.0f, -1.0f, 0.0f,
	//1.0f, 1.0f, 0.0f,
	//-1.0f, 1.0f, 0.0f
	//};
	//indices = new std::vector<GLuint>{
	//0, 1, 2,
	//2, 3, 0
	//};


	// Utworzenie obiektu kamery
	camera = new Camera(800, 600, glm::vec3(0.0f, 0.0f, 4.0f));

	// Wczytanie modelu
	vao = new VAO();
	vao->Bind();
	vbo = new VBO(&vertices[0], vertices.size() * sizeof(vertices[0]));
	vbo2 = new VBO(&normals[0], normals.size() * sizeof(normals[0]));
	//vbo3 = new VBO(texCoords, sizeof(texCoords));
	ebo = new EBO(&indices[0], indices.size() * sizeof(indices[0]));
	vao->LinkAttrib(vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	vao->LinkAttrib(vbo2, 1, 3, GL_FLOAT, 0, (void*)0);
	//vao->LinkAttrib(vbo3, 2, 2, GL_FLOAT, 0, (void*)0);
	vbo->Unbind();
	vbo2->Unbind();
	//vbo3->Unbind();
	vao->Unbind();
	ebo->Unbind();
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;

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

	//vbo3->Delete();
	//delete vbo3;

	// Usunięcie obiektu EBO
	ebo->Delete();
	delete ebo;
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	// Wyczyść bufery
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Aktualizacja macierzy widoku
	camera->updateViewMatrix(60.0f, 0.01f, 50.0f, *sp);

	// Aktywacja programu cieniującego
	sp->use();

	// Wylicz macierz modelu
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.5f));
	M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
	M = glm::rotate(M, angle_y + glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));

	// Przekaż macierz modelu do programu cieniującego
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	// Uaktywnij teksturę
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, defaultTexture);
	//glUniform1i(sp->u("tex0"), 0);

	// Przekaż dane modelu
	vao->Bind();

	// Rysowanie obiektu
	glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, NULL);

	//glDrawArrays(GL_TRIANGLES, 0, 6);  // Narysuj obiekt

	// Przerzuć tylny bufor na przedni
	glfwSwapBuffers(window);
}

int main(void) {
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
	glfwSwapInterval(0);

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
