#include "Camera.h"
#include <string>


Camera::Camera()
{
    // Set up view matrix
    m_MoveSpeed = 3;
    m_Position = glm::vec3(0, 0, 5);
    m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);

    // Construct projection matrix 100px x 100px as default.
    UpdateProjectionMatrix(100, 100);
}

Camera::Camera(int screenWidth, int screenHeight)
{
    // Set up view matrix
    m_MoveSpeed = 3;
    m_Position = glm::vec3(0, 0, 5);
    m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    // Set up projection matrix
    UpdateViewMatrix(m_Position, m_Front, m_Up);
    UpdateProjectionMatrix(screenWidth, screenHeight);
}

void Camera::UpdateProjectionMatrix(int screenWidth, int screenHeight)
{
    float fovyRad = (float)(90.0 / 180.0) * M_PI;
    m_ProjectionMatrix = glm::perspective(fovyRad, screenWidth / (float)screenHeight, 0.1f, 200.0f);
}

void Camera::UpdateViewMatrix(glm::vec3 position, glm::vec3 lookingPoint, glm::vec3 up)
{
    m_Position = position;
    m_Front = lookingPoint;
    m_Up = up;
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, float deltaTime)
{
    xoffset *= m_MoveSpeed * deltaTime;
    yoffset *= m_MoveSpeed * deltaTime;

    yaw -= xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    //glm::vec3 right = glm::vec3(1, 0, 0);

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    m_Front = glm::normalize(front);

    // Calculate the new camera position
    float radius = glm::length(m_Position); // Assuming m_Position is initially set to the distance from the origin
    m_Position.x = radius * front.x;
    m_Position.y = radius * front.y;
    m_Position.z = radius * front.z;

    // Update camera right and up vectors
    glm::vec3 right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_Up = glm::normalize(glm::cross(right, m_Front));

    // Update the view matrix with the new position and direction
    m_ViewMatrix = glm::lookAt(m_Position, glm::vec3(0.0f), m_Up);
    
}

//void Camera::processKeyboard(GLFWwindow* window, float deltaTime)
//{
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//    {
//        glm::vec3 direction = m_Front;
//        m_Position += m_MoveSpeed * deltaTime * direction ;
//        UpdateViewMatrix();
//    }
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//    {
//        glm::vec3 direction = glm::cross(m_Front, m_Up);
//        m_Position -= m_MoveSpeed * deltaTime * direction;
//        UpdateViewMatrix();
//
//    }
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//    {
//        glm::vec3 direction = m_Front;
//        m_Position -= m_MoveSpeed * deltaTime * direction;
//        UpdateViewMatrix();;
//    }
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//    {
//        glm::vec3 direction = glm::cross(m_Front, m_Up);
//        m_Position += m_MoveSpeed * deltaTime * direction;
//        UpdateViewMatrix();
//    }
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 * m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        UpdateViewMatrix();
//    }
//
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 *m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -theta, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        UpdateViewMatrix();
//    }
//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 * m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        m_Up = glm::vec3(rotationMatrix * glm::vec4(m_Up, 1.0f));
//        UpdateViewMatrix();
//    }
//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 * m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        m_Up = glm::vec3(rotationMatrix * glm::vec4(m_Up, 1.0f));
//        UpdateViewMatrix();
//    }
//    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 * m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        m_Up = glm::vec3(rotationMatrix * glm::vec4(m_Up, 1.0f));
//        UpdateViewMatrix();
//    }
//    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
//    {
//        float theta = glm::radians(35 * m_MoveSpeed * deltaTime);
//        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
//        m_Front = glm::vec3(rotationMatrix * glm::vec4(m_Front, 1.0f)); // Update the direction vector
//        m_Up = glm::vec3(rotationMatrix * glm::vec4(m_Up, 1.0f));
//        UpdateViewMatrix();
//    }
//}
