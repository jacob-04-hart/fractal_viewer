#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    IN,
    OUT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  1.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::dvec2 mandelbrotOffset = glm::vec2(-0.5, 0.0);
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    double mandelbrotZoom = 3.0;
    bool flat = false;
    GLFWwindow *window;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (flat) {
            float velocity = MovementSpeed * deltaTime;
            double mVelocity = MovementSpeed * deltaTime * mandelbrotZoom; //zoom and move less the more you are zoomed
            if (direction == UP)
            {
                mandelbrotOffset.y += mVelocity;
            }
            if (direction == DOWN)
            {
                mandelbrotOffset.y -= mVelocity;
            }
            if (direction == LEFT)
            {
                mandelbrotOffset.x -= mVelocity;
            }
            if (direction == RIGHT)
            {
                mandelbrotOffset.x += mVelocity;
            }
            if (direction == IN)
            {
                float zoomStep = mandelbrotZoom * 0.1f;
                mandelbrotZoom -= 0.5f * zoomStep;
            }
            if (direction == OUT)
            {
                float zoomStep = mandelbrotZoom * 0.1f;
                mandelbrotZoom += 0.5f * zoomStep;
                if (mandelbrotZoom > 10.0f)
                    mandelbrotZoom = 10.0f;
            }
        } else {
            float velocity = MovementSpeed * deltaTime;
            float mVelocity = velocity * (Zoom / 45.0f); //zoom and move less the more you are zoomed
            if (direction == UP)
            {
                Position += Up * mVelocity;
            }
            if (direction == DOWN)
            {
                Position -= Up * mVelocity;
            }
            if (direction == LEFT)
            {
                Position -= Right * mVelocity;
            }
            if (direction == RIGHT)
            {
                Position += Right * mVelocity;
            }
            if (direction == IN)
            {
                float zoomStep = Zoom * 0.05f;
                Zoom -= 1.0f * zoomStep;
            }
            if (direction == OUT)
            {
                float zoomStep = Zoom * 0.05f;
                Zoom += 1.0f * zoomStep;
                if (Zoom > 45.0f)
                    Zoom = 45.0f;
            }
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (flat) {
            float zoomStep = mandelbrotZoom * 0.1f; // 10% of current zoom
            mandelbrotZoom -= yoffset * zoomStep;
            if (mandelbrotZoom > 10.0f)
                mandelbrotZoom = 10.0f;
        } else {
            float zoomStep = Zoom * 0.05f;
            Zoom -= yoffset * zoomStep;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

