#pragma once

#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
DISABLE_WARNINGS_POP()

#include "util/clock.cpp"
#include "gamestate.cpp"

Camera::Camera(Window *pWindow)
    : Camera(pWindow, glm::vec3(0), glm::vec3(0, 0, -1))
{
}

Camera::Camera(Window *pWindow, const glm::vec3 &pos, const glm::vec3 &forward)
    : m_position(pos), m_forward(glm::normalize(forward)), m_pWindow(pWindow)
{
}

void Camera::setUserInteraction(bool enabled)
{
    m_userInteraction = enabled;
}

glm::vec3 Camera::cameraPos() const
{
    if (m_front_view)
        return m_position + m_zoom * m_forward;
    return m_position - m_zoom * m_forward;
}

glm::mat4 Camera::viewMatrix() const
{
    if (m_front_view)
        return glm::lookAt(m_position + m_zoom * m_forward, m_position - m_forward, m_up);
    return glm::lookAt(m_position - m_zoom * m_forward, m_position + m_forward, m_up);
}

void Camera::rotateX(float angle)
{
    const glm::vec3 horAxis = glm::cross(s_yAxis, m_forward);

    // m_position += m_zoom * m_forward;
    m_forward = glm::normalize(glm::angleAxis(angle, horAxis) * m_forward);
    // m_position -= m_zoom * m_forward;
    m_up = glm::normalize(glm::cross(m_forward, horAxis));
    m_right = glm::normalize(glm::cross(m_forward, m_up));
}

void Camera::rotateY(float angle)
{
    const glm::vec3 horAxis = glm::cross(s_yAxis, m_forward);

    // m_position += m_zoom * m_forward;
    m_forward = glm::normalize(glm::angleAxis(angle, s_yAxis) * m_forward);
    // m_position -= m_zoom * m_forward;
    m_up = glm::normalize(glm::cross(m_forward, horAxis));
    m_right = glm::normalize(glm::cross(m_forward, m_up));
}

void Camera::initialInput()
{
    const glm::dvec2 cursorPos = m_pWindow->getCursorPos();
    m_prevCursorPos = cursorPos;
}

void Camera::reset()
{
    m_position = { 0, 0, 0.0f };
    m_forward = { 0, 0, -1 };
    m_up = { 0, 1, 0 };
    m_right = { 1, 0, 0 };
}

void Camera::updateInput()
{
    float directionalAcceleration = (0.1f + 0.1f * gamestate::powerUp) * timing::delta_s;
    float maxMoveSpeed = 0.15f + 0.2f * gamestate::powerUp;
    float lookSpeed = 0.0035f;

    glm::vec3 acceleration = glm::vec3(0.0f);
    m_front_view = m_pWindow->isKeyPressed(GLFW_KEY_TAB);

    glm::vec3 localMoveDelta{0};

    const glm::vec3 right = glm::normalize(glm::cross(m_forward, m_up));
    if (m_pWindow->isKeyPressed(GLFW_KEY_ESCAPE)){
        m_pWindow->setMouseCapture(false);
        mouse_captured = false;
        gamestate::paused = true;
    }

    if (m_pWindow->isKeyPressed(GLFW_KEY_P)) {
        m_pWindow->setMouseCapture(true);
        mouse_captured = true;
        gamestate::paused = false;
        initialInput();
    }

    m_zoom = glm::clamp(m_zoom, 5.0f, 10.0f);

    if (mouse_captured)
    {
        const glm::dvec2 cursorPos = m_pWindow->getCursorPos();
        const glm::vec2 delta = lookSpeed * glm::vec2(cursorPos - m_prevCursorPos);
        m_prevCursorPos = cursorPos;

        if (delta.x != 0.0f)
            rotateY(-delta.x);
        if (delta.y != 0.0f)
            rotateX(-delta.y);
    }

    if (!gamestate::paused)
    {
        if (m_pWindow->isKeyPressed(GLFW_KEY_A))
            acceleration -= directionalAcceleration * right;
        if (m_pWindow->isKeyPressed(GLFW_KEY_D))
            acceleration += directionalAcceleration * right;
        if (m_pWindow->isKeyPressed(GLFW_KEY_W))
            acceleration += directionalAcceleration * m_forward;
        if (m_pWindow->isKeyPressed(GLFW_KEY_S))
            acceleration -= directionalAcceleration * m_forward;
        if (m_pWindow->isKeyPressed(GLFW_KEY_SPACE))
            acceleration += directionalAcceleration * m_up;
        if (m_pWindow->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
            acceleration -= directionalAcceleration * m_up;
        if (m_pWindow->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
            m_zoom -= 0.05f;
        else
            m_zoom += 0.05f;

        glm::vec3 drag = glm::vec3(glm::sign(m_velocity.x) * (directionalAcceleration / 2), glm::sign(m_velocity.y) * (directionalAcceleration / 2), glm::sign(m_velocity.z) * (directionalAcceleration / 2));

        m_velocity += acceleration - m_velocity * 0.01f;
        if (glm::length(m_velocity) > 3 * maxMoveSpeed)
            m_velocity = (m_velocity / maxMoveSpeed) * 3.0f;

        m_position += m_velocity;
    }
    // std::cout << glm::to_string(acceleration) << glm::to_string(m_velocity) << "\n";
}
