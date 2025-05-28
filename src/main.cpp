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

#include "fractal_utils.h"

// settings
unsigned int SCR_WIDTH = 1500;
unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(0.5f, 1.0f, 2.0f);

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

unsigned int initialMaxDepth = 6; // change this to either save or set fire to your computer

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

    Shader mandelShader("../src/mandelbrot.vs", "../src/mandelbrot.fs");

    std::vector<float> vertices;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    bool readyToDraw3D = false;
    bool readyToDraw2D = false;

    int type = 0;
    int renderedType = 0;
    std::vector<std::string> typeOptions = {"Split Koch", "Checkered Koch", "Pointy Koch", "3D Sierpinski", "3D Inverse Sierpinski", "Koch Tetrahedron", "Menger Sponge", "Mandelbrot"};

    nanogui::ref<nanogui::Window> mainWindow = new nanogui::Window(&screen, "Fractal Controls");
    mainWindow->setPosition(Eigen::Vector2i(10, 10));
    mainWindow->setLayout(new nanogui::GroupLayout());
    mainWindow->setSize(Eigen::Vector2i(270, 1000));

    new nanogui::Label(mainWindow, "Type");
    auto *combo = new nanogui::ComboBox(mainWindow, typeOptions);
    combo->setSelectedIndex(type);
    combo->setFixedWidth(230);

    new nanogui::Label(mainWindow, "Colors");
    nanogui::TabWidget *colors = new nanogui::TabWidget(mainWindow);
    
    nanogui::Widget* layer1 = colors->createTab("1");
    layer1->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer1, "Color 1");
    nanogui::ColorWheel *colorWheel1 = new nanogui::ColorWheel(layer1);
    colorWheel1->setColor(nanogui::Color(color1[0], color1[1], color1[2], 1.0f));
    colors->setFixedSize(Eigen::Vector2i(230, 175));

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

    nanogui::Widget* layer5 = colors->createTab("5");
    layer5->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer5, "Color 5");
    nanogui::ColorWheel *colorWheel5 = new nanogui::ColorWheel(layer5);
    colorWheel5->setColor(nanogui::Color(color5[0], color5[1], color5[2], 1.0f));

    nanogui::Widget* layer6 = colors->createTab("6");
    layer6->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer6, "Color 6");
    nanogui::ColorWheel *colorWheel6 = new nanogui::ColorWheel(layer6);
    colorWheel6->setColor(nanogui::Color(color6[0], color6[1], color6[2], 1.0f));

    colors->setActiveTab(0);

    new nanogui::Label(mainWindow, "Parameters");
    nanogui::Widget *params = new nanogui::Widget(mainWindow);
    params->setLayout(new nanogui::GroupLayout());
    params->setFixedWidth(260);
    params->add<nanogui::Label>("Recursive Depth");

    nanogui::Widget *depthBoxContainer = new nanogui::Widget(params);
    depthBoxContainer->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                                        nanogui::Alignment::Middle, 0, 0));

    new nanogui::Widget(depthBoxContainer);

    nanogui::IntBox<int> *depthBox = new nanogui::IntBox<int>(depthBoxContainer);
    depthBox->setValue(initialMaxDepth);
    depthBox->setEditable(true);
    depthBox->setMinValue(0);
    depthBox->setMaxValue(15);
    depthBox->setFixedWidth(60);

    combo->setCallback([&type, depthBox](int idx){ 
        type = idx;
        if (type==6){
            depthBox->setValue(4);
        }
    });

    nanogui::Widget *generateButtonContainer = new nanogui::Widget(mainWindow);
    generateButtonContainer->setLayout(new nanogui::BoxLayout(
        nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 0));

    new nanogui::Widget(generateButtonContainer);

    nanogui::Button *generateButton = new nanogui::Button(generateButtonContainer, "Generate");
    generateButton->setFixedWidth(240);

    generateButton->setCallback([&, depthBox]() {
        setMaxDepth(depthBox->value());
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
            drawK2D4(eqTVertex1,eqTVertex2,eqTVertex3,D4Top,D4Bottom,0,vertices,color1,color2,color3,color1,color2,color3);
            readyToDraw3D = true;
            readyToDraw2D = false;
        } else if (type==1){
            drawKT2(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT2(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT2(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT2(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        } else if (type==2){
            drawKT3(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT3(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT3(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT3(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        }else if (type==3){
            drawST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        }else if (type==4){
            drawInverseST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawInverseST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawInverseST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawInverseST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        }else if (type==5){
            drawKT(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        }else if (type==6){
            drawSponge(cubeVert1,cubeVert2,cubeVert3,cubeVert4,cubeVert5,cubeVert6,cubeVert7,cubeVert8,0,vertices);
            readyToDraw3D = true;
            readyToDraw2D = false;
        }else if (type==7){
            vertices = {
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            readyToDraw2D = true;
            readyToDraw3D = false;
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        renderedType = type;
    });

    screen.setVisible(true);
    screen.performLayout();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {   
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        SCR_WIDTH = width;
        SCR_HEIGHT = height;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (readyToDraw3D) {
            ourShader.use();
            ourShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
            ourShader.setVec3("lightPos", lightPos);
            ourShader.setVec3("viewPos", camera.Position); 

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
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);

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
                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
            }
        } else if(readyToDraw2D) {
            mandelShader.use();
            mandelShader.setInt("maxItr",200);
            mandelShader.setVec3("color1", glm::vec3(color1[0], color1[1], color1[2]));
            mandelShader.setVec3("color2", glm::vec3(color2[0], color2[1], color2[2]));
            mandelShader.setVec3("color3", glm::vec3(color3[0], color3[1], color3[2]));

            float time = glfwGetTime();
            float cycles = 1.0f + time * 0.5f;
            mandelShader.setFloat("cycles", cycles);

            GLint zoomLoc = glGetUniformLocation(mandelShader.ID, "zoom");
            GLint offsetLoc = glGetUniformLocation(mandelShader.ID, "offset");
            glUniform1d(zoomLoc, camera.mandelbrotZoom);
            glUniform2d(offsetLoc, camera.mandelbrotOffset.x, camera.mandelbrotOffset.y);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
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
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        float zoomStep = camera.mandelbrotZoom * 0.1f;
        camera.mandelbrotZoom -= 0.5f * zoomStep;
        if (camera.mandelbrotZoom > 10.0f)
            camera.mandelbrotZoom = 10.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        float zoomStep = camera.mandelbrotZoom * 0.1f;
        camera.mandelbrotZoom += 0.5f * zoomStep;
        if (camera.mandelbrotZoom > 10.0f)
            camera.mandelbrotZoom = 10.0f;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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