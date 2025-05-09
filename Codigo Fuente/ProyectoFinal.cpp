// Inclusión de bibliotecas estándar para entrada/salida y matemáticas
#include <iostream>
#include <cmath>

// Bibliotecas para OpenGL: GLEW para extensiones, GLFW para ventanas y eventos
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Biblioteca para cargar texturas (imágenes)
#include "stb_image.h"

// Biblioteca para cargar modelos con texturas (SOIL2)
#include "SOIL2/SOIL2.h"

// GLM para operaciones matemáticas con vectores y matrices (transformaciones 3D)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Archivos personalizados para manejar shaders, cámara y modelos 3D
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Prototipos de funciones para manejar entrada de teclado, ratón y movimiento
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Dimensiones iniciales de la ventana
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT; // Dimensiones reales del framebuffer

// Objeto cámara inicializado en (0, 0, 3) para una vista inicial
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Variables para seguimiento del ratón
GLfloat lastX = WIDTH / 4.0, lastY = HEIGHT / 4.0;
bool keys[1024]; // Arreglo para almacenar estado de teclas
bool firstMouse = true; // Evita saltos iniciales del ratón

// Posición de la luz (inicialmente en el origen)
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active; // Estado de activación de la luz

// Posición del personaje y offset de la cámara para vista en tercera persona
glm::vec3 playerPosition = glm::vec3(0.0f, 0.8f, 0.0f);
glm::vec3 cameraOffset = glm::vec3(0.0f, 4.0f, 12.0f);

// Posiciones de las luces puntuales (inicialmente en el origen)
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f)
};

// Vértices de un cubo para representar las fuentes de luz (posición + normal)
float vertices[] = {
    // Cara trasera
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    // Cara frontal
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    // Cara izquierda
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // Cara derecha
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     // Cara inferior
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     // Cara superior
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// Color inicial de la luz (apagada)
glm::vec3 Light1 = glm::vec3(0);

// Variables para calcular el tiempo entre fotogramas (movimiento suave)
GLfloat deltaTime = 0.0f; // Tiempo entre el fotograma actual y el anterior
GLfloat lastFrame = 0.0f; // Tiempo del último fotograma

// Función principal
int main() {
    // Inicializa GLFW para gestionar ventanas y eventos
    glfwInit();

    // Crea una ventana de 800x600 píxeles con el título "Fuentes de luz"
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fuentes de luz", nullptr, nullptr);
    if (nullptr == window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE; // Termina si no se pudo crear la ventana
    }

    // Establece la ventana como el contexto actual de OpenGL
    glfwMakeContextCurrent(window);

    // Obtiene las dimensiones reales del framebuffer
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Asocia funciones de callback para teclado y ratón
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // Configura GLEW para usar un enfoque moderno en extensiones
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE; // Termina si no se pudo inicializar GLEW
    }

    // Define el área de renderizado (viewport) según las dimensiones de la ventana
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Carga los shaders para objetos iluminados y fuentes de luz
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // Carga los modelos 3D (casa y personaje)
    Model Dog((char*)"Models/casafinal.obj"); // Modelo de la casa
    Model personaje((char*)"Models/snoopy.obj"); // Modelo del personaje

    // Configura los buffers para los vértices del cubo (usado para luces)
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Atributo de posición (3 componentes: x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Atributo de normal (3 componentes para iluminación)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configura las unidades de textura en el shader de iluminación
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

    // Define la matriz de proyección (perspectiva)
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Bucle principal del juego
    while (!glfwWindowShouldClose(window)) {
        // Calcula el tiempo entre fotogramas para movimiento suave
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Procesa eventos de teclado y ratón
        glfwPollEvents();
        DoMovement(); // Actualiza posiciones del personaje y luces

        // Limpia los buffers de color y profundidad con un fondo gris oscuro
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activa pruebas de profundidad y mezcla para transparencias
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Usa el shader de iluminación para los objetos
        lightingShader.Use();

        // Configura texturas difusa y especular
        glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"), 1);

        // Pasa la posición de la cámara al shader
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Configura la luz direccional (como un sol)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);

        // Configura la primera luz puntual (con color dinámico)
        pointLightPositions[0] = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 lightColor;
        lightColor.x = abs(sin(glfwGetTime() * Light1.x)); // Color pulsante
        lightColor.y = abs(sin(glfwGetTime() * Light1.y));
        lightColor.z = sin(glfwGetTime() * Light1.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

        // Configura las otras luces puntuales (desactivadas)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0f);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0f);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);

        // Configura la luz de foco (spotlight) que sigue la cámara
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

        // Configura la propiedad de brillo del material
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

        // Crea la matriz de vista para la cámara en tercera persona
        glm::mat4 view;
        glm::vec3 newCamPos = playerPosition + cameraOffset; // Posición de la cámara relativa al personaje
        view = glm::lookAt(newCamPos, playerPosition, glm::vec3(0.0f, 1.0f, 0.0f)); // Mira al personaje

        // Obtiene las ubicaciones de las matrices en el shader
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        // Pasa las matrices al shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Dibuja la casa
        glm::mat4 model(1);
        glEnable(GL_BLEND); // Activa la transparencia
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Dog.Draw(lightingShader); // Renderiza el modelo de la casa

        // Dibuja el personaje
        model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition); // Posiciona el personaje
        model = glm::scale(model, glm::vec3(0.7f)); // Escala el modelo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        personaje.Draw(lightingShader); // Renderiza el modelo del personaje

        glDisable(GL_BLEND); // Desactiva la transparencia
        glBindVertexArray(0);

        // Usa el shader para las fuentes de luz
        lampShader.Use();
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");

        // Pasa las matrices al shader de la lámpara
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Dibuja las fuentes de luz como cubos pequeños
        for (GLuint i = 0; i < 4; i++) {
            model = glm::mat4(1);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Cubo pequeño
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36); // Dibuja el cubo
        }
        glBindVertexArray(0);

        // Intercambia los buffers para mostrar el fotograma renderizado
        glfwSwapBuffers(window);
    }

    // Libera los recursos de GLFW y termina el programa
    glfwTerminate();
    return 0;
}

