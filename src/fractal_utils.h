#pragma once
#include <vector>
#include <cmath>
#include <glm/gtx/rotate_vector.hpp>

float splitValue = 10.0f;
std::vector<float> color1 = {1.0f, 0.0f, 0.0f}; // red
std::vector<float> color2 = {0.0f, 1.0f, 0.0f}; // green
std::vector<float> color3 = {0.0f, 0.0f, 1.0f}; // blue
std::vector<float> color4 = {1.0f, 0.5f, 0.0f}; // orange
std::vector<float> color5 = {1.0f, 1.0f, 0.0f}; // yellow
std::vector<float> color6 = {0.5f, 0.0f, 0.5f}; // purple

unsigned int maxDepth = 6; // change this to either save or set fire to your computer

// tetrahedron vertices
const std::vector<float> tetA = {-.5f,.5f,-.5f};
const std::vector<float> tetB = {-.5f,-.5f,.5f};
const std::vector<float> tetC = {.5f,-.5f,-.5f};
const std::vector<float> tetD = {.5f,.5f,.5f};

// Face 1 (tetrahedron)
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
const std::vector<float> eqTVertex2 = {-.5f,-(sqrt3/2.0f),0.0f};
const std::vector<float> eqTVertex3 = {-.5f,sqrt3/2.0f,0.0f};

std::vector<float> d4Top = {0.0f,0.0f,-.5f};
std::vector<float> d4Bottom = {0.0f,0.0f,.5f};

// cube vertices
const std::vector<float> cubeVert1 = {-.5f,.5f,-.5f};
const std::vector<float> cubeVert2 = {.5f,.5f,-.5f};
const std::vector<float> cubeVert3 = {-.5f,-.5f,-.5f};
const std::vector<float> cubeVert4 = {.5f,-.5f,-.5f};
const std::vector<float> cubeVert5 = {-.5f,.5f,.5f};
const std::vector<float> cubeVert6 = {.5f,.5f,.5f};
const std::vector<float> cubeVert7 = {-.5f,-.5f,.5f};
const std::vector<float> cubeVert8 = {.5f,-.5f,.5f};

void setMaxDepth(int depth)
{
    maxDepth = depth;
}

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

std::vector<float> unnormalizedNormal(const std::vector<float> &a, const std::vector<float> &b, const std::vector<float> &c)
{
    std::vector<float> ab = pointsVector(a, b);
    std::vector<float> ac = pointsVector(a, c);
    return crossProduct(ab, ac);
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

const std::vector<float> d4TopNormal1 = normal(d4Top,eqTVertex1,eqTVertex2);
const std::vector<float> d4TopNormal2 = normal(d4Top,eqTVertex2,eqTVertex3);
const std::vector<float> d4TopNormal3 = normal(d4Top,eqTVertex3,eqTVertex1);

const std::vector<float> d4BottomNormal1 = normal(d4Bottom,eqTVertex2,eqTVertex1);
const std::vector<float> d4BottomNormal2 = normal(d4Bottom,eqTVertex3,eqTVertex2);
const std::vector<float> d4BottomNormal3 = normal(d4Bottom,eqTVertex1,eqTVertex3);

void drawTriangle(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> &vertices)
{
    std::vector<float> n = unnormalizedNormal(a, b, c);

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
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
}
void drawTriangle(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> &vertices, std::vector<float> color){
    const std::vector<float> n = unnormalizedNormal(a,c,b);
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
}

void draw2D4(const std::vector<float>& a, const std::vector<float>& b, const std::vector<float>& c, const std::vector<float>& top, const std::vector<float>& bottom,
        std::vector<float>& vertices, 
        std::vector<float> f1, std::vector<float> f2, std::vector<float> f3,
        std::vector<float> b1, std::vector<float> b2, std::vector<float> b3) 
{
    assert(f1.size() == 3 && f2.size() == 3 && f3.size() == 3);
    assert(b1.size() == 3 && b2.size() == 3 && b3.size() == 3);

    std::vector<float> n = unnormalizedNormal(a, b, top);
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), f1.begin(), f1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), f1.begin(), f1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), f1.begin(), f1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    n = unnormalizedNormal(b, c, top);
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), f2.begin(), f2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), f2.begin(), f2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), f2.begin(), f2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    n = unnormalizedNormal(c, a, top);
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), f3.begin(), f3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), f3.begin(), f3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), top.begin(), top.end());
    vertices.insert(vertices.end(), f3.begin(), f3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    n = unnormalizedNormal(a, bottom, b);
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), b1.begin(), b1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), b1.begin(), b1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), b1.begin(), b1.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    n = unnormalizedNormal(b, bottom, c);
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), b2.begin(), b2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), b2.begin(), b2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), b2.begin(), b2.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    n = unnormalizedNormal(c, bottom, a);
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), b3.begin(), b3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), bottom.begin(), bottom.end());
    vertices.insert(vertices.end(), b3.begin(), b3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), b3.begin(), b3.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
}

