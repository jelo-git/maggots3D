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

#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"

#include "camera.h"
#include "VAO.h"
#include "EBO.h"
#include "particle.h"
#include "terrain.h"

#include "tiny_obj_loader.h"
#include "hitbox.h"
#include "player.h"
#include "rocket.h"

#define PERSISTANCE 0.5
#define OCTAVES 4
#define FREQUENCY 0.3
#define TERRAIN_SIZE 10
#define TERRAIN_DENSITY 1.0
#define TERRAIN_HEIGHT 1.0 //bigger is flatter

#define ALPHA_ANGLE 89.0
#define BETA_ANGLE 0.0


#define VSYNC 0 //1 - włączony, 0 - wyłączony

Camera* camera;

ParticleSystem* explosionParticles;

Rocket* rocket;

Terrain* terrain;

Player* player1;
Player* player2;

GLuint texture;

ShaderProgram* sp;
ShaderProgram* sp_light;
ShaderProgram* sp_particle;

float speed_x = 0;
float speed_y = 0;

// Plaszczyzna 2D
std::vector<GLfloat> vertices = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f
};
std::vector<GLfloat> normals = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};
std::vector<GLfloat> texCoords = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};
std::vector<GLuint> indices = {
	0, 1, 2,
	2, 3, 0
};

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
		if (key == GLFW_KEY_UP) speed_y = -PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = PI / 2;
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
		if (key == GLFW_KEY_P) {
			explosionParticles->emit(100);
		}
		if (key == GLFW_KEY_O) {
			rocket->position = glm::vec3(5.0f, 5.0f, -5.0f);
			rocket->initial_position = rocket->position;
			rocket->setVelocity(0.0f);
			rocket->setVerticalAngle(0.0f);
			rocket->setHorizontalAngle(0.0f);
		}
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

	if (error) {
		fprintf(stderr, "Nie można wczytać tekstury %s: %s\n", filename, lodepng_error_text(error));
		return 0;
	}
	else {
		fprintf(stdout, "Wczytano teksture %s, rozmiaru: %d x %d\n", filename, width, height);
	}

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Callback funkcje
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	// Wczytanie programu cieniującego upostępnionego przez prowadzącego
	sp = new ShaderProgram("v_terrain_shader.glsl", NULL, "f_terrain_shader.glsl");
	sp_light = new ShaderProgram("v_light_shader.glsl", NULL, "f_light_shader.glsl");
	sp_particle = new ShaderProgram("v_particle_shader.glsl", NULL, "f_particle_shader.glsl");

	// Utworzenie obiektu kamery
	camera = new Camera(800, 600, glm::vec3(5.0f, 2.0f, 0.0f));

	// Utworzenie particle system
	ParticleInfo info;
	info.position = glm::vec3(0.0f, 1.0f, 0.0f);
	info.sizeEnd = 1.0f;
	info.ttl = 10.0f;
	info.gravity = glm::vec3(0.0f, -9.0f, 0.0f);
	info.velocity = glm::vec3(0.0f, 2.0f, 0.0f);
	info.velocityVariation = glm::vec3(1.0f, 1.0f, 1.0f);
	explosionParticles = new ParticleSystem(info);
	explosionParticles->emit(100);

	// Utworzenie terenu
	terrain = new Terrain(TERRAIN_SIZE, TERRAIN_DENSITY, TERRAIN_HEIGHT, FREQUENCY, OCTAVES, PERSISTANCE);

	// Wczytanie tekstur
	terrain->texture_base = readTexture("sand.png");
	texture = readTexture("cate.png");

	// Wczytanie pocisku
	rocket = new Rocket(5.0f, 5.0f, -5.0f, vertices, indices);
	rocket->texture_base = texture;
	rocket->setVelocity(0.0f);
	rocket->setVerticalAngle(0.0f);
	rocket->setHorizontalAngle(0.0f);

	// Wczytanie gracza
	glm::vec2 pos = glm::vec2(TERRAIN_SIZE / 2, -0.1f * TERRAIN_SIZE);
	glm::vec3 spawn = glm::vec3(pos.x, terrain->getHeight(pos.x, pos.y), pos.y);
	player1 = new Player(spawn, 3, vertices, indices);
	pos = glm::vec2(TERRAIN_SIZE / 2, -0.9f * TERRAIN_SIZE);
	spawn = glm::vec3(pos.x, terrain->getHeight(pos.x, pos.y), pos.y);
	player2 = new Player(spawn, 3, vertices, indices);
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	// Usunięcie programu cieniującego
	delete sp;
	delete sp_light;
	delete sp_particle;

	// Usunięcie obiektu kamery
	delete camera;

	// Usunięcie particle system
	delete explosionParticles;

	// Usunięcie pocisku
	delete rocket;

	// Usunięcie terenu
	delete terrain;

	// Usunięcie gracza
	delete player1;
	delete player2;
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) {
	// Wyczyść bufery
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Aktualizacja macierzy widoku
	camera->updateMatrix(60.0f, 0.01f, 50.0f);

	// Rysowanie terenu
	terrain->draw(*sp, *camera);

	// Rysowanie particle system
	explosionParticles->render(*sp_particle, *camera);

	// Rysowanie pocisku
	rocket->draw(*sp_particle, *camera);
	fprintf(stdout, "Rocket position: %f %f %f\n", rocket->position.x, rocket->position.y, rocket->position.z);

	// Rysowanie gracza
	//sp->use();
	//camera->shaderMatrix(*sp);
	//glm::mat4 M = glm::mat4(1.0f);
	//float height = terrain->getHeight(playerPosition.x, playerPosition.y);
	//M = glm::translate(M, glm::vec3(playerPosition.x, height, playerPosition.y));
	//glUniformMatrix4fv(sp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
	//glUniform1i(sp->u("textureBase"), 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//player_VAO->Bind();
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	//player_VAO->Unbind();

	// Przerzuć tylny bufor na przedni
	glfwSwapBuffers(window);
}

