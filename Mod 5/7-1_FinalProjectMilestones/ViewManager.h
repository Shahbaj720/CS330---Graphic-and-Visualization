///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport - camera, projection
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "ShaderManager.h"

// ViewManager class handles camera movement and view transformations
class ViewManager
{
public:

    // ORIGINAL CONSTRUCTOR (REQUIRED)
    ViewManager(ShaderManager* pShaderManager);
    ~ViewManager();

    // ORIGINAL REQUIRED FUNCTIONS 
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);
    void PrepareSceneView();

    // MILESTONE 3 INPUT FUNCTIONS 
    void ProcessKeyboard(bool* keys);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

    // VIEW / PROJECTION
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspect);

    // GLFW CALLBACK FUNCTIONS
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
    static void Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

private:

    // ORIGINAL VARIABLES
    ShaderManager* m_pShaderManager;
    GLFWwindow* m_pWindow;

    // CAMERA VARIABLES
    glm::vec3 cameraPosition;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float cameraSpeed;
    float yaw;
    float pitch;

    bool perspectiveView;

    // MOUSE STATE
    float lastX;
    float lastY;
    bool firstMouse;
       
    // PROCESS LIVE GLFW KEYBOARD INPUT
    void ProcessKeyboardEvents();
};