std::vector<float> split(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result = a;
    float d = (splitValue - 1)/2;
    float t = d / splitValue;
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + (b[i] - a[i]) * t;
    }
    return result;
}

std::vector<float> thirdSplit(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result = a;
    float t = 1.0f / 3.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + (b[i] - a[i]) * t;
    }
    return result;
}

void drawSquare(std::vector<float> a,std::vector<float> b,std::vector<float> c,std::vector<float> d,std::vector<float> color,std::vector<float> &vertices)
{
    std::vector<float> n = unnormalizedNormal(a, b, c);

    vertices.insert(vertices.end(), a.begin(), a.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());

    vertices.insert(vertices.end(), d.begin(), d.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), c.begin(), c.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
    vertices.insert(vertices.end(), b.begin(), b.end());
    vertices.insert(vertices.end(), color.begin(), color.end());
    vertices.insert(vertices.end(), n.begin(), n.end());
}

std::vector<std::vector<bool>> layer1Draw(3, std::vector<bool>(3, true));
std::vector<std::vector<bool>> layer2Draw(3, std::vector<bool>(3, true));
std::vector<std::vector<bool>> layer3Draw(3, std::vector<bool>(3, true));

std::vector<std::vector<bool>> layer1Inc(3, std::vector<bool>(3, true));
std::vector<std::vector<bool>> layer2Inc(3, std::vector<bool>(3, true));
std::vector<std::vector<bool>> layer3Inc(3, std::vector<bool>(3, true));

