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

    /*IMGUI INIT */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io; //sets up input / output
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 150");

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

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int color_buffer;
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    Camera my_camera;
    my_camera.position = glm::vec3(0.34f, 3.85f, 11.39f);
    my_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    my_camera.front = glm::vec3(-0.03f, -0.53f, -0.85f);

    bool main_window = true;
    bool camera_window = false;

    /* MAIN RENDER LOOP */
    while(!glfwWindowShouldClose(win)){

        my_camera.move(&win, &delta_time, &last_frame);

        //setup imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //main demo window
        {   
            ImGui::Begin("Hello, world!");   
            ImGui::Checkbox("Camera Details", &camera_window);                      
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        if(camera_window){
            ImGui::Text("Camera position: %.4f, %.4f, %.4f", my_camera.position.x, my_camera.position.y, my_camera.position.z);
            ImGui::Text("Camera up: %.4f, %.4f, %.4f", my_camera.up.x, my_camera.up.y, my_camera.up.z);
            ImGui::Text("Camera front: %.4f, %.4f, %.4f", my_camera.front.x, my_camera.front.y, my_camera.front.z);
        }

    
        glUseProgram(my_shader.ID);
        glBindVertexArray(vao);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(my_camera.position, my_camera.position + my_camera.front, my_camera.up);

        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);

        ImGui::Render();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertex_positions) / 3);   
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(win); //swap front and back buffer
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;



}