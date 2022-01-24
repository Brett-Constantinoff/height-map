#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "../stb/stb_image.h"

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

    //loads height map
    stbi_set_flip_vertically_on_load(true);
    int w, h, num_channels;
    unsigned char *data = stbi_load("../resources/heightmap.png", &w, &h, &num_channels, 0);
    if (data){
        std::cout << "Loaded heightmap of size " << h << " x " << w << std::endl;
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }

    //sets up vertex position
    std::vector<float> vertex_positions;
    float y_scale = 64.0f / 256.0f, y_shift = 16.0f;
    int rez = 1;
    unsigned byte_per_pixel = num_channels;
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            unsigned char* pixel_offset = data + (j + w * i) * byte_per_pixel;
            unsigned char y = pixel_offset[0];

            // vertex
            vertex_positions.push_back( -h/2.0f + h*i/(float)h );   // vx
            vertex_positions.push_back( (int) y * y_scale - y_shift);   // vy
            vertex_positions.push_back( -w/2.0f + w*j/(float)w );   // vz
        }
    }
    std::cout << "Loaded " << vertex_positions.size() / 3 << " vertex_positions" << std::endl;
    stbi_image_free(data);

    //sets up indices
    std::vector<unsigned> indices;
    for(unsigned i = 0; i < h - 1; i += rez){
        for (unsigned j = 0; j < w; j += rez){
            for (unsigned k = 0; k < 2; k++){
                indices.push_back(j + w * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;
    
    const int num_strips = (h-1)/rez;
    const int tri_per_strip = (w/rez)*2-2;
    std::cout << "Created lattice of " << num_strips << " strips with " << tri_per_strip << " triangles each" << std::endl;
    std::cout << "Created " << num_strips * tri_per_strip << " triangles total" << std::endl;

    unsigned int vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(float), &vertex_positions[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    Camera my_camera;
    my_camera.position = glm::vec3(0.34f, 30.0f, 11.39f);
    my_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    my_camera.front = glm::vec3(-0.03f, -0.53f, -0.85f);

    glm::vec3 diffuse_color = glm::vec3(0.40f, 0.47f, 0.70f);
    glm::vec3 ambient_color = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 specular_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 light_position = glm::vec3(0.0f, 50.0f, 5.0f);

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
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 10000.0f);
        glm::mat4 view = glm::lookAt(my_camera.position, my_camera.position + my_camera.front, my_camera.up);
        glm::mat4 normal_matrix = glm::mat4();

        normal_matrix = glm::transpose(model);
        normal_matrix = glm::inverse(normal_matrix);

        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "normal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);
        glUniform3fv(glGetUniformLocation(my_shader.ID, "light_position"), 1, &light_position[0]);
        glUniform3fv(glGetUniformLocation(my_shader.ID, "light_color"), 1, &light_color[0]);
        glUniform3fv(glGetUniformLocation(my_shader.ID, "diffuseVal"), 1, &diffuse_color[0]);
        glUniform3fv(glGetUniformLocation(my_shader.ID, "ambientVal"), 1, &ambient_color[0]);
        glUniform3fv(glGetUniformLocation(my_shader.ID, "specularVal"), 1, &specular_color[0]);

        ImGui::Render();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(unsigned strip = 0; strip < num_strips; strip++){
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                           tri_per_strip+2,   // number of indices to render
                           GL_UNSIGNED_INT,     // index data type
                           (void*)(sizeof(unsigned) * (tri_per_strip+2) * strip)); // offset to starting index
        }
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

