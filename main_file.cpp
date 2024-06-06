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
#include "player.h"
#include "rocket.h"
#include "bazooka.h"

#define PERSISTANCE 0.5
#define OCTAVES 4
#define FREQUENCY 0.3
#define TERRAIN_SIZE 20
#define TERRAIN_DENSITY 0.1
#define TERRAIN_HEIGHT 1.0 //bigger is flatter

#define ALPHA_ANGLE 89.0
#define BETA_ANGLE 0.0

#define BAZOOKA_PITCH_SPEED PI/8 
#define BAZOOKA_POWER 10.0

#define CAM_COUNTDOWN 2.0


#define VSYNC 0 //1 - włączony, 0 - wyłączony

Camera* camera;

ParticleSystem* explosionParticles;
glm::vec3 explosionPosition;
float explLightStrength = 0.0f;

Rocket* rocket;

Terrain* terrain;

Player* player1;
Player* player2;

Bazooka* bazooka1;
Bazooka* bazooka2;

GLuint texture;

ShaderProgram* sp;
ShaderProgram* sp_light;
ShaderProgram* sp_particle;

float speed_x = 0;
float speed_y = 0;

float cam_switch_countdown = CAM_COUNTDOWN;

float player_rotate_speed = 0;

float bazooka_pitch = 0;
float bazooka_pitch_speed = BAZOOKA_PITCH_SPEED;

bool player1_turn = true;
bool firstPersonCamera = false;
bool shiftPressed = false;
bool controlPressed = false;
bool enteredGameWindow = false;
bool firing = false;
bool fired = false;
bool flying = false;

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
	if (!enteredGameWindow) return;
	static double lastX = 0;
	static double lastY = 0;

	double dx = lastX - x;
	double dy = lastY - y;

	lastX = x;
	lastY = y;

	if (firstPersonCamera) {
		camera->rotate(window, dx, dy);
	}
	else {
		if (flying) {
			camera->rotateAroundPlayer(rocket->position, dx, dy);
		}
		else if (player1_turn) {
			camera->rotateAroundPlayer(player1->position, dx, dy);
		}
		else {
			camera->rotateAroundPlayer(player2->position, dx, dy);
		}
	}
}

void mouseBtnCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		enteredGameWindow = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, camera->width / 2, camera->height / 2);
	}
}

