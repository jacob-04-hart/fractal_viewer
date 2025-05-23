#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader_s.h"
#include "camera.h"

#include <iostream>
#include <cmath>
#include <vector>

#include <nanogui/nanogui.h>
#include <nanogui/textbox.h>
#include <nanogui/button.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool isFirstDown = true;
bool justStartedDragging = false;

float orgX;
float orgY;

float rotX;
float rotY;

float preRotX;
float preRotY;

float totalRotX, totalRotY;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

std::vector<float> color1 = {1.0f, 0.0f, 0.0f}; // red
std::vector<float> color2 = {0.0f, 1.0f, 0.0f}; // green
std::vector<float> color3 = {0.0f, 0.0f, 1.0f}; // blue
std::vector<float> color4 = {1.0f, 1.0f, 0.0f}; // yellow

unsigned int maxDepth = 3; // change this to either save or set fire to your computer

// Face 1
const std::vector<float> f1vertex1 = {.5f, .5f, .5f};
const std::vector<float> f1vertex2 = {-.5f, -.5f, .5f};
const std::vector<float> f1vertex3 = {.5f, -.5f, -.5f};

// Face 2
const std::vector<float> f2vertex1 = {.5f, .5f, .5f};
const std::vector<float> f2vertex2 = {.5f, -.5f, -.5f};
const std::vector<float> f2vertex3 = {-.5f, .5f, -.5f};

// Face 3
const std::vector<float> f3vertex1 = {.5f, .5f, .5f};
const std::vector<float> f3vertex2 = {-.5f, .5f, -.5f};
const std::vector<float> f3vertex3 = {-.5f, -.5f, .5f};

// Face 4
const std::vector<float> f4vertex1 = {-.5f, -.5f, .5f};
const std::vector<float> f4vertex2 = {-.5f, .5f, -.5f};
const std::vector<float> f4vertex3 = {.5f, -.5f, -.5f};

// 2:2:3 triangle d8 vertices
const float sqrt3 = sqrt(3);
const std::vector<float> eqTVertex1 = {1.0f,0.0f,0.0f};
const std::vector<float> eqTVertex2 = {-.5f,sqrt3/2.0f,0.0f};
const std::vector<float> eqTVertex3 = {-.5f,-(sqrt3/2.0f),0.0f};

const std::vector<float> D4Top = {0.0f,0.0f,.4f};
const std::vector<float> D4Bottom = {0.0f,0.0f,-.4f};

float splitValue = 1.0f/9.0f;


std::vector<float> crossProduct(const std::vector<float> &a, const std::vector<float> &b)
{
    return {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]};
}

std::vector<float> pointsVector(const std::vector<float> &a, const std::vector<float> &b)
{
    std::vector<float> ab(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ab[i] = b[i] - a[i];
    }
    return ab;
}

std::vector<float> normalize(const std::vector<float> &v)
{
    float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length == 0.0f)
        return {0.0f, 0.0f, 0.0f};
    return {v[0] / length, v[1] / length, v[2] / length};
}

std::vector<float> normal(const std::vector<float> &a, const std::vector<float> &b, const std::vector<float> &c)
{
    std::vector<float> ab = pointsVector(a, b);
    std::vector<float> ac = pointsVector(a, c);
    return normalize(crossProduct(ab, ac));
}

std::vector<float> midpoint(std::vector<float> c1, std::vector<float> c2)
{
    float x = (c1[0] + c2[0]) / 2;
    float y = (c1[1] + c2[1]) / 2;
    float z = (c1[2] + c2[2]) / 2;
    std::vector<float> midpoint;
    midpoint.insert(midpoint.end(), x);
    midpoint.insert(midpoint.end(), y);
    midpoint.insert(midpoint.end(), z);
    return midpoint;
}

bool vectorEquals(const std::vector<float> &v1, const std::vector<float> &v2)
{
    if (v1.size() != v2.size())
        return false;
    for (size_t i = 0; i < v1.size(); ++i)
    {
        if (std::abs(v1[i] - v2[i]) > 1e-1)
            return false;
    }
    return true;
}

