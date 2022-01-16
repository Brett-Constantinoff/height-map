#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class Camera{
    public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw = -90.0f;
    float pitch = 0.0f;

    void move(GLFWwindow **win, float *deltaTime, float *lastFrame);

};


#endif