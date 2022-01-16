#include "camera.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Camera::move(GLFWwindow **win, float *deltaTime, float *lastFrame){

    float currentFrame = glfwGetTime();
    *deltaTime = currentFrame - *lastFrame;
    *lastFrame = currentFrame;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    const float cameraSpeed = 3.0f * *deltaTime;

    if(glfwGetKey(*win, GLFW_KEY_W) == GLFW_PRESS){
        position += cameraSpeed * front;
    }
    if(glfwGetKey(*win, GLFW_KEY_S) == GLFW_PRESS){
        position -= cameraSpeed * front;
    }
    if(glfwGetKey(*win, GLFW_KEY_A) == GLFW_PRESS){
        position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    }
    if(glfwGetKey(*win, GLFW_KEY_D) == GLFW_PRESS){
        position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
    }
    if(glfwGetKey(*win, GLFW_KEY_RIGHT) == GLFW_PRESS){
         yaw += 2.0f;
    }
    if(glfwGetKey(*win, GLFW_KEY_LEFT) == GLFW_PRESS){
         yaw -= 2.0f;
    }
    if(glfwGetKey(*win, GLFW_KEY_UP) == GLFW_PRESS){
         pitch += 2.0f;
    }
    if(glfwGetKey(*win, GLFW_KEY_DOWN) == GLFW_PRESS){
         pitch -= 2.0f;
    }

}