void scrollCallback(GLFWwindow* window, double x, double y) {
	camera->setZoom(y);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = -PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = PI / 2;
		if (key == GLFW_KEY_SPACE) {
			firstPersonCamera = true;
		}
		if (key == GLFW_KEY_W) {
			if (firstPersonCamera) {
				camera->speed.x = PI;
				if (shiftPressed) {
					camera->speed.x *= 3.0f;
					if (controlPressed) {
						camera->speed.x *= 3.0f;
					}
				}
			}
		}
		if (key == GLFW_KEY_S) {
			if (firstPersonCamera) {
				camera->speed.x = -PI;
				if (shiftPressed) {
					camera->speed.x *= 3.0f;
					if (controlPressed) {
						camera->speed.x *= 3.0f;
					}
				}
			}
		}
		if (key == GLFW_KEY_D) {
			if (firstPersonCamera) {
				camera->speed.z = PI;
				if (shiftPressed) {
					camera->speed.z *= 3.0f;
					if (controlPressed) {
						camera->speed.z *= 3.0f;
					}
				}
			}
		}
		if (key == GLFW_KEY_A) {
			if (firstPersonCamera) {
				camera->speed.z = -PI;
				if (shiftPressed) {
					camera->speed.z *= 3.0f;
					if (controlPressed) {
						camera->speed.z *= 3.0f;
					}
				}
			}
		}

		if (key == GLFW_KEY_LEFT_SHIFT) {
			shiftPressed = !shiftPressed;
		}
		if (key == GLFW_KEY_LEFT_CONTROL) {
			controlPressed = !controlPressed;
		}
		if (key == GLFW_KEY_ENTER) {
			firing = true;
			fired = false;
		}
		if (key == GLFW_KEY_K) {
			player_rotate_speed = PI / 8;
		}
		if (key == GLFW_KEY_L) {
			player_rotate_speed = -PI / 8;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_SPACE) {
			firstPersonCamera = false;
		}
		if (key == GLFW_KEY_W) camera->speed.x = 0;
		if (key == GLFW_KEY_S) camera->speed.x = 0;
		if (key == GLFW_KEY_D) camera->speed.z = 0;
		if (key == GLFW_KEY_A) camera->speed.z = 0;
		if (key == GLFW_KEY_ESCAPE) {
			camera->firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			enteredGameWindow = false;
		}
		if (key == GLFW_KEY_ENTER) {
			firing = false;
			fired = true;
		}
		if (key == GLFW_KEY_K) {
			player_rotate_speed = 0;
		}
		if (key == GLFW_KEY_L) {
			player_rotate_speed = 0;
		}

		if (key == GLFW_KEY_P) {
			rocket->spawn(glm::vec3(0.0f, 2.0f, 0.0f), 10.0f, -90.0f, 45.0f);
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

bool readModel(const char* filename, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices) {
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
	srand(time(NULL));

	// Ustawienie cullface
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Callback funkcje
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseBtnCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Wczytanie programu cieniującego upostępnionego przez prowadzącego
	sp = new ShaderProgram("v_terrain_shader.glsl", NULL, "f_terrain_shader.glsl");
	sp_light = new ShaderProgram("v_light_shader.glsl", NULL, "f_light_shader.glsl");
	sp_particle = new ShaderProgram("v_particle_shader.glsl", NULL, "f_particle_shader.glsl");

	// Utworzenie obiektu kamery
	camera = new Camera(800, 600, glm::vec3(0.0f, 8.0f, 0.0f));
	camera->facing = glm::vec3(TERRAIN_SIZE / 2, -16.0f, -TERRAIN_SIZE / 2);

	// Utworzenie particle system
	ParticleInfo info;
	info.position = glm::vec3(0.0f, 1.0f, 0.0f);
	info.size = 0.5f;
	info.sizeEnd = 0.0f;
	info.startColor = glm::vec3(0.85f, 0.8f, 0.01f);
	info.endColor = glm::vec3(1.0f, 0.12f, 0.1f);
	info.ttl = 1.0f;
	info.gravity = glm::vec3(0.0f, -9.0f, 0.0f);
	info.velocity = glm::vec3(0.0f, 3.0f, 0.0f);
	info.velocityVariation = glm::vec3(2.0f, 3.0f, 2.0f);
	explosionParticles = new ParticleSystem(info);
	explosionParticles->emit(100);

	// Utworzenie terenu
	terrain = new Terrain(TERRAIN_SIZE, TERRAIN_DENSITY, TERRAIN_HEIGHT, FREQUENCY, OCTAVES, PERSISTANCE);

	// Wczytanie tekstur
	terrain->texture_base = readTexture("sand.png");

	// Wczytanie pocisku
	std::vector<GLfloat> rocket_vertices;
	std::vector<GLfloat> rocket_normals;
	std::vector<GLfloat> rocket_texCoords;
	std::vector<GLuint> rocket_indices;
	if (!readModel("models/Missile.obj", rocket_vertices, rocket_normals, rocket_texCoords, rocket_indices)) {
		fprintf(stderr, "Nie można wczytać modelu pocisku\n");
	}
	rocket = new Rocket(rocket_vertices, rocket_normals, rocket_texCoords, rocket_indices);
	GLuint rocket_base = readTexture("models/missile_base.png");
	rocket->texture_base = rocket_base;
	rocket->spawn(glm::vec3(0.0f, 2.0f, 0.0f), 10.0f, 0.0f, 0.0f);

	// Wczytanie gracza
	glm::vec2 pos = glm::vec2(TERRAIN_SIZE / 2, -0.1f * TERRAIN_SIZE);
	glm::vec3 spawn = glm::vec3(pos.x, terrain->getHeight(pos.x, pos.y), pos.y);

	std::vector<GLfloat> player_vertices;
	std::vector<GLfloat> player_normals;
	std::vector<GLfloat> player_texCoords;
	std::vector<GLuint> player_indices;
	if (!readModel("models/Mesh_Ape.obj", player_vertices, player_normals, player_texCoords, player_indices)) {
		fprintf(stderr, "Nie można wczytać modelu gracza\n");
	}

	player1 = new Player(spawn, 3, player_vertices, player_normals, player_texCoords, player_indices);
	GLuint player_tex = readTexture("models/Tex_Ape.png");
	player1->texture_base = player_tex;
	pos = glm::vec2(TERRAIN_SIZE / 2, -0.9f * TERRAIN_SIZE);
	spawn = glm::vec3(pos.x, terrain->getHeight(pos.x, pos.y), pos.y);
	player2 = new Player(spawn, 3, player_vertices, player_normals, player_texCoords, player_indices);
	player2->texture_base = player_tex;

	// Wczytanie bazooki
	std::vector<GLfloat> bazooka_vertices;
	std::vector<GLfloat> bazooka_normals;
	std::vector<GLfloat> bazooka_texCoords;
	std::vector<GLuint> bazooka_indices;
	if (!readModel("models/bazooka.obj", bazooka_vertices, bazooka_normals, bazooka_texCoords, bazooka_indices)) {
		fprintf(stderr, "Nie można wczytać modelu bazooki\n");
	}
	bazooka1 = new Bazooka(player1->position, bazooka_vertices, bazooka_normals, bazooka_texCoords, bazooka_indices);
	GLuint bazooka_tex = readTexture("models/bazooka_base.png");
	bazooka1->texture_base = bazooka_tex;
	bazooka2 = new Bazooka(player2->position, bazooka_vertices, bazooka_normals, bazooka_texCoords, bazooka_indices);
	bazooka2->texture_base = bazooka_tex;

	// Ustawienie wiatru
	wind.x = (rand() % 2 ? -1 : 1) * rand() % 30 / 10;
	wind.z = (rand() % 2 ? -1 : 1) * rand() % 30 / 10;

	fprintf(stdout, "Player 1 & 2 hp: %d\n", 3);
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

	// Usunięcie bazooki
	delete bazooka1;
	delete bazooka2;
}

void movePlayer(Player* p, Bazooka* b) {
	//Przesuń gracza
	float x = speed_x * glfwGetTime();
	float y = speed_y * glfwGetTime();
	if (!player1_turn) {
		x *= -1;
		y *= -1;
	}
	if (x != 0 || y != 0) {
		p->rotation = atan2(x, y);
	}
	p->position.x += x;
	p->position.z += y;
	p->position.y = terrain->getHeight(p->position.x, p->position.z);

	p->rotation += player_rotate_speed * glfwGetTime();

	b->position = p->position;
	b->yaw = p->rotation;
}

void playerStuff(Player* p, Bazooka* b) {
	movePlayer(p, b);
	if (enteredGameWindow && !firstPersonCamera) {
		if (flying) camera->followPlayer(rocket->position);
		else camera->followPlayer(p->position);
	}
	if (firing) {
		b->pitch += bazooka_pitch_speed * glfwGetTime();
		if (b->pitch > glm::radians(89.0f)) {
			b->pitch = glm::radians(89.0f);
			bazooka_pitch_speed = -BAZOOKA_PITCH_SPEED;
		}
		if (b->pitch < glm::radians(0.0f)) {
			b->pitch = glm::radians(0.0f);
			bazooka_pitch_speed = BAZOOKA_PITCH_SPEED;
		}
	}
	if (fired) {
		glm::vec3 offset = glm::vec3(0.0f,
			1.0f,
			0.0f);
		rocket->spawn(b->position + offset, BAZOOKA_POWER, glm::degrees(-b->yaw) + 90.0f, glm::degrees(b->pitch));
		fired = false;
		flying = true;
	}
}

void explosionEffect(Rocket* r, ParticleSystem* p) {
	explosionPosition = rocket->position + glm::vec3(0.0f, 1.0f, 0.0f);
	explLightStrength = 1.0f;
	explosionParticles->info.position = rocket->position;
	explosionParticles->emit(100);
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) {
	// Wyczyść bufery
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Aktualizacja macierzy widoku
	camera->updateMatrix(60.0f, 0.01f, 50.0f);

	// Rysowanie terenu
	terrain->draw(*sp, *camera, explosionPosition, explLightStrength);

	// Rysowanie particle system
	explosionParticles->render(*sp_particle, *camera);

	// Rysowanie pocisku
	if (flying)
		rocket->draw(*sp, *camera, explosionPosition, explLightStrength);

	// Rysowanie bazooki
	bazooka1->draw(*sp, *camera, explosionPosition, explLightStrength);
	bazooka2->draw(*sp, *camera, explosionPosition, explLightStrength);

	// Rysowanie gracza
	player1->draw(*sp, *camera, explosionPosition, explLightStrength);
	player2->draw(*sp, *camera, explosionPosition, explLightStrength);

	// Przerzuć tylny bufor na przedni
	glfwSwapBuffers(window);
}

void printFPS(GLFWwindow* window) {
	static float avg_fps = 0;
	static float time = 0;
	static float frames = 0;

	frames++;
	time += glfwGetTime();

	if (time >= 1.0 / 4.0) {
		avg_fps = (avg_fps + frames / time) / 2;
		glfwSetWindowTitle(window, ("Maggots3D - FPS: " + std::to_string(avg_fps)).c_str());
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

	// Czekaj na zamknięcie konsoli
	while (true)
	{

	}

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
		printFPS(window);

		// Przesuń kamerę
		camera->move(glfwGetTime());

		// Aktualizuj particle system
		explosionParticles->update(glfwGetTime());

		// Aktualizuj pocisk
		if (flying) {
			if (cam_switch_countdown < CAM_COUNTDOWN) {
				if (cam_switch_countdown <= 0.0) {
					fprintf(stdout, "Player %d turn\n", player1_turn ? 1 : 2);
					player1_turn = !player1_turn;
					flying = false;
					cam_switch_countdown = CAM_COUNTDOWN;
				}
				else {
					cam_switch_countdown -= glfwGetTime();
				}
			}
			else {
				rocket->updatePosition(glfwGetTime());

				if (rocket->collisionHappened(*terrain)) {
					explosionEffect(rocket, explosionParticles);
					cam_switch_countdown -= glfwGetTime();
				}
				if (rocket->collisionHappened(*player1)) {
					explosionEffect(rocket, explosionParticles);
					cam_switch_countdown -= glfwGetTime();
					player1->hp -= 1;
					fprintf(stdout, "Player 1 hp: %d\n", player1->hp);
					if (player1->hp == 0) {
						fprintf(stdout, "Player 2 wins\n");
						break;
					}
				}
				if (rocket->collisionHappened(*player2)) {
					explosionEffect(rocket, explosionParticles);
					cam_switch_countdown -= glfwGetTime();
					player2->hp -= 1;
					fprintf(stdout, "Player 2 hp: %d\n", player2->hp);
					if (player2->hp == 0) {
						fprintf(stdout, "Player 1 wins\n");
						break;
					}
				}
			}
		}

		// Aktualizuj moc eksplozji
		if (explLightStrength > 0) {
			explLightStrength -= 1.0f * glfwGetTime();
		}
		if (explLightStrength < 0)
		{
			explLightStrength = 0.0f;
		}

		// Aktualizuj gracza
		if (player1_turn) {
			playerStuff(player1, bazooka1);
		}
		else {
			playerStuff(player2, bazooka2);
		}

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
