#define GL_SILENCE_DEPRECATION

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

#include "../shaders/shader.h"
#include "../camera/camera.h"

int main(){
    /* INIT GLFW */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* WINDOW INIT */
    const unsigned int width = 800, height = 600;
    GLFWwindow *win = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if(!win){
        std::cerr << "Error creating OpenGL window" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);
    glViewport(0, 0, width, height);

    /* GLEW INIT */
    glewExperimental = true;
    if(glewInit() != GLEW_OK){
        std::cerr << "ERROR INITIALIZING GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    Shader my_shader = Shader("../shaders/shader.shader");

    float vertex_positions[] = {
        0.0f, 0.0f, 0.5f, 
        0.5f, 0.0f, 0.5f, 
        0.0f, 0.0f, 0.0f, 

        0.5f, 0.0f, 0.5f,
        0.5f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f,
    };

    float vertex_colors[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Camera myCamera;
    myCamera.front = glm::vec3(0.0f, 0.0f, -2.0f);
    myCamera.position = glm::vec3(0.0f, 0.0f, 3.0f);
    myCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    /* MAIN RENDER LOOP */
    while(!glfwWindowShouldClose(win)){

        myCamera.move(&win, &deltaTime, &lastFrame);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(win); //swap front and back buffer
        glfwPollEvents();
    }

    return 0;



}