void drawModular3x3Cube(std::vector<float> one, float length, int depth, std::vector<float> &vertices){
    float third = length/3;
    float layer1ZOffset = 0;
    float layer2ZOffset = third;
    float layer3ZOffset = third*2;
    if(depth<maxDepth) {
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer1Inc[row][col]) drawModular3x3Cube({one[0]+(third*row),one[1]-(third*col),one[2]+layer1ZOffset},length/3,depth+1,vertices);
            }
        }
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer2Inc[row][col]) drawModular3x3Cube({one[0]+(third*row),one[1]-(third*col),one[2]+layer2ZOffset},length/3,depth+1,vertices);
            }
        }
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer3Inc[row][col]) drawModular3x3Cube({one[0]+(third*row),one[1]-(third*col),one[2]+layer3ZOffset},length/3,depth+1,vertices);
            }
        }
    } else {
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer1Draw[row][col]) {
                    std::vector<float> newOne = {one[0]+(third*row),one[1]-(third*col),one[2]+layer1ZOffset};
                    std::vector<float> two = {newOne[0]+third, newOne[1], newOne[2]};
                    std::vector<float> three = {newOne[0], newOne[1]-third, newOne[2]};
                    std::vector<float> four = {newOne[0]+third, newOne[1]-third, newOne[2]};
                    std::vector<float> five = {newOne[0], newOne[1], newOne[2]+third};
                    std::vector<float> six = {newOne[0]+third, newOne[1], newOne[2]+third};
                    std::vector<float> seven = {newOne[0], newOne[1]-third, newOne[2]+third};
                    std::vector<float> eight = {newOne[0]+third, newOne[1]-third, newOne[2]+third};
                    drawSquare(newOne,two,three,four,color1,vertices);
                    drawSquare(six,five,eight,seven,color2,vertices);
                    drawSquare(five,six,newOne,two,color3,vertices);
                    drawSquare(eight,seven,four,three,color4,vertices);
                    drawSquare(two,six,four,eight,color5,vertices);
                    drawSquare(five,newOne,seven,three,color6,vertices);
                }
            }
        }
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer2Draw[row][col]) {
                    std::vector<float> newOne = {one[0] + (third * row), one[1] - (third * col), one[2] + layer2ZOffset};
                    std::vector<float> two = {newOne[0] + third, newOne[1], newOne[2]};
                    std::vector<float> three = {newOne[0], newOne[1] - third, newOne[2]};
                    std::vector<float> four = {newOne[0] + third, newOne[1] - third, newOne[2]};
                    std::vector<float> five = {newOne[0], newOne[1], newOne[2] + third};
                    std::vector<float> six = {newOne[0] + third, newOne[1], newOne[2] + third};
                    std::vector<float> seven = {newOne[0], newOne[1] - third, newOne[2] + third};
                    std::vector<float> eight = {newOne[0] + third, newOne[1] - third, newOne[2] + third};
                    drawSquare(newOne, two, three, four, color1, vertices);
                    drawSquare(six, five, eight, seven, color2, vertices);
                    drawSquare(five, six, newOne, two, color3, vertices);
                    drawSquare(eight, seven, four, three, color4, vertices);
                    drawSquare(two, six, four, eight, color5, vertices);
                    drawSquare(five, newOne, seven, three, color6, vertices);
                }
            }
        }
        for (int row = 0;row < 3;row++) {
            for (int col = 0;col < 3;col++) {
                if (layer3Draw[row][col]) {
                    std::vector<float> newOne = {one[0] + (third * row), one[1] - (third * col), one[2] + layer3ZOffset};
                    std::vector<float> two = {newOne[0] + third, newOne[1], newOne[2]};
                    std::vector<float> three = {newOne[0], newOne[1] - third, newOne[2]};
                    std::vector<float> four = {newOne[0] + third, newOne[1] - third, newOne[2]};
                    std::vector<float> five = {newOne[0], newOne[1], newOne[2] + third};
                    std::vector<float> six = {newOne[0] + third, newOne[1], newOne[2] + third};
                    std::vector<float> seven = {newOne[0], newOne[1] - third, newOne[2] + third};
                    std::vector<float> eight = {newOne[0] + third, newOne[1] - third, newOne[2] + third};
                    drawSquare(newOne, two, three, four, color1, vertices);
                    drawSquare(six, five, eight, seven, color2, vertices);
                    drawSquare(five, six, newOne, two, color3, vertices);
                    drawSquare(eight, seven, four, three, color4, vertices);
                    drawSquare(two, six, four, eight, color5, vertices);
                    drawSquare(five, newOne, seven, three, color6, vertices);
                }       
            }
        }
        // std::vector<float> two = {one[0]+length, one[1], one[2]};
        // std::vector<float> three = {one[0], one[1]-length, one[2]};
        // std::vector<float> four = {one[0]+length, one[1]-length, one[2]};
        // std::vector<float> five = {one[0], one[1], one[2]+length};
        // std::vector<float> six = {one[0]+length, one[1], one[2]+length};
        // std::vector<float> seven = {one[0], one[1]-length, one[2]+length};
        // std::vector<float> eight = {one[0]+length, one[1]-length, one[2]+length};

        // drawSquare(one,two,three,four,color1,vertices);
        // drawSquare(six,five,eight,seven,color2,vertices);
        // drawSquare(five,six,one,two,color3,vertices);
        // drawSquare(eight,seven,four,three,color4,vertices);
        // drawSquare(two,six,four,eight,color5,vertices);
        // drawSquare(five,one,seven,three,color6,vertices);
    }
}
std::vector<std::vector<std::vector<bool>>> layerInc(4, std::vector<std::vector<bool>>(4, std::vector<bool>(4, true)));
void drawModular4x4Cube(std::vector<float> one, float length, int depth, std::vector<float> &vertices){
    float fourth = length/4;
    if (depth < maxDepth){
        for (int layer = 0; layer < 4; ++layer)
        {
            float z = one[2] + (fourth * layer);
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    if (layerInc[layer][row][col])
                    {
                        drawModular4x4Cube({one[0] + (fourth * row), one[1] - (fourth * col), z}, length / 3, depth + 1, vertices);
                    }
                }
            }
        }
    } else {
        std::vector<float> two = {one[0]+length, one[1], one[2]};
        std::vector<float> three = {one[0], one[1]-length, one[2]};
        std::vector<float> four = {one[0]+length, one[1]-length, one[2]};
        std::vector<float> five = {one[0], one[1], one[2]+length};
        std::vector<float> six = {one[0]+length, one[1], one[2]+length};
        std::vector<float> seven = {one[0], one[1]-length, one[2]+length};
        std::vector<float> eight = {one[0]+length, one[1]-length, one[2]+length};

        drawSquare(one,two,three,four,color1,vertices);
        drawSquare(six,five,eight,seven,color2,vertices);
        drawSquare(five,six,one,two,color3,vertices);
        drawSquare(eight,seven,four,three,color4,vertices);
        drawSquare(two,six,four,eight,color5,vertices);
        drawSquare(five,one,seven,three,color6,vertices);
    }
}