bool normalsEqual(const std::vector<float> &n1, const std::vector<float> &n2, float epsilon = 1e-1)
{
    float dot = n1[0] * n2[0] + n1[1] * n2[1] + n1[2] * n2[2];
    return std::abs(std::abs(dot) - 1.0f) < epsilon;
}

std::vector<float> rotateAroundAxis(
    const std::vector<float>& point,
    const std::vector<float>& center,
    const std::vector<float>& axis,
    float angle)
{
    glm::vec3 p(point[0] - center[0], point[1] - center[1], point[2] - center[2]);
    glm::vec3 ax(axis[0], axis[1], axis[2]);
    glm::vec3 rotated = glm::rotate(p, angle, glm::normalize(ax));
    return {rotated.x + center[0], rotated.y + center[1], rotated.z + center[2]};
}

const std::vector<float> normal1 = normal(f1vertex1, f1vertex2, f1vertex3);
const std::vector<float> normal2 = normal(f2vertex1, f2vertex2, f2vertex3);
const std::vector<float> normal3 = normal(f3vertex1, f3vertex2, f3vertex3);
const std::vector<float> normal4 = normal(f4vertex1, f4vertex2, f4vertex3);

const std::vector<float> d4TopNormal1 = normal(D4Top,eqTVertex1,eqTVertex2);
const std::vector<float> d4TopNormal2 = normal(D4Top,eqTVertex2,eqTVertex3);
const std::vector<float> d4TopNormal3 = normal(D4Top,eqTVertex3,eqTVertex1);

const std::vector<float> d4BottomNormal1 = normal(D4Bottom,eqTVertex2,eqTVertex1);
const std::vector<float> d4BottomNormal2 = normal(D4Bottom,eqTVertex3,eqTVertex2);
const std::vector<float> d4BottomNormal3 = normal(D4Bottom,eqTVertex1,eqTVertex3);

void drawTriangle(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> &vertices)
{
    std::vector<float> n = normal(a, b, c);

    const std::vector<std::vector<float>> faceNormals = {normal1, normal2, normal3, normal4};
    const std::vector<std::vector<float>> faceColors = {color1, color2, color3, color4};

    float maxDot = -1.0f;
    int bestFace = 0;
    for (int i = 0; i < 4; ++i)
    {
        float dot = std::abs(n[0] * faceNormals[i][0] + n[1] * faceNormals[i][1] + n[2] * faceNormals[i][2]);
        if (dot > maxDot)
        {
            maxDot = dot;
            bestFace = i;
        }
    }

    const std::vector<float> &color = faceColors[bestFace];
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
}

//even parity:
//Top/a/b and Bottom/a/b will be color1, T/b/c and B/b/c will be color2, T/c/a and B/c/a will be color3
//odd parity:
//Top/c/a and Top/a/b will be color1, T/b/c and B/b/c will be color2, B/a/b and B/c/a will be color3
void draw2D4(
    const std::vector<float>& a,
    const std::vector<float>& b,
    const std::vector<float>& c,
    const std::vector<float>& top,
    const std::vector<float>& bottom,
    int parity,
    std::vector<float>& vertices)
{
    // Choose colors for each face based on parity
    std::vector<float> col1, col2, col3, col4, col5, col6;

    //if(parity==0||parity==4||parity==8){
        col1 = color1;
        col2 = color2;
        col3 = color1;
        col4 = color1;
        col5 = color2;
        col6 = color1;
    /*} else if(parity==1||parity==5||parity==9){
        col1 = color1;
        col2 = color2;
        col3 = color1;
        col4 = color1;
        col5 = color2;
        col6 = color1;
    } else if(parity==2||parity==6||parity==10){
        col1 = color1;
        col2 = color2;
        col3 = color1;
        col4 = color1;
        col5 = color2;
        col6 = color1;
    } else if(parity==3||parity==7||parity==11){
        col1 = color1;
        col2 = color2;
        col3 = color1;
        col4 = color1;
        col5 = color2;
        col6 = color1;
    }*/

    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), col1.begin(), col1.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), col1.begin(), col1.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), col1.begin(), col1.end());

    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), col2.begin(), col2.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), col2.begin(), col2.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), col2.begin(), col2.end());

    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), col3.begin(), col3.end());
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), col3.begin(), col3.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), col3.begin(), col3.end());

    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), col4.begin(), col4.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), col4.begin(), col4.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), col4.begin(), col4.end());

    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), col5.begin(), col5.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), col5.begin(), col5.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), col5.begin(), col5.end());

    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), col6.begin(), col6.end());
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), col6.begin(), col6.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), col6.begin(), col6.end());
}

