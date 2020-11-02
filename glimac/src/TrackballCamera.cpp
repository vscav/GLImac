#include <glimac/SDLWindowManager.hpp>
#include "glimac/TrackballCamera.hpp"

TrackballCamera::TrackballCamera()
	:m_fDistance(-5.0f), m_fAngleX(0.0f), m_fAngleY(0.0f)
{}

TrackballCamera::~TrackballCamera() 
{}

void TrackballCamera::moveFront(const float delta) {
	m_fDistance += delta;
}

void TrackballCamera::rotateLeft(const float degrees) {
	m_fAngleX += degrees;
}

void TrackballCamera::rotateUp(const float degrees) {
	m_fAngleY += degrees;
}

glm::mat4 TrackballCamera::getViewMatrix() const {
	// Declare a ViewMatrix
	glm::mat4 ViewMatrix = glm::mat4();

    // Move the scene back along the z axis (distance: m_fDistance)
	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, 0, m_fDistance));

    // Rotate the scene around the x axis (angle: m_fAngleX)
	ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleX), glm::vec3(1, 0, 0));

    // Rotate the scene around the y axis (angle: m_fAngleY)
	ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleY), glm::vec3(0, 1, 0));

	return ViewMatrix;
}