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

Camera* camera;

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

ShaderProgram* sp;

// Odkomentuj, żeby rysować kostkę
float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;

GLuint tex0;
GLuint tex1;

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
    aspectRatio = (float)width / (float)height;
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

    return tex;
}

// Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0.2, 0.46, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    sp = new ShaderProgram("v_shader.glsl", NULL, "f_shader.glsl");
    // tex0 = readTexture("metal.png");

	// Utworzenie obiektu kamery
	camera = new Camera(800, 600, glm::vec3(0.0f, 0.0f, 4.0f));
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;

	// Usunięcie obiektu kamery
	delete camera;
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
    // Wyczyść bufery
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->updateViewMatrix(90.0f, 0.01f, 50.0f, *sp);

    //glm::mat4 V = glm::lookAt(
    //    glm::vec3(0, 0, -4),
    //    glm::vec3(0, 0, 0),
    //    glm::vec3(0.0f, 1.0f, 0.0f));  // Wylicz macierz widoku

    //glm::mat4 P = glm::perspective(90.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);  // Wylicz macierz rzutowania

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));  // Wylicz macierz modelu
    M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));  // Wylicz macierz modelu

    sp->use();  // Aktywacja programu cieniującego
    // Przeslij parametry programu cieniującego do karty graficznej
    /*glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));*/
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));                               // Włącz przesyłanie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);  // Wskaż tablicę z danymi dla atrybutu vertex

    glEnableVertexAttribArray(sp->a("color"));                             // Włącz przesyłanie danych do atrybutu color
    glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);  // Wskaż tablicę z danymi dla atrybutu color

    glEnableVertexAttribArray(sp->a("normal"));                              // Włącz przesyłanie danych do atrybutu normal
    glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);  // Wskaż tablicę z danymi dla atrybutu normal

    // glEnableVertexAttribArray(sp->a("texCoord0"));                                // Włącz przesyłanie danych do atrybutu texCoord
    // glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);  // Wskaż tablicę z danymi dla atrybutu texCoord

    // glUniform1i(sp->u("textureMap0"), 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, tex0);

    // glUniform1i(sp->u("textureMap1"), 1);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);  // Narysuj obiekt

    glDisableVertexAttribArray(sp->a("vertex"));  // Wyłącz przesyłanie danych do atrybutu vertex
    glDisableVertexAttribArray(sp->a("color"));   // Wyłącz przesyłanie danych do atrybutu color
    glDisableVertexAttribArray(sp->a("normal"));  // Wyłącz przesyłanie danych do atrybutu normal
    // glDisableVertexAttribArray(sp->a("texCoord0"));  // Wyłącz przesyłanie danych do atrybutu texCoord0

    glfwSwapBuffers(window);  // Przerzuć tylny bufor na przedni
}

int main(void) {
    GLFWwindow* window;  // Wskaźnik na obiekt reprezentujący okno

    glfwSetErrorCallback(error_callback);  // Zarejestruj procedurę obsługi błędów

    if (!glfwInit()) {  // Zainicjuj bibliotekę GLFW
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(800, 600, "Maggots3D", NULL, NULL);  // Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

    if (!window)  // Jeżeli okna nie udało się utworzyć, to zamknij program
    {
        fprintf(stderr, "Nie można utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);  // Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
    glfwSwapInterval(1);             // Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    if (glewInit() != GLEW_OK) {  // Zainicjuj bibliotekę GLEW
        fprintf(stderr, "Nie można zainicjować GLEW.\n");
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);  // Operacje inicjujące

    // Główna pętla
    float angle_x = 0;                      // Aktualny kąt obrotu obiektu
    float angle_y = 0;                      // Aktualny kąt obrotu obiektu
    glfwSetTime(0);                         // Zeruj timer
    while (!glfwWindowShouldClose(window))  // Tak długo jak okno nie powinno zostać zamknięte
    {
        angle_x += speed_x * glfwGetTime();   // Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y += speed_y * glfwGetTime();   // Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		
		camera->move(glfwGetTime());

        // Zeruj timer
        glfwSetTime(0);

        // Wykonaj procedurę rysującą
        drawScene(window, angle_x, angle_y);  

        // Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
        glfwPollEvents();
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window);  // Usuń kontekst OpenGL i okno
    glfwTerminate();            // Zwolnij zasoby zajęte przez GLFW
    exit(EXIT_SUCCESS);
}