void drawLSponge(std::vector<float> one, float length, int depth, std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        float fourth = length/4;
        float half = length/2;
        float threeQ = length * 3 / 4;

        drawLSponge(one, fourth, depth+2, vertices);
        drawLSponge({one[0]+fourth, one[1], one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0]+half, one[1], one[2]}, half, depth+1, vertices);
        drawLSponge({one[0], one[1]-fourth, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0], one[1]-half, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-half, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0], one[1]-threeQ, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0]+fourth, one[1]-threeQ, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0]+half, one[1]-threeQ, one[2]}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]}, fourth, depth+2, vertices);

        drawLSponge({one[0], one[1], one[2]+fourth}, fourth, depth+2, vertices);
        drawLSponge({one[0], one[1]-threeQ, one[2]+fourth}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+fourth}, fourth, depth+2, vertices);

        drawLSponge({one[0], one[1], one[2]+half}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1], one[2]+half}, fourth, depth+2, vertices);
        drawLSponge({one[0], one[1]-half, one[2]+half}, half, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+half}, fourth, depth+2, vertices);

        drawLSponge({one[0], one[1], one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+fourth, one[1], one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0], one[1]-fourth, one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+half, one[1], one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1], one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-fourth, one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-half, one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+half, one[1]-threeQ, one[2]+threeQ}, fourth, depth+2, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+threeQ}, fourth, depth+2, vertices);
    } else {
        std::vector<float> two = {one[0]+length, one[1], one[2]};
        std::vector<float> three = {one[0], one[1]-length, one[2]};
        std::vector<float> four = {one[0]+length, one[1]-length, one[2]};
        std::vector<float> five = {one[0], one[1], one[2]+length};
        std::vector<float> six = {one[0]+length, one[1], one[2]+length};
        std::vector<float> seven = {one[0], one[1]-length, one[2]+length};
        std::vector<float> eight = {one[0]+length, one[1]-length, one[2]+length};

        drawSquare(one,two,three,four,color1,vertices);
        drawSquare(six,five,eight,seven,color2,vertices);
        drawSquare(five,six,one,two,color3,vertices);
        drawSquare(eight,seven,four,three,color4,vertices);
        drawSquare(two,six,four,eight,color5,vertices);
        drawSquare(five,one,seven,three,color6,vertices);
    }
}