// Función para manejar el movimiento del personaje y la luz
void DoMovement() {
    float speed = 20.0f * deltaTime; // Velocidad ajustada al tiempo

    // Movimiento del personaje con teclas W, S, A, D o flechas
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_UP])
        playerPosition.z -= speed; // Mueve hacia adelante
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_DOWN])
        playerPosition.z += speed; // Mueve hacia atrás
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_LEFT])
        playerPosition.x -= speed; // Mueve a la izquierda
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_RIGHT])
        playerPosition.x += speed; // Mueve a la derecha

    // Movimiento manual de la primera luz puntual
    if (keys[GLFW_KEY_T])
        pointLightPositions[0].x += 0.01f; // Derecha
    if (keys[GLFW_KEY_G])
        pointLightPositions[0].x -= 0.01f; // Izquierda
    if (keys[GLFW_KEY_F])
        pointLightPositions[0].y += 0.01f; // Arriba
    if (keys[GLFW_KEY_H])
        pointLightPositions[0].y -= 0.01f; // Abajo
    if (keys[GLFW_KEY_U])
        pointLightPositions[0].z -= 0.1f; // Adelante
    if (keys[GLFW_KEY_J])
        pointLightPositions[0].z += 0.01f; // Atrás
}

// Función para manejar eventos de teclado
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // Cierra la ventana al presionar ESC
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // Actualiza el estado de las teclas (presionada o liberada)
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }

    // Alterna el estado de la luz al presionar ESPACIO
    if (keys[GLFW_KEY_SPACE]) {
        active = !active;
        if (active) {
            Light1 = glm::vec3(1.0f, 1.0f, 0.0f); // Luz amarilla
        }
        else {
            Light1 = glm::vec3(0); // Luz apagada
        }
    }
}

// Función para manejar el movimiento del ratón (control de cámara)
void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    // Evita saltos iniciales del ratón
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    // Calcula el desplazamiento del ratón
    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // Invertido para movimiento natural
    lastX = xPos;
    lastY = yPos;

    // Ajusta sensibilidad del ratón
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    // Ángulos de rotación para la cámara (yaw y pitch)
    static float yaw = -90.0f;
    static float pitch = 10.0f;
    yaw += xOffset;
    pitch += yOffset;

    // Limita el ángulo de pitch para evitar volteretas
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < 5.0f) pitch = 5.0f; // Evita cámara demasiado baja

    // Calcula la posición de la cámara en coordenadas esféricas
    float radius = 6.0f; // Distancia de la cámara al personaje
    float camX = radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    float camY = radius * sin(glm::radians(pitch));
    float camZ = radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    // Asegura que la cámara no baje demasiado
    if ((playerPosition.y + camY) < 1.0f)
        camY = 1.0f - playerPosition.y;

    // Actualiza el offset de la cámara
    cameraOffset = glm::vec3(camX, camY, camZ);
}