std::vector<float> split(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result = a;
    float t = 4.0f / 9.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + (b[i] - a[i]) * t;
    }
    return result;
}

void drawK2D4(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> top, std::vector<float> bottom, int depth, int parity, std::vector<float> &vertices) 
{
    if (depth < maxDepth)
    {
        std::vector<float> newT1 = split(a,b);
        std::vector<float> newT2 = split(b,a);
        std::vector<float> newBot2 = split(b,c);
        std::vector<float> newBot3 = split(c,b);
        std::vector<float> newT3 = split(c,a);
        std::vector<float> newBot1 = split(a,c);

        drawK2D4(a,bottom,top,newT1,newBot1,depth+1,parity+1,vertices);
        drawK2D4(b,bottom,top,newT2,newBot2,depth+1,parity+1,vertices);
        drawK2D4(c,bottom,top,newT3,newBot3,depth+1,parity++,vertices);
    } else {
        draw2D4(a,b,c,top,bottom,parity,vertices);
    }
}

void drawKT(std::vector<float> a, std::vector<float> b, std::vector<float> c, int depth,
            std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        std::vector<float> mid1 = midpoint(c, a);
        std::vector<float> mid2 = midpoint(a, b);
        std::vector<float> mid3 = midpoint(b, c);

        std::vector<float> newA1 = mid1;
        std::vector<float> newB1 = mid2;

        std::vector<float> newA2 = mid2;
        std::vector<float> newB2 = mid3;

        std::vector<float> newA3 = mid3;
        std::vector<float> newB3 = mid1;

        std::vector<float> origNormal = normal(a, b, c);
        std::vector<float> baseNormal = normal(mid1, mid2, mid3);

        float edgeLength = std::sqrt(
            (mid1[0] - mid2[0]) * (mid1[0] - mid2[0]) +
            (mid1[1] - mid2[1]) * (mid1[1] - mid2[1]) +
            (mid1[2] - mid2[2]) * (mid1[2] - mid2[2]));
        float height = std::sqrt(2.0f / 3.0f) * edgeLength;

        std::vector<float> centroid = {
            (mid1[0] + mid2[0] + mid3[0]) / 3.0f,
            (mid1[1] + mid2[1] + mid3[1]) / 3.0f,
            (mid1[2] + mid2[2] + mid3[2]) / 3.0f};

        std::vector<float> newC1 = {
            centroid[0] + baseNormal[0] * height,
            centroid[1] + baseNormal[1] * height,
            centroid[2] + baseNormal[2] * height};
        
        drawKT(mid1, mid2, newC1, depth + 1, vertices);
        drawKT(mid2, mid3, newC1, depth + 1, vertices);
        drawKT(mid3, mid1, newC1, depth + 1, vertices);

        if (depth < (maxDepth - 1))
        {
            drawKT(a, mid2, mid1, depth + 1, vertices);
            drawKT(b, mid3, mid2, depth + 1, vertices);
            drawKT(c, mid1, mid3, depth + 1, vertices);
        }
        else
        {
            drawTriangle(mid2, mid1, a, vertices);
            drawTriangle(mid3, mid2, b, vertices);
            drawTriangle(mid1, mid3, c, vertices);
        }
        //drawTriangle(mid1, mid2, mid3, vertices);
    }
    else
    {
        drawTriangle(a, b, c, vertices);
    }
}