void drawLSpongeV2(std::vector<float> one, float length, int depth, std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        float fourth = length/4;
        float half = length/2;
        float threeQ = length * 3 / 4;

        drawLSponge(one, fourth, depth+1, vertices);
        drawLSponge({one[0]+fourth, one[1], one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0]+half, one[1], one[2]}, half, depth+1, vertices);
        drawLSponge({one[0], one[1]-fourth, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0], one[1]-half, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-half, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0], one[1]-threeQ, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0]+fourth, one[1]-threeQ, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0]+half, one[1]-threeQ, one[2]}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]}, fourth, depth+1, vertices);

        drawLSponge({one[0], one[1], one[2]+fourth}, fourth, depth+1, vertices);
        drawLSponge({one[0], one[1]-threeQ, one[2]+fourth}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+fourth}, fourth, depth+1, vertices);

        drawLSponge({one[0], one[1], one[2]+half}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1], one[2]+half}, fourth, depth+1, vertices);
        drawLSponge({one[0], one[1]-half, one[2]+half}, half, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+half}, fourth, depth+1, vertices);

        drawLSponge({one[0], one[1], one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+fourth, one[1], one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0], one[1]-fourth, one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+half, one[1], one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1], one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-fourth, one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-half, one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+half, one[1]-threeQ, one[2]+threeQ}, fourth, depth+1, vertices);
        drawLSponge({one[0]+threeQ, one[1]-threeQ, one[2]+threeQ}, fourth, depth+1, vertices);
    } else {
        std::vector<float> two = {one[0]+length, one[1], one[2]};
        std::vector<float> three = {one[0], one[1]-length, one[2]};
        std::vector<float> four = {one[0]+length, one[1]-length, one[2]};
        std::vector<float> five = {one[0], one[1], one[2]+length};
        std::vector<float> six = {one[0]+length, one[1], one[2]+length};
        std::vector<float> seven = {one[0], one[1]-length, one[2]+length};
        std::vector<float> eight = {one[0]+length, one[1]-length, one[2]+length};

        drawSquare(one,two,three,four,color1,vertices);
        drawSquare(six,five,eight,seven,color2,vertices);
        drawSquare(five,six,one,two,color3,vertices);
        drawSquare(eight,seven,four,three,color4,vertices);
        drawSquare(two,six,four,eight,color5,vertices);
        drawSquare(five,one,seven,three,color6,vertices);
    }
}

