///////////////////////////////////////////////////////////////////////////////
// viewmanager.cpp
// ============
// manage the viewing of 3D objects within the viewport - camera, projection
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
    ViewManager* g_pCurrentViewManager = nullptr;
}

// Constructor
ViewManager::ViewManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_pWindow = nullptr;

    // CAMERA SETUP 
    cameraPosition = glm::vec3(0.0f, 2.0f, 6.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    cameraSpeed = 0.05f;

    yaw = -90.0f;
    pitch = 0.0f;

    perspectiveView = true;

    lastX = 400.0f;
    lastY = 300.0f;
    firstMouse = true;

    g_pCurrentViewManager = this;
}

// Destructor
ViewManager::~ViewManager()
{
    if (g_pCurrentViewManager == this)
    {
        g_pCurrentViewManager = nullptr;
    }
}

// CREATE WINDOW (FIXED RETURN TYPE)
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    glfwInit();

    m_pWindow = glfwCreateWindow(800, 600, windowTitle, NULL, NULL);
    if (m_pWindow == nullptr)
    {
        return nullptr;
    }

    glfwMakeContextCurrent(m_pWindow);

    // Register mouse movement and scroll callbacks so Milestone 3 input works
    glfwSetCursorPosCallback(m_pWindow, ViewManager::Mouse_Position_Callback);
    glfwSetScrollCallback(m_pWindow, ViewManager::Mouse_Scroll_Callback);

    // Keep cursor visible for easier testing
    // glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return m_pWindow;
}

// PREPARE VIEW
void ViewManager::PrepareSceneView()
{
    glEnable(GL_DEPTH_TEST);

    // Poll live keyboard input every frame so WASD / QE / P / O work
    ProcessKeyboardEvents();

    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    glm::mat4 projection;

    if (perspectiveView)
    {
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    }
    else
    {
        projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    }

    if (m_pShaderManager != nullptr)
    {
        m_pShaderManager->setMat4Value("view", view);
        m_pShaderManager->setMat4Value("projection", projection);

        // Also send camera position for lighting if the shader uses it
        m_pShaderManager->setVec3Value("viewPosition", cameraPosition);
    }
}

// KEYBOARD INPUT 
void ViewManager::ProcessKeyboard(bool* keys)
{
    if (keys == nullptr)
    {
        return;
    }

    if (keys[GLFW_KEY_W])
        cameraPosition += cameraSpeed * cameraFront;

    if (keys[GLFW_KEY_S])
        cameraPosition -= cameraSpeed * cameraFront;

    if (keys[GLFW_KEY_A])
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (keys[GLFW_KEY_D])
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (keys[GLFW_KEY_Q])
        cameraPosition += cameraSpeed * cameraUp;

    if (keys[GLFW_KEY_E])
        cameraPosition -= cameraSpeed * cameraUp;

    // Projection toggle
    if (keys[GLFW_KEY_P])
        perspectiveView = true;

    if (keys[GLFW_KEY_O])
        perspectiveView = false;
}

// MOUSE LOOK 
void ViewManager::ProcessMouseMovement(float xoffset, float yoffset)
{
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

// SCROLL 
void ViewManager::ProcessMouseScroll(float yoffset)
{
    cameraSpeed += yoffset * 0.01f;

    if (cameraSpeed < 0.01f)
        cameraSpeed = 0.01f;
}

// VIEW MATRIX 
glm::mat4 ViewManager::GetViewMatrix()
{
    return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

// PROJECTION MATRIX 
glm::mat4 ViewManager::GetProjectionMatrix(float aspect)
{
    if (perspectiveView)
    {
        return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    }
    else
    {
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    }
}

// GLFW MOUSE CALLBACK
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
    if (g_pCurrentViewManager == nullptr)
    {
        return;
    }

    if (g_pCurrentViewManager->firstMouse)
    {
        g_pCurrentViewManager->lastX = static_cast<float>(xMousePos);
        g_pCurrentViewManager->lastY = static_cast<float>(yMousePos);
        g_pCurrentViewManager->firstMouse = false;
    }

    float xOffset = static_cast<float>(xMousePos) - g_pCurrentViewManager->lastX;
    float yOffset = g_pCurrentViewManager->lastY - static_cast<float>(yMousePos);

    g_pCurrentViewManager->lastX = static_cast<float>(xMousePos);
    g_pCurrentViewManager->lastY = static_cast<float>(yMousePos);

    g_pCurrentViewManager->ProcessMouseMovement(xOffset, yOffset);
}

// GLFW SCROLL CALLBACK
void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (g_pCurrentViewManager == nullptr)
    {
        return;
    }

    g_pCurrentViewManager->ProcessMouseScroll(static_cast<float>(yOffset));
}

// PROCESS LIVE GLFW KEYBOARD INPUT 
void ViewManager::ProcessKeyboardEvents()
{
    if (m_pWindow == nullptr)
    {
        return;
    }

    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_pWindow, true);
    }

    if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;

    if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;

    if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraUp;

    if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraUp;

    if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
        perspectiveView = true;

    if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
        perspectiveView = false;
}