void drawKT2(std::vector<float> a, std::vector<float> b, std::vector<float> c, int depth,
            std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        std::vector<float> mid1 = midpoint(c, a);
        std::vector<float> mid2 = midpoint(a, b);
        std::vector<float> mid3 = midpoint(b, c);

        std::vector<float> newA1 = mid1;
        std::vector<float> newB1 = mid2;

        std::vector<float> newA2 = mid2;
        std::vector<float> newB2 = mid3;

        std::vector<float> newA3 = mid3;
        std::vector<float> newB3 = mid1;

        std::vector<float> origNormal = normal(a, b, c);
        std::vector<float> baseNormal = normal(mid1, mid2, mid3);

        float edgeLength = std::sqrt(
            (mid1[0] - mid2[0]) * (mid1[0] - mid2[0]) +
            (mid1[1] - mid2[1]) * (mid1[1] - mid2[1]) +
            (mid1[2] - mid2[2]) * (mid1[2] - mid2[2]));
        float height = std::sqrt(2.0f / 3.0f) * edgeLength;

        std::vector<float> centroid = {
            (mid1[0] + mid2[0] + mid3[0]) / 3.0f,
            (mid1[1] + mid2[1] + mid3[1]) / 3.0f,
            (mid1[2] + mid2[2] + mid3[2]) / 3.0f};

        std::vector<float> newC1 = {
            centroid[0] + baseNormal[0] * height,
            centroid[1] + baseNormal[1] * height,
            centroid[2] + baseNormal[2] * height};
        if (depth%3!=0){
            drawKT2(mid1, mid2, newC1, depth + 1, vertices);
            drawKT2(mid2, mid3, newC1, depth + 1, vertices);
            drawKT2(mid3, mid1, newC1, depth + 1, vertices);
        }

        if (depth < (maxDepth - 1))
        {
            drawKT2(a, mid2, mid1, depth + 1, vertices);
            drawKT2(b, mid3, mid2, depth + 1, vertices);
            drawKT2(c, mid1, mid3, depth + 1, vertices);
        }
        else
        {
            drawTriangle(mid2, mid1, a, vertices);
            drawTriangle(mid3, mid2, b, vertices);
            drawTriangle(mid1, mid3, c, vertices);
        }
        drawTriangle(mid1, mid2, mid3, vertices);
    }
    else
    {
        drawTriangle(a, b, c, vertices);
    }
};

void drawKT3(std::vector<float> a, std::vector<float> b, std::vector<float> c, int depth,
            std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        std::vector<float> mid1 = midpoint(c, a);
        std::vector<float> mid2 = midpoint(a, b);
        std::vector<float> mid3 = midpoint(b, c);

        std::vector<float> newA1 = mid1;
        std::vector<float> newB1 = mid2;

        std::vector<float> newA2 = mid2;
        std::vector<float> newB2 = mid3;

        std::vector<float> newA3 = mid3;
        std::vector<float> newB3 = mid1;

        std::vector<float> origNormal = normal(a, b, c);
        std::vector<float> baseNormal = normal(mid1, mid2, mid3);

        float edgeLength = std::sqrt(
            (mid1[0] - mid2[0]) * (mid1[0] - mid2[0]) +
            (mid1[1] - mid2[1]) * (mid1[1] - mid2[1]) +
            (mid1[2] - mid2[2]) * (mid1[2] - mid2[2]));
        float height = std::sqrt(2.0f / 3.0f) * edgeLength;

        std::vector<float> centroid = {
            (mid1[0] + mid2[0] + mid3[0]) / 3.0f,
            (mid1[1] + mid2[1] + mid3[1]) / 3.0f,
            (mid1[2] + mid2[2] + mid3[2]) / 3.0f};

        std::vector<float> newC1 = {
            centroid[0] + baseNormal[0] * height,
            centroid[1] + baseNormal[1] * height,
            centroid[2] + baseNormal[2] * height};

        drawKT3(mid1, mid2, newC1, 20, vertices);
        drawKT3(mid2, mid3, newC1, 20, vertices);
        drawKT3(mid3, mid1, newC1, 20, vertices);

        if (depth < (maxDepth - 1))
        {
            drawKT3(mid2, mid1, a, depth + 1, vertices);
            drawKT3(mid3, mid2, b, depth + 1, vertices);
            drawKT3(mid1, mid3, c, depth + 1, vertices);
        }
        else
        {
            drawTriangle(mid1, mid2, a, vertices);
            drawTriangle(mid2, mid3, b, vertices);
            drawTriangle(mid3, mid1, c, vertices);
        }
        drawTriangle(mid1, mid2, mid3, vertices);
    }
    else
    {
        drawTriangle(a, b, c, vertices);
    }
};