void drawSponge(std::vector<float> one, std::vector<float> two, std::vector<float> three, std::vector<float> four, 
            std::vector<float> five, std::vector<float> six, std::vector<float> seven, std::vector<float> eight, int depth, std::vector<float> &vertices)
{
    if (depth < maxDepth)
    {
        std::vector<std::vector<float>> cornerOne = {
        thirdSplit(one,one), thirdSplit(one,two), thirdSplit(one,three), thirdSplit(one,four), thirdSplit(one,five), thirdSplit(one,six), thirdSplit(one,seven), thirdSplit(one,eight)};
        
        std::vector<std::vector<float>> cornerTwo = {
        thirdSplit(two,one), thirdSplit(two,two), thirdSplit(two,three), thirdSplit(two,four), thirdSplit(two,five), thirdSplit(two,six), thirdSplit(two,seven), thirdSplit(two,eight)};
        
        std::vector<std::vector<float>> cornerThree = {
        thirdSplit(three,one), thirdSplit(three,two), thirdSplit(three,three), thirdSplit(three,four), thirdSplit(three,five), thirdSplit(three,six), thirdSplit(three,seven), thirdSplit(three,eight)};
        
        std::vector<std::vector<float>> cornerFour = {
        thirdSplit(four,one), thirdSplit(four,two), thirdSplit(four,three), thirdSplit(four,four), thirdSplit(four,five), thirdSplit(four,six), thirdSplit(four,seven), thirdSplit(four,eight)};
        
        std::vector<std::vector<float>> cornerFive = {
        thirdSplit(five,one), thirdSplit(five,two), thirdSplit(five,three), thirdSplit(five,four), thirdSplit(five,five), thirdSplit(five,six), thirdSplit(five,seven), thirdSplit(five,eight)};
        
        std::vector<std::vector<float>> cornerSix = {
        thirdSplit(six,one), thirdSplit(six,two), thirdSplit(six,three), thirdSplit(six,four), thirdSplit(six,five), thirdSplit(six,six), thirdSplit(six,seven), thirdSplit(six,eight)};
        
        std::vector<std::vector<float>> cornerSeven = {
        thirdSplit(seven,one), thirdSplit(seven,two), thirdSplit(seven,three), thirdSplit(seven,four), thirdSplit(seven,five), thirdSplit(seven,six), thirdSplit(seven,seven), thirdSplit(seven,eight)};
        
        std::vector<std::vector<float>> cornerEight = {
        thirdSplit(eight,one), thirdSplit(eight,two), thirdSplit(eight,three), thirdSplit(eight,four), thirdSplit(eight,five), thirdSplit(eight,six), thirdSplit(eight,seven), thirdSplit(eight,eight)};
        
        // corner cubes
        drawSponge(one,cornerOne[1],cornerOne[2],cornerOne[3],cornerOne[4],cornerOne[5],cornerOne[6],cornerOne[7],depth+1,vertices);
        drawSponge(cornerTwo[0],two,cornerTwo[2],cornerTwo[3],cornerTwo[4],cornerTwo[5],cornerTwo[6],cornerTwo[7],depth+1,vertices);
        drawSponge(cornerThree[0],cornerThree[1],three,cornerThree[3],cornerThree[4],cornerThree[5],cornerThree[6],cornerThree[7],depth+1,vertices);
        drawSponge(cornerFour[0],cornerFour[1],cornerFour[2],four,cornerFour[4],cornerFour[5],cornerFour[6],cornerFour[7],depth+1,vertices);
        drawSponge(cornerFive[0],cornerFive[1],cornerFive[2],cornerFive[3],five,cornerFive[5],cornerFive[6],cornerFive[7],depth+1,vertices);
        drawSponge(cornerSix[0],cornerSix[1],cornerSix[2],cornerSix[3],cornerSix[4],six,cornerSix[6],cornerSix[7],depth+1,vertices);
        drawSponge(cornerSeven[0],cornerSeven[1],cornerSeven[2],cornerSeven[3],cornerSeven[4],cornerSeven[5],seven,cornerSeven[7],depth+1,vertices);
        drawSponge(cornerEight[0],cornerEight[1],cornerEight[2],cornerEight[3],cornerEight[4],cornerEight[5],cornerEight[6],eight,depth+1,vertices);
        
        // edge cubes
        drawSponge(cornerOne[1],cornerTwo[0],cornerOne[3],cornerTwo[2],cornerOne[5],cornerTwo[4],cornerOne[7],cornerTwo[6],depth+1,vertices);
        drawSponge(cornerThree[1],cornerFour[0],cornerThree[3],cornerFour[2],cornerThree[5],cornerFour[4],cornerThree[7],cornerFour[6],depth+1,vertices);
        drawSponge(cornerFive[1],cornerSix[0],cornerFive[3],cornerSix[2],cornerFive[5],cornerSix[4],cornerFive[7],cornerSix[6],depth+1,vertices);
        drawSponge(cornerSeven[1],cornerEight[0],cornerSeven[3],cornerEight[2],cornerSeven[5],cornerEight[4],cornerSeven[7],cornerEight[6],depth+1,vertices);

        drawSponge(cornerOne[2],cornerOne[3],cornerThree[0],cornerThree[1],cornerOne[6],cornerOne[7],cornerThree[4],cornerThree[5],depth+1,vertices);
        drawSponge(cornerTwo[2],cornerTwo[3],cornerFour[0],cornerFour[1],cornerTwo[6],cornerTwo[7],cornerFour[4],cornerFour[5],depth+1,vertices);
        drawSponge(cornerFive[2],cornerFive[3],cornerSeven[0],cornerSeven[1],cornerFive[6],cornerFive[7],cornerSeven[4],cornerSeven[5],depth+1,vertices);
        drawSponge(cornerSix[2],cornerSix[3],cornerEight[0],cornerEight[1],cornerSix[6],cornerSix[7],cornerEight[4],cornerEight[5],depth+1,vertices);

        drawSponge(cornerOne[4],cornerOne[5],cornerOne[6],cornerOne[7],cornerFive[0],cornerFive[1],cornerFive[2],cornerFive[3],depth+1,vertices);
        drawSponge(cornerTwo[4],cornerTwo[5],cornerTwo[6],cornerTwo[7],cornerSix[0],cornerSix[1],cornerSix[2],cornerSix[3],depth+1,vertices);
        drawSponge(cornerThree[4],cornerThree[5],cornerThree[6],cornerThree[7],cornerSeven[0],cornerSeven[1],cornerSeven[2],cornerSeven[3],depth+1,vertices);
        drawSponge(cornerFour[4],cornerFour[5],cornerFour[6],cornerFour[7],cornerEight[0],cornerEight[1],cornerEight[2],cornerEight[3],depth+1,vertices);
    } else {
        drawSquare(one,two,three,four,color1,vertices);
        drawSquare(six,five,eight,seven,color2,vertices);
        drawSquare(five,six,one,two,color3,vertices);
        drawSquare(eight,seven,four,three,color4,vertices);
        drawSquare(two,six,four,eight,color5,vertices);
        drawSquare(five,one,seven,three,color6,vertices);
    }
}