void printFPS() {
	static float avg_fps = 0;
	static float time = 0;
	static float frames = 0;

	frames++;
	time += glfwGetTime();

	if (time >= 1.0 / 4.0) {
		avg_fps = (avg_fps + frames / time) / 2;
		float ms = time / frames * 1000.0;
		fprintf(stdout, "AVG FPS: %f, ms: %f\n", avg_fps, ms);
		time = 0;
		frames = 0;
	}
}

GLFWwindow* inits() {
	// Zarejestruj procedurę obsługi błędów
	glfwSetErrorCallback(error_callback);

	// Zainicjuj bibliotekę GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	// Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	GLFWwindow* window = glfwCreateWindow(800, 600, "Maggots3D", NULL, NULL);

	// Jeżeli okna nie udało się utworzyć, to zamknij program
	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwMakeContextCurrent(window);

	// Czekaj na x powrót plamki przed pokazaniem ukrytego bufora
	glfwSwapInterval(VSYNC);

	// Zainicjuj bibliotekę GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	// Operacje inicjujące
	initOpenGLProgram(window);

	return window;
}
void terminates(GLFWwindow* window) {
	// Zwolnij zasoby zajęte przez program
	freeOpenGLProgram(window);

	// Usuń kontekst OpenGL i okno
	glfwDestroyWindow(window);

	// Zwolnij zasoby zajęte przez GLFW
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

int main(void) {
	// Inicjalizacja
	GLFWwindow* window = inits();

	// Główna pętla
	glfwSetTime(0);

	while (!glfwWindowShouldClose(window))
	{
		// Oblicz liczbę klatek na sekundę
		printFPS();

		// Przesuń kamerę
		camera->move(glfwGetTime());

		// Aktualizuj particle system
		explosionParticles->update(glfwGetTime());

		// Aktualizuj pocisk
		/*rocket->position = rocket->getMovementCoords(glfwGetTime());
		if (rocket->collisionHappened(*player, *terrain)) {
			player->updateHP();
			explosionParticles->info.position = rocket->position;
			explosionParticles->emit(100);
		}*/

		//Przesuń gracza
		player1->position.x += speed_x * glfwGetTime();
		player1->position.y += speed_y * glfwGetTime();
		player1->position.z = terrain->getHeight(player1->position.x, player1->position.z);

		// Zeruj timer
		glfwSetTime(0);

		// Wykonaj procedurę rysującą
		drawScene(window);

		// Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
		glfwPollEvents();
	}

	// Zwolnij zasoby zajęte przez program i zakoncz działanie
	terminates(window);
}