void drawST(std::vector<float> a, std::vector<float> b, std::vector<float> c, int depth,
            std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        std::vector<float> mid1 = midpoint(a, b);
        std::vector<float> mid2 = midpoint(b, c);
        std::vector<float> mid3 = midpoint(c, a);

        drawST(a, mid1, mid3, depth + 1, vertices);
        drawST(mid1, b, mid2, depth + 1, vertices);
        drawST(mid3, mid2, c, depth + 1, vertices);

        std::vector<float> center = {
            (mid1[0] + mid2[0] + mid3[0]) / 3.0f,
            (mid1[1] + mid2[1] + mid3[1]) / 3.0f,
            (mid1[2] + mid2[2] + mid3[2]) / 3.0f};
        std::vector<float> n = normal(mid1, mid2, mid3);

        float edgeLength = std::sqrt(
            (mid1[0] - mid2[0]) * (mid1[0] - mid2[0]) +
            (mid1[1] - mid2[1]) * (mid1[1] - mid2[1]) +
            (mid1[2] - mid2[2]) * (mid1[2] - mid2[2]));
        float height = std::sqrt(2.0f / 3.0f) * edgeLength;

        std::vector<float> inner1 = {mid1[0] - n[0] * height, mid1[1] - n[1] * height, mid1[2] - n[2] * height};
        std::vector<float> inner2 = {mid2[0] - n[0] * height, mid2[1] - n[1] * height, mid2[2] - n[2] * height};
        std::vector<float> inner3 = {mid3[0] - n[0] * height, mid3[1] - n[1] * height, mid3[2] - n[2] * height};

        float angle = glm::radians(60.0f);
        std::vector<float> rotated1 = rotateAroundAxis(inner1, center, n, angle);
        std::vector<float> rotated2 = rotateAroundAxis(inner2, center, n, angle);
        std::vector<float> rotated3 = rotateAroundAxis(inner3, center, n, angle);

        drawST(rotated1, rotated2, rotated3, depth + 1, vertices);
    }
    else
    {
        drawTriangle(a, b, c, vertices);
    }
}