bool drawTet1 = true;
bool drawTet2 = true;
bool drawTet3 = true;
bool drawTet4 = true;
bool drawOcta = true;

bool incTet1 = true;
bool incTet2 = true;
bool incTet3 = true;
bool incTet4 = true;

void drawModularTetrahedron(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> d, int depth, std::vector<float> &vertices) {
    std::vector<float> point1 = a;
    std::vector<float> point2 = midpoint(a, b);
    std::vector<float> point3 = midpoint(a, c);
    std::vector<float> point4 = midpoint(a, d);
    std::vector<float> point5 = b;
    std::vector<float> point6 = midpoint(b, c);
    std::vector<float> point7 = c;
    std::vector<float> point8 = midpoint(b, d);
    std::vector<float> point9 = midpoint(c, d);
    std::vector<float> point10 = d;
    if (depth < maxDepth) {
        if (incTet1){
            drawModularTetrahedron(point1,point2,point3,point4,depth+1,vertices);
        }
        if (incTet2){
            drawModularTetrahedron(point2,point5,point6,point8,depth+1,vertices);
        }
        if (incTet3){
            drawModularTetrahedron(point3,point6,point7,point9,depth+1,vertices);
        }
        if (incTet4){
            drawModularTetrahedron(point4,point8,point9,point10,depth+1,vertices);
        }
        if (drawOcta){
            drawTriangle(point2,point6,point3,vertices,color1);
            drawTriangle(point4,point9,point8,vertices,color1);
            drawTriangle(point4,point8,point2,vertices,color2);
            drawTriangle(point3,point6,point9,vertices,color2);
            drawTriangle(point3,point9,point4,vertices,color3);
            drawTriangle(point2,point8,point6,vertices,color3);
            drawTriangle(point4,point2,point3,vertices,color4);
            drawTriangle(point6,point8,point9,vertices,color4);
        }
    } else {
        if (drawTet1) {
            drawTriangle(point1,point2,point3,vertices,color1);
            drawTriangle(point1,point4,point2,vertices,color2);
            drawTriangle(point1,point3,point4,vertices,color3);
            drawTriangle(point2,point4,point3,vertices,color4);
        }
        if (drawTet2) {
            drawTriangle(point2,point5,point6,vertices,color1);
            drawTriangle(point2,point8,point5,vertices,color2);
            drawTriangle(point2,point6,point8,vertices,color3);
            drawTriangle(point5,point8,point6,vertices,color4);
        }
        if (drawTet3) {
            drawTriangle(point3,point6,point7,vertices,color1);
            drawTriangle(point3,point9,point6,vertices,color2);
            drawTriangle(point3,point7,point9,vertices,color3);
            drawTriangle(point6,point9,point7,vertices,color4);
        }
        if (drawTet4) {
            drawTriangle(point4,point8,point9,vertices,color1);
            drawTriangle(point4,point10,point8,vertices,color2);
            drawTriangle(point4,point9,point10,vertices,color3);
            drawTriangle(point8,point10,point9,vertices,color4);
        }
        if (drawOcta){
            drawTriangle(point2,point6,point3,vertices,color1);
            drawTriangle(point4,point9,point8,vertices,color1);
            drawTriangle(point4,point8,point2,vertices,color2);
            drawTriangle(point3,point6,point9,vertices,color2);
            drawTriangle(point3,point9,point4,vertices,color3);
            drawTriangle(point2,point8,point6,vertices,color3);
            drawTriangle(point4,point2,point3,vertices,color4);
            drawTriangle(point6,point8,point9,vertices,color4);
        }
    }
}

