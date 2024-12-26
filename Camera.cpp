#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

const float NORMAL_SPEED = 0.5f;
const float MAXSPEED = 1.0f;
const float Accelaration = 0.01f;
const float SENSITIVITY = 0.1f;
	// ye sab config values hai 
bool FIRST_MOVE = true;
// jab humara window load hoat hai lab lastPos nhi hota is liye hum Fisrt _move boo bante hai since at staring it might cause error or disloaction

Camera::Camera(int width, int height, float fov, float nearplane, float farplane, C_Shader shader)
{
	UP = glm::vec3(0.0f, 1.0f, 0.0f);
	Stand = glm::vec3(0.0f);
	//start me pos 0.0 hoga
	Focus = glm::vec3(0.0f, 0.f, -3.0f);
	// ye start ka focus hai

	winWidth = width;
	winHeight = height;
	nearPlane = nearplane;
	farPlane = farplane;
	FOV = fov;
	//ye deafault set karega
	Speed = NORMAL_SPEED;
	

	projectionMatrix = glm::perspective(glm::radians(FOV), (float)(winWidth / winHeight), nearPlane, farPlane);
	finalMatLOCATION = glGetUniformLocation(shader.GPUcode, "finalMatrix");
}

void Camera::CamInputs(GLFWwindow* window)
{
	if (glfwGetKey(window , GLFW_KEY_W) == GLFW_PRESS ){
		Stand += Speed * Focus;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Stand += Speed * (-Focus);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Stand += Speed * glm::normalize(glm::cross(UP,Focus));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Stand += Speed * glm::normalize(glm::cross(Focus,UP));
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		Stand += Speed * UP;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		Stand += Speed * (-UP);
	}
	/*
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && (Speed <= MAXSPEED) ) {
		Speed += Accelaration;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		Speed = NORMAL_SPEED;
	}
	
	 yaha par expected result nhi milraha becuase loop baar baar pressstate check karraha hai or mene input delay rakhahai jab 60 input hojata hai tab speed reset ho jata hai to norm */
}

void Camera::compMatrix()
{
	ViewMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::lookAt(Stand,Stand+ Focus, UP);
	//yaha pe stand + focus kiya kyuki if not toh camera along focus rotatehori hai when gettin to left
	ViewMatrix = projectionMatrix * ViewMatrix;
	glUniformMatrix4fv(finalMatLOCATION, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
}

void Camera::UpdateProjection(int width, int height)
{
	projectionMatrix = glm::mat4(1.0f);
	winHeight = height;
	winWidth = width;
	projectionMatrix = glm::perspective(glm::radians(FOV), (float)(winWidth / winHeight), nearPlane, farPlane);
}

void Camera::CamMouseMove(GLFWwindow* window)
{
	static double lastXpos, lastYpos;
	double Xpos, Ypos;
	float offsetX, offsetY;
	//mistake 1 - not making yaw nas pitch static
	static float yaw = -90.0f, pitch= 0.0f;
	glm::vec3 tempFocus;

	glfwGetCursorPos(window, &Xpos, &Ypos);
	if (FIRST_MOVE) {
		lastXpos = Xpos;
		lastYpos = Ypos;
		FIRST_MOVE = false;
	}
	offsetX = SENSITIVITY* (float)(Xpos - lastXpos);
	offsetY = SENSITIVITY * (float)(lastYpos - Ypos);// idk why its reversed do research

	/*Yaw and Pitch are two of the three angles used to describe the orientation of an object in a 3D space.The third angle is Roll.

		Yaw: This is the rotation around the vertical axis(usually the y - axis).Think of yaw as turning your head left or right.If you imagine looking down at an object from above, yaw would be the angle you turn left or right.

		Pitch : This is the rotation around the lateral or horizontal axis(usually the x - axis).Imagine nodding your head up and down; that's pitch. It tilts the object up or down.*/

	// yaw & pitch records czmera angle preview and used to set next value
	yaw += offsetX;
	pitch += offsetY;

	/*if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
	agar y me topmost ans bottom most lock karna hotoh
	*/

	tempFocus.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tempFocus.y = sin(glm::radians(pitch));
	tempFocus.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	Focus = glm::normalize(tempFocus);
	lastXpos = Xpos;
	lastYpos = Ypos;
}