void drawInverseST(std::vector<float> a, std::vector<float> b, std::vector<float> c, int depth,
            std::vector<float> &vertices)
{
    std::vector<float> mid1 = midpoint(a, b);
    std::vector<float> mid2 = midpoint(b, c);
    std::vector<float> mid3 = midpoint(c, a);
    std::vector<float> center = {
        (mid1[0] + mid2[0] + mid3[0]) / 3.0f,
        (mid1[1] + mid2[1] + mid3[1]) / 3.0f,
        (mid1[2] + mid2[2] + mid3[2]) / 3.0f};
    std::vector<float> n = normal(mid1, mid2, mid3);

    float edgeLength = std::sqrt(
        (mid1[0] - mid2[0]) * (mid1[0] - mid2[0]) +
        (mid1[1] - mid2[1]) * (mid1[1] - mid2[1]) +
        (mid1[2] - mid2[2]) * (mid1[2] - mid2[2]));
    float height = std::sqrt(2.0f / 3.0f) * edgeLength;

    std::vector<float> inner1 = {mid1[0] - n[0] * height, mid1[1] - n[1] * height, mid1[2] - n[2] * height};
    std::vector<float> inner2 = {mid2[0] - n[0] * height, mid2[1] - n[1] * height, mid2[2] - n[2] * height};
    std::vector<float> inner3 = {mid3[0] - n[0] * height, mid3[1] - n[1] * height, mid3[2] - n[2] * height};

    float angle = glm::radians(60.0f);
    std::vector<float> rotated1 = rotateAroundAxis(inner1, center, n, angle);
    std::vector<float> rotated2 = rotateAroundAxis(inner2, center, n, angle);
    std::vector<float> rotated3 = rotateAroundAxis(inner3, center, n, angle);

    drawTriangle(mid1, mid2, mid3, vertices);
    drawTriangle(rotated1, rotated2, rotated3, vertices);
    if (depth < maxDepth)
    {
        drawInverseST(a, mid1, mid3, depth + 1, vertices);
        drawInverseST(mid1, b, mid2, depth + 1, vertices);
        drawInverseST(mid3, mid2, c, depth + 1, vertices);
        drawInverseST(rotated1, rotated2, rotated3, depth + 1, vertices);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* window, unsigned int codepoint);

nanogui::Screen* g_screen = nullptr;

int main()
{   
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "fractal_viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    nanogui::Screen screen;
    screen.initialize(window, false);
    g_screen = &screen;

    screen.setVisible(true);
    screen.performLayout();

    Shader ourShader("../src/shader.vs", "../src/shader.fs");

    std::vector<float> vertices;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    bool readyToDraw = false;

    int type = 0;
    int renderedType = 0;
    std::vector<std::string> typeOptions = {"Koch Tetrahedron", "Checkered Koch", "Pointy Koch", "3D Sierpinski", "3D Inverse Sierpinski", "Split Koch"};

    nanogui::ref<nanogui::Window> comboWindow = new nanogui::Window(&screen, "Type");
    comboWindow->setPosition(Eigen::Vector2i(10, 10));
    comboWindow->setLayout(new nanogui::GroupLayout());
    auto *combo = new nanogui::ComboBox(comboWindow, typeOptions);
    combo->setSelectedIndex(type);
    combo->setCallback([&type](int idx)
                       { type = idx; });

    nanogui::ref<nanogui::Window> colorsWindow = new nanogui::Window(&screen, "Colors");
    colorsWindow->setPosition(Eigen::Vector2i(10, 110));
    colorsWindow->setLayout(new nanogui::GroupLayout());
    colorsWindow->setSize(Eigen::Vector2i(250, 400));
    nanogui::TabWidget *colors = new nanogui::TabWidget(colorsWindow);
    
    nanogui::Widget* layer1 = colors->createTab("1");
    layer1->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer1, "Color 1");
    nanogui::ColorWheel *colorWheel1 = new nanogui::ColorWheel(layer1);
    colorWheel1->setColor(nanogui::Color(color1[0], color1[1], color1[2], 1.0f));

    nanogui::Widget* layer2 = colors->createTab("2");
    layer2->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer2, "Color 2");
    nanogui::ColorWheel *colorWheel2 = new nanogui::ColorWheel(layer2);
    colorWheel2->setColor(nanogui::Color(color2[0], color2[1], color2[2], 1.0f));

    nanogui::Widget* layer3 = colors->createTab("3");
    layer3->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer3, "Color 3");
    nanogui::ColorWheel *colorWheel3 = new nanogui::ColorWheel(layer3);
    colorWheel3->setColor(nanogui::Color(color3[0], color3[1], color3[2], 1.0f));

    nanogui::Widget* layer4 = colors->createTab("4");
    layer4->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer4, "Color 4");
    nanogui::ColorWheel *colorWheel4 = new nanogui::ColorWheel(layer4);
    colorWheel4->setColor(nanogui::Color(color4[0], color4[1], color4[2], 1.0f));

    nanogui::ref<nanogui::Window> paramsWindow = new nanogui::Window(&screen, "Parameters");
    paramsWindow->setPosition(Eigen::Vector2i(10, 360));
    paramsWindow->setLayout(new nanogui::GroupLayout());
    paramsWindow->setSize(Eigen::Vector2i(250, 250));
    nanogui::Widget *params = new nanogui::Widget(paramsWindow);
    params->setLayout(new nanogui::GroupLayout());

    params->add<nanogui::Label>("Recursive Depth", "sans-bold");
    int guiMaxDepth = maxDepth;
    nanogui::IntBox<int> *depthBox = new nanogui::IntBox<int>(params);
    depthBox->setValue(guiMaxDepth);
    depthBox->setEditable(true);
    depthBox->setMinValue(0);
    depthBox->setMaxValue(10);

    nanogui::Button *generateButton = new nanogui::Button(params, "Generate");
    generateButton->setCallback([&, depthBox]() {
        maxDepth = depthBox->value();
        nanogui::Color c1 = colorWheel1->color();
        color1 = {c1.r(), c1.g(), c1.b()};
        nanogui::Color c2 = colorWheel2->color();
        color2 = {c2.r(), c2.g(), c2.b()};
        nanogui::Color c3 = colorWheel3->color();
        color3 = {c3.r(), c3.g(), c3.b()};
        nanogui::Color c4 = colorWheel4->color();
        color4 = {c4.r(), c4.g(), c4.b()};
        vertices.clear();
        if (type==0){
            drawKT(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
        } else if (type==1){
            drawKT2(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT2(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT2(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT2(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
        } else if (type==2){
            drawKT3(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT3(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT3(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT3(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
        }else if (type==3){
            drawST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
        }else if (type==4){
            drawInverseST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawInverseST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawInverseST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawInverseST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
        }else if (type==5){
            drawK2D4(eqTVertex1,eqTVertex2,eqTVertex3,D4Top,D4Bottom,0,0,vertices);
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        readyToDraw = true; // <--- Set flag to true
        renderedType = type;
    });

    screen.setVisible(true);
    screen.performLayout();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (readyToDraw) {
            ourShader.use();

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            ourShader.setMat4("projection", projection);

            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            if(isFirstDown){
                totalRotX = preRotX;
                totalRotY = preRotY;
            }else{
                totalRotX = preRotX + rotX;
                totalRotY = preRotY + rotY;
            }
            model = glm::rotate(model, glm::radians(totalRotY), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(totalRotX), glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", model);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);

            if (renderedType == 1)
            {
                glm::mat4 rotatedModel = glm::mat4(1.0f);
                if (isFirstDown)
                {
                    totalRotX = preRotX;
                    totalRotY = preRotY;
                }
                else
                {
                    totalRotX = preRotX + rotX;
                    totalRotY = preRotY + rotY;
                }
                rotatedModel = glm::rotate(rotatedModel, glm::radians(totalRotY), glm::vec3(1.0f, 0.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(totalRotX), glm::vec3(0.0f, 1.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                ourShader.setMat4("model", rotatedModel);

                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
            }
        }

        screen.drawContents();
        screen.drawWidgets();

        // Restore OpenGL state NanoGUI may have changed
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glCullFace(GL_BACK);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (g_screen && g_screen->mouseButtonCallbackEvent(button, action, mods)) {
        return; // Event handled by NanoGUI
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isFirstDown = false;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            orgX = xpos;
            orgY = ypos;
            rotX = 0;
            rotY = 0;
            justStartedDragging = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else if (action == GLFW_RELEASE) {
            isFirstDown = true;
            preRotX += rotX;
            preRotY += rotY;
            rotX = 0;
            rotY = 0;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (g_screen && g_screen->cursorPosCallbackEvent(xpos, ypos)) {
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    if(isFirstDown){
        //camera.ProcessMouseMovement(xoffset, yoffset);
    } else {
        if (justStartedDragging) {
            orgX = xpos;
            orgY = ypos;
            rotX = 0;
            rotY = 0;
            justStartedDragging = false;
        } else {
            rotX = xpos - orgX;
            rotY = ypos - orgY;
        }
    }
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_screen && g_screen->scrollCallbackEvent(xoffset, yoffset)) {
        return;
    }
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{   
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::cout << "Key event: " << key << " action: " << action << std::endl;
    if (g_screen && g_screen->keyCallbackEvent(key, scancode, action, mods)) {
        return;
    }
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    if (g_screen && g_screen->charCallbackEvent(codepoint)) {
        return;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}