void drawK2D4(std::vector<float> a, std::vector<float> b, std::vector<float> c, std::vector<float> top, std::vector<float> bottom, int depth, std::vector<float> &vertices, 
            std::vector<float> f1, std::vector<float> f2, std::vector<float> f3,
            std::vector<float> b1, std::vector<float> b2, std::vector<float> b3) 
{
    if (depth < maxDepth)
    {
        std::vector<float> newT1 = split(a,b);
        std::vector<float> newT2 = split(b,a);
        std::vector<float> newBot2 = split(b,c);
        std::vector<float> newBot3 = split(c,b);
        std::vector<float> newT3 = split(c,a);
        std::vector<float> newBot1 = split(a,c);

        drawK2D4(a,bottom,top,newT1,newBot1,depth+1,vertices,b1,color4,f1,b3,color4,f3);
        drawK2D4(b,top,bottom,newT2,newBot2,depth+1,vertices,f1,color4,b1,b2,color4,f2);
        drawK2D4(c,bottom,top,newT3,newBot3,depth+1,vertices,b3,color4,f3,b2,color4,f2);
    } else {
        draw2D4(a,b,c,top,bottom,vertices,f1,f2,f3,b1,b2,b3);
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
            drawTriangle(a, mid2, mid1, vertices);
            drawTriangle(b, mid3, mid2, vertices);
            drawTriangle(c, mid1, mid3, vertices);
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
            drawTriangle(a, mid2, mid1, vertices);
            drawTriangle(b, mid3, mid2, vertices);
            drawTriangle(c, mid1, mid3, vertices);
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
            drawTriangle(a, mid2, mid1, vertices);
            drawTriangle(b, mid3, mid2, vertices);
            drawTriangle(c, mid1, mid3, vertices);
        }
        drawTriangle(mid1, mid2, mid3, vertices);
    }
    else
    {
        drawTriangle(a, b, c, vertices);
    }
};

// unconventional way to generate a 3D sierpinski but it works
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
    drawTriangle(rotated1, rotated3, rotated2, vertices);
    if (depth < maxDepth)
    {
        drawInverseST(a, mid1, mid3, depth + 1, vertices);
        drawInverseST(mid1, b, mid2, depth + 1, vertices);
        drawInverseST(mid3, mid2, c, depth + 1, vertices);
        drawInverseST(rotated1, rotated2, rotated3, depth + 1, vertices);
    }
}