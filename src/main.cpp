#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader_s.h"
#include "camera.h"

#include <iostream>
#include <thread>
#include <cmath>
#include <vector>

#include <nanogui/nanogui.h>
#include <nanogui/textbox.h>
#include <nanogui/button.h>
#include "MultiLineLabel.h"

#include "fractal_utils.h"

#include <fstream>
#include <sstream>

#include <mutex>
#include <atomic>

#include "nfd.h"

std::string loadTextFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

nanogui::Window* g_infoWindow = nullptr;
bool infoOpen = false;

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

//button position
float buttonX0 = 0.0f;
float buttonY0 = 0.0f;
float buttonX1 = 0.0f;
float buttonY1 = 0.0f;

bool overMascot = false;
bool hideUi = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

unsigned int initialMaxDepth = 6; // change this to either save or set fire to your computer

bool generateDone = true;
std::thread generateThread;
std::atomic<bool> generateCancel{false};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* window, unsigned int codepoint);

nanogui::Screen* g_screen = nullptr;
nanogui::IntBox<int>* depthBox = nullptr;
nanogui::Button* generateButton = nullptr;

int main()
{   
    glfwInit();
    puts(glfwGetVersionString());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "GLFW version: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "GLFW version string: " << glfwGetVersionString() << std::endl;
    std::cout << glfwGetVersionString() << std::endl;

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "fractal_viewer", NULL, NULL);
    camera.window = window;
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
    Shader buttonShader("../src/buttonShader.vs","../src/buttonShader.fs");

    std::vector<float> vertices;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//----------------------------------------------------------------------userinterface----------------------------------------------------------------------
    int type = 0;
    int renderedType = 0;
    std::vector<std::string> typeOptions = {"Split Koch", "Checkered Koch", "Pointy Koch", "3D Sierpinski", "3D Inverse Sierpinski", 
                                            "Koch Tetrahedron", "Menger Sponge", "Mandelbrot", "L-Sponge", "Build Your Own: 2x2 Cube",
                                            "Build Your Own: 3x3 Cube", "Build Your Own: 4x4 Cube", "Build Your Own: 5x5 Cube",
                                            "Jerusalem Cube", "Octahedron"};

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

    nanogui::Widget* layer7 = colors->createTab("7");
    layer7->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(layer7, "Color 7 (background)");
    nanogui::ColorWheel *colorWheel7 = new nanogui::ColorWheel(layer7);
    colorWheel7->setColor(nanogui::Color(color7[0], color7[1], color7[2], 1.0f));

    colors->setActiveTab(0);

    new nanogui::Label(mainWindow, "3D Parameters");
    nanogui::Widget *params = new nanogui::Widget(mainWindow);
    params->setLayout(new nanogui::GroupLayout());
    params->setFixedWidth(260);
    params->add<nanogui::Label>("Recursive Depth");

    nanogui::Widget *depthBoxContainer = new nanogui::Widget(params);
    depthBoxContainer->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                                        nanogui::Alignment::Middle, 0, 0));

    new nanogui::Widget(depthBoxContainer);

    depthBox = new nanogui::IntBox<int>(depthBoxContainer);
    depthBox->setValue(initialMaxDepth);
    depthBox->setEditable(true);
    depthBox->setMinValue(0);
    depthBox->setMaxValue(15);
    depthBox->setFormat("[0-9]*");
    depthBox->setFixedWidth(60);
    depthBox->setCallback([&](int value) {
        setMaxDepth(value);
    });

    nanogui::Widget *perspectiveBoxContainer = new nanogui::Widget(params);
    perspectiveBoxContainer->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                                        nanogui::Alignment::Middle, 0, 0));
    
    nanogui::CheckBox *perspectiveBox = new nanogui::CheckBox(params, "Orthographic");
    perspectiveBox->setChecked(false);

    nanogui::CheckBox *lightingBox = new nanogui::CheckBox(params, "Lighting");
    lightingBox->setChecked(true);

    std::vector<std::string> perspectiveOptions = {"None","Corner 1","Corner 2","Corner 3","Corner 4","Corner 5","Corner 6","Corner 7","Corner 8",};
    int perspective = 0;

    new nanogui::Label(params, "Preset Perspectives");
    auto *pCombo = new nanogui::ComboBox(params, perspectiveOptions);
    pCombo->setSelectedIndex(perspective);
    pCombo->setFixedWidth(150);


    bool ortho = false;

    // window for type 1 specific params
    nanogui::ref<nanogui::Window> type1Window = new nanogui::Window(&screen, "Split Koch Parameters");
    type1Window->setPosition(Eigen::Vector2i(10, 615));
    type1Window->setLayout(new nanogui::GroupLayout());
    type1Window->setSize(Eigen::Vector2i(270, 1000));

    //parameter for split koch thickness
    nanogui::Widget *splitKochParams = new nanogui::Widget(type1Window);
    splitKochParams->setLayout(new nanogui::GroupLayout());
    splitKochParams->setFixedWidth(260);
    splitKochParams->add<nanogui::Label>("Thickness (0-1)");

    nanogui::FloatBox<float> *thicknessBox = new nanogui::FloatBox<float>(splitKochParams);
    thicknessBox->setValue(1.0f);
    thicknessBox->setEditable(true);
    thicknessBox->setMinValue(0);
    thicknessBox->setMaxValue(1);
    thicknessBox->setFormat("0(\\.[0-9]*)?|1(\\.0*)?");
    thicknessBox->setFixedWidth(60);

    // window for type 9 specific parameters
    nanogui::ref<nanogui::Window> type9Window = new nanogui::Window(&screen, "L-Sponge Parameters");
    type9Window->setPosition(Eigen::Vector2i(10, 6150));
    type9Window->setLayout(new nanogui::GroupLayout());
    type9Window->setSize(Eigen::Vector2i(270, 1000));
    type9Window->setVisible(false);

    nanogui::Widget *lSpongeParams = new nanogui::Widget(type9Window);
    lSpongeParams->setLayout(new nanogui::GroupLayout());
    lSpongeParams->setFixedWidth(260);
    //lSpongeParams->add<nanogui::Label>("Experimental Recursive Sizing");

    nanogui::CheckBox *experimentalBox = new nanogui::CheckBox(lSpongeParams, "Experimental Recursive Sizing");
    experimentalBox->setChecked(false);
    
    // modular 3x3 cube params

    nanogui::ref<nanogui::Window> type10Window = new nanogui::Window(&screen, "B.Y.O. 2x2 Cube Controls: Include");
    type10Window->setPosition(Eigen::Vector2i(10, 615));
    type10Window->setLayout(new nanogui::GroupLayout());
    type10Window->setSize(Eigen::Vector2i(200, 1000));
    type10Window->setVisible(false);

    nanogui::Widget *modular2x2CubeParams = new nanogui::Widget(type10Window);
    modular2x2CubeParams->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal,2));
    modular2x2CubeParams->setFixedWidth(260);
    
    std::vector<std::vector<std::vector<nanogui::Button *>>> cubeButtons2(2, std::vector<std::vector<nanogui::Button *>>(2, std::vector<nanogui::Button *>(2, nullptr)));
    for (int layer = 0; layer < 2; ++layer)
    {
        for (int row = 0; row < 2; ++row)
        {
            for (int col = 0; col < 2; ++col)
            {
                if (row==0&&col==0){
                    modular2x2CubeParams->add<nanogui::Label>("Layer "+std::to_string(layer+1));
                    modular2x2CubeParams->add<nanogui::Label>("");
                }
                cubeButtons2[layer][row][col] = new nanogui::Button(modular2x2CubeParams, "  ");
                cubeButtons2[layer][row][col]->setFlags(1 << 2);
                cubeButtons2[layer][row][col]->setPushed(true);
            }
        }
    }

    nanogui::Button *clearButton2 = new nanogui::Button(type10Window, "Clear");
    clearButton2->setFixedWidth(80);
    clearButton2->setCallback([&type10Window, cubeButtons2](){
            for (int layer = 0; layer < 2; ++layer)
            {
                for (int row = 0; row < 2; ++row)
                {
                    for (int col = 0; col < 2; ++col)
                    {
                        cubeButtons2[layer][row][col]->setPushed(true);
                    }
                }
            }
    });
    
    // modular 3x3 cube params

    nanogui::ref<nanogui::Window> type11Window = new nanogui::Window(&screen, "B.Y.O. 3x3 Cube Controls: Include");
    type11Window->setPosition(Eigen::Vector2i(330, 10));
    type11Window->setLayout(new nanogui::GroupLayout());
    type11Window->setSize(Eigen::Vector2i(200, 1000));
    type11Window->setVisible(false);

    nanogui::Widget *modular3x3CubeParams = new nanogui::Widget(type11Window);
    modular3x3CubeParams->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal,3));
    modular3x3CubeParams->setFixedWidth(260);
    
    std::vector<std::vector<std::vector<nanogui::Button *>>> cubeButtons3(3, std::vector<std::vector<nanogui::Button *>>(3, std::vector<nanogui::Button *>(3, nullptr)));
    for (int layer = 0; layer < 3; ++layer)
    {
        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                if (row==0&&col==0){
                    modular3x3CubeParams->add<nanogui::Label>("Layer "+std::to_string(layer+1));
                    modular3x3CubeParams->add<nanogui::Label>("");
                    modular3x3CubeParams->add<nanogui::Label>("");
                }
                cubeButtons3[layer][row][col] = new nanogui::Button(modular3x3CubeParams, "  ");
                cubeButtons3[layer][row][col]->setFlags(1 << 2);
                cubeButtons3[layer][row][col]->setPushed(true);
            }
        }
    }

    nanogui::Button *clearButton3 = new nanogui::Button(type11Window, "Clear");
    clearButton3->setFixedWidth(80);
    clearButton3->setCallback([&type11Window, cubeButtons3](){
            for (int layer = 0; layer < 3; ++layer)
            {
                for (int row = 0; row < 3; ++row)
                {
                    for (int col = 0; col < 3; ++col)
                    {
                        cubeButtons3[layer][row][col]->setPushed(true);
                    }
                }
            }
    });

    //4x4 cube
    nanogui::ref<nanogui::Window> type12Window = new nanogui::Window(&screen, "B.Y.O. 4x4 Cube Controls: Include");
    type12Window->setPosition(Eigen::Vector2i(330, 10));
    type12Window->setLayout(new nanogui::GroupLayout());
    type12Window->setSize(Eigen::Vector2i(200, 1000));
    type12Window->setVisible(false);

    nanogui::Widget *modular4x4CubeParams = new nanogui::Widget(type12Window);
    modular4x4CubeParams->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal,4));
    modular4x4CubeParams->setFixedWidth(300);

    std::vector<std::vector<std::vector<nanogui::Button *>>> cubeButtons4(4, std::vector<std::vector<nanogui::Button *>>(4, std::vector<nanogui::Button *>(4, nullptr)));
    for (int layer = 0; layer < 4; ++layer)
    {
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                if (row==0&&col==0){
                    modular4x4CubeParams->add<nanogui::Label>("Layer "+std::to_string(layer+1));
                    modular4x4CubeParams->add<nanogui::Label>("");
                    modular4x4CubeParams->add<nanogui::Label>("");
                    modular4x4CubeParams->add<nanogui::Label>("");
                }
                cubeButtons4[layer][row][col] = new nanogui::Button(modular4x4CubeParams, "  ");
                cubeButtons4[layer][row][col]->setFlags(1 << 2);
                cubeButtons4[layer][row][col]->setPushed(true);
            }
        }
    }

    nanogui::Button *clearButton4 = new nanogui::Button(type12Window, "Clear");
    clearButton4->setFixedWidth(80);
    clearButton4->setCallback([&type12Window, cubeButtons4](){
            for (int layer = 0; layer < 4; ++layer)
            {
                for (int row = 0; row < 4; ++row)
                {
                    for (int col = 0; col < 4; ++col)
                    {
                        cubeButtons4[layer][row][col]->setPushed(true);
                    }
                }
            }
    });

    //5x5 cube
    nanogui::ref<nanogui::Window> type13Window = new nanogui::Window(&screen, "B.Y.O. 5x5 Cube Controls: Include");
    type13Window->setPosition(Eigen::Vector2i(330, 10));
    type13Window->setLayout(new nanogui::GroupLayout());
    type13Window->setSize(Eigen::Vector2i(200, 1000));
    type13Window->setVisible(false);

    nanogui::Widget *modular5x5CubeParams = new nanogui::Widget(type13Window);
    modular5x5CubeParams->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal,5));
    modular5x5CubeParams->setFixedWidth(300);

    std::vector<std::vector<std::vector<nanogui::Button *>>> cubeButtons5(5, std::vector<std::vector<nanogui::Button *>>(5, std::vector<nanogui::Button *>(5, nullptr)));
    for (int layer = 0; layer < 5; ++layer)
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if (row==0&&col==0){
                    modular5x5CubeParams->add<nanogui::Label>("Layer "+std::to_string(layer+1));
                    modular5x5CubeParams->add<nanogui::Label>("");
                    modular5x5CubeParams->add<nanogui::Label>("");
                    modular5x5CubeParams->add<nanogui::Label>("");
                    modular5x5CubeParams->add<nanogui::Label>("");
                }
                cubeButtons5[layer][row][col] = new nanogui::Button(modular5x5CubeParams, "  ");
                cubeButtons5[layer][row][col]->setFlags(1 << 2);
                cubeButtons5[layer][row][col]->setPushed(true);
            }
        }
    }

    nanogui::Button *clearButton5 = new nanogui::Button(type13Window, "Clear");
    clearButton5->setFixedWidth(80);
    clearButton5->setCallback([&type13Window, cubeButtons5](){
            for (int layer = 0; layer < 5; ++layer)
            {
                for (int row = 0; row < 5; ++row)
                {
                    for (int col = 0; col < 5; ++col)
                    {
                        cubeButtons5[layer][row][col]->setPushed(true);
                    }
                }
            }
    });

    nanogui::ref<nanogui::Window> infoWindow = new nanogui::Window(&screen, "Info");
    infoWindow->setPosition(Eigen::Vector2i(SCR_WIDTH - 350, 10));
    infoWindow->setLayout(new nanogui::GroupLayout());
    infoWindow->setFixedWidth(340);

    MultiLineLabel *infoBox = new MultiLineLabel(infoWindow,loadTextFile("../resources/info/type0.txt"));
    infoBox->setFixedSize(Eigen::Vector2i(320, 400));

    infoWindow->setVisible(false); 

    g_infoWindow = infoWindow.get();

    new nanogui::Widget(infoWindow);

    nanogui::Button *closeButton = new nanogui::Button(infoWindow, "Close");
    closeButton->setFixedWidth(80);
    closeButton->setCallback([&infoWindow](){
        infoWindow->setVisible(false); 
    });
//---------------------------------------------------------------------------------------------combocallback----------------------------------------------------------------------------
    combo->setCallback([&type, &vertices, params, infoBox, &type1Window, &type9Window, &type10Window, &type11Window, &type12Window, &type13Window](int idx){ 
        type = idx;
        if (type==6||type==8){
            depthBox->setValue(4);
        } else if (depthBox->value()>3&&(type==9||type==10||type==11||type==12||type==13)) {
            depthBox->setValue(3);
        }
        if (type==7){
            generateButton->setVisible(false);
            vertices = {
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            camera.flat = true;
        } else if (camera.flat=true) {
        generateButton->setVisible(true); 
        vertices.clear();
        }
        params->setVisible(type != 7);
        type1Window->setVisible(type == 0);
        type9Window->setVisible(type == 8);
        type10Window->setVisible(type == 9);
        type11Window->setVisible(type == 10);
        type12Window->setVisible(type == 11);
        type13Window->setVisible(type == 12);
        std::string filename = "../resources/info/type" + std::to_string(type) + ".txt";
        infoBox->setValue(loadTextFile(filename));
    });

    pCombo->setCallback([&perspective, pCombo](int idx)
                        {
    perspective = idx;
    if (perspective != 0) {
        // Set rotation state to preset
        struct CornerRotation { float angleY; float angleX; };
        CornerRotation rotations[9] = {
            {0.0f, 0.0f}, {45.0f, 35.26f}, {135.0f, 35.26f}, {135.0f, -35.26f},
            {45.0f, -35.26f}, {-45.0f, 35.26f}, {-135.0f, 35.26f},
            {-135.0f, -35.26f}, {-45.0f, -35.26f}
        };
        preRotX = rotations[perspective].angleY;
        preRotY = rotations[perspective].angleX;
        rotX = 0;
        rotY = 0;
        perspective = 0; // Switch to user control after setting
        pCombo->setSelectedIndex(0);
    } });

    nanogui::Widget *generateButtonContainer = new nanogui::Widget(mainWindow);
    generateButtonContainer->setLayout(new nanogui::BoxLayout(
        nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 0));

    new nanogui::Widget(generateButtonContainer);

    generateButton = new nanogui::Button(generateButtonContainer, "Generate");
    generateButton->setFixedWidth(240);

    nanogui::Button *testDialogButton = new nanogui::Button(generateButtonContainer, "Test Dialog");
    testDialogButton->setFixedWidth(110);
    testDialogButton->setCallback([&]() {
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("txt", NULL, &outPath);
        if (result == NFD_OKAY) {
            std::cout << "Selected file: " << outPath << std::endl;
            free(outPath);
        } else if (result == NFD_CANCEL) {
            std::cout << "Dialog cancelled" << std::endl;
        } else {
            std::cerr << "NFD error opening dialog" << std::endl;
        }
    });
    
    generateThread = std::thread();
//------------------------------------------------------------generatecallback----------------------------------------------------------
    generateButton->setCallback([&]()
                                { 
        if (generateThread.joinable()&&!generateDone){
            generateButton->setCaption("Generate");
            generateCancel.store(true);
            generateThread.join();
            return;
        }else if (generateThread.joinable()) generateThread.join();

        generateCancel.store(false);
        renderedType = type; 
        setMaxDepth(depthBox->value());
        generateDone = false;
        // generateButton->setEnabled(false);
        generateButton->setCaption("Bail");
        combo->setEnabled(false);
        vertices.clear();
        generateThread = std::thread([&]()
        {
        nanogui::Color c1 = colorWheel1->color();
        color1 = {c1.r(), c1.g(), c1.b()};
        nanogui::Color c2 = colorWheel2->color();
        color2 = {c2.r(), c2.g(), c2.b()};
        nanogui::Color c3 = colorWheel3->color();
        color3 = {c3.r(), c3.g(), c3.b()};
        nanogui::Color c4 = colorWheel4->color();
        color4 = {c4.r(), c4.g(), c4.b()};
        nanogui::Color c5 = colorWheel5->color();
        color5 = {c5.r(), c5.g(), c5.b()};
        nanogui::Color c6 = colorWheel6->color();
        color6 = {c6.r(), c6.g(), c6.b()};
        if (type==0){
            d4Top.at(2) = -(thicknessBox->value()/2);
            d4Bottom.at(2) = thicknessBox->value()/2;
            drawK2D4(eqTVertex1,eqTVertex2,eqTVertex3,d4Top,d4Bottom,0,vertices,color1,color2,color3,color1,color2,color3);
            camera.flat = false;
        } else if (type==1){
            drawKT2(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT2(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT2(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT2(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            camera.flat = false;
        } else if (type==2){
            drawKT3(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT3(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT3(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT3(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            camera.flat = false;
        }else if (type==3){
            drawST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            camera.flat = false;
        }else if (type==4){
            drawInverseST(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawInverseST(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawInverseST(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawInverseST(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            camera.flat = false;
        }else if (type==5){ // add option to not do some sides
            drawKT(f1vertex1, f1vertex2, f1vertex3, 0, vertices);
            drawKT(f2vertex1, f2vertex2, f2vertex3, 0, vertices);
            drawKT(f3vertex1, f3vertex2, f3vertex3, 0, vertices);
            drawKT(f4vertex1, f4vertex2, f4vertex3, 0, vertices);
            camera.flat = false;
        }else if (type==6){
            drawSponge(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==8){
            if (!experimentalBox->checked()) drawLSponge(cubeVert1,1,0,vertices);
            else drawLSpongeV2(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==9){
            for (int layer = 0; layer < 2; ++layer)
            {
                for (int row = 0; row < 2; ++row)
                {
                    for (int col = 0; col < 2; ++col)
                    {
                        layerInc2[abs(layer-1)][abs(row-1)][abs(col-1)] = !(cubeButtons2[layer][row][col]->pushed());
                    }
                }
            }
            drawModular2x2Cube(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==10){ // takes up way more lines than needed but i'm too lazy
            for (int layer = 0; layer < 3; ++layer)
            {
                for (int row = 0; row < 3; ++row)
                {
                    for (int col = 0; col < 3; ++col)
                    {
                        layerInc3[abs(layer-2)][abs(row-2)][abs(col-2)] = !(cubeButtons3[layer][row][col]->pushed());
                    }
                }
            }
            drawModular3x3Cube(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==11){
            for (int layer = 0; layer < 4; ++layer)
            {
                for (int row = 0; row < 4; ++row)
                {
                    for (int col = 0; col < 4; ++col)
                    {
                        layerInc4[abs(layer-3)][abs(row-3)][abs(col-3)] = !(cubeButtons4[layer][row][col]->pushed());
                    }
                }
            }
            drawModular4x4Cube(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==12){
            for (int layer = 0; layer < 5; ++layer)
            {
                for (int row = 0; row < 5; ++row)
                {
                    for (int col = 0; col < 5; ++col)
                    {
                        layerInc5[abs(layer-4)][abs(row-4)][abs(col-4)] = !(cubeButtons5[layer][row][col]->pushed());
                    }
                }
            }
            drawModular5x5Cube(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==13){
            drawJCube(cubeVert1,1,0,vertices);
            camera.flat = false;
        }else if (type==14) {
            drawHedron(octVert1,octVert2,octVert3,octVert4,octVert5,octVert6,0,vertices);
            camera.flat = false;
        }

        // generateButton->setEnabled(true);
        combo->setEnabled(true);
        generateButton->setCaption("Generate");
        generateDone = true; }); });

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

    float buttonVertices[] = {
        // positions        // texcoords
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    unsigned int buttonIndices[] = {0, 1, 2, 2, 3, 0};

    unsigned int buttonVAO, buttonVBO, buttonEBO;
    glGenVertexArrays(1, &buttonVAO);
    glGenBuffers(1, &buttonVBO);
    glGenBuffers(1, &buttonEBO);

    glBindVertexArray(buttonVAO);

    glBindBuffer(GL_ARRAY_BUFFER, buttonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buttonVertices), buttonVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buttonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(buttonIndices), buttonIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texcoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int texture1, texture2;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("../resources/textures/SirPinski1.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../resources/textures/SirPinski2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    nanogui::Color c7 = colorWheel7->color();
//------------------------------------------------------------------renderloop--------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        if (type != 7 ){
            verticesMutex.lock();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            verticesMutex.unlock();
        }else{
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        }
        if (generateDone && generateThread.joinable())
        {
            generateThread.join();
            renderedType = type;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        SCR_WIDTH = width;
        SCR_HEIGHT = height;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        c7 = colorWheel7->color();
        color7 = {c7.r(), c7.g(), c7.b()};
        glClearColor(color7[0], color7[1], color7[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if(type==7) {
            mandelShader.use();
            mandelShader.setInt("maxItr",200);
            //for possible user choice coloring
            //mandelShader.setVec3("color1", glm::vec3(color1[0], color1[1], color1[2]));
            //mandelShader.setVec3("color2", glm::vec3(color2[0], color2[1], color2[2]));
            //mandelShader.setVec3("color3", glm::vec3(color3[0], color3[1], color3[2]));

            float time = glfwGetTime();
            float cycles = 1.0f + time * 0.1f;
            mandelShader.setFloat("cycles", cycles);

            GLint zoomLoc = glGetUniformLocation(mandelShader.ID, "zoom");
            GLint offsetLoc = glGetUniformLocation(mandelShader.ID, "offset");
            glUniform1d(zoomLoc, camera.mandelbrotZoom);
            glUniform2d(offsetLoc, camera.mandelbrotOffset.x, camera.mandelbrotOffset.y);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
        } else {
            ourShader.use();
            ourShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
            ourShader.setVec3("lightPos", lightPos);
            ourShader.setVec3("viewPos", camera.Position); 
            ourShader.setBool("lightToggle",lightingBox->checked());
            ortho = perspectiveBox->checked();

            if (ortho){
                float orthoSize = 1.0f * (camera.Zoom/45.0f);
                float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
                glm::mat4 projection = glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, 0.1f, 100.0f);
                ourShader.setMat4("projection", projection);
            } else {
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                ourShader.setMat4("projection", projection);
            }

            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);

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

            float maxAngle = 90.0f;
            if (totalRotY > maxAngle)
                totalRotY = maxAngle;
            if (totalRotY < -maxAngle)
                totalRotY = -maxAngle;

            model = glm::rotate(model, glm::radians(totalRotY), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(totalRotX), glm::vec3(0.0f, 1.0f, 0.0f));

            ourShader.setMat4("model", model);
            if (vertices.size() >= 9) {
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
            }
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
                float maxAngle = 90.0f;
                if (totalRotY > maxAngle)
                    totalRotY = maxAngle;
                if (totalRotY < -maxAngle)
                    totalRotY = -maxAngle;
                rotatedModel = glm::rotate(rotatedModel, glm::radians(totalRotY), glm::vec3(1.0f, 0.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(totalRotX), glm::vec3(0.0f, 1.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                rotatedModel = glm::rotate(rotatedModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                ourShader.setMat4("model", rotatedModel);

                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
            }
        }
        if (!hideUi) {
            screen.drawContents();
            screen.drawWidgets();

            glDisable(GL_DEPTH_TEST);

            // Update button vertices for bottom right
            float buttonWidth = 300.0f, buttonHeight = 300.0f;
            float x0 = SCR_WIDTH - buttonWidth - 20.0f;
            float y0 = 20.0f;
            float x1 = SCR_WIDTH - 20.0f;
            float y1 = 20.0f + buttonHeight;
            buttonX0 = x0;
            buttonY0 = y0;
            buttonX1 = x1;
            buttonY1 = y1;
            float buttonVertices[] = {
                x0, y0, 0.0f, 0.0f,
                x1, y0, 1.0f, 0.0f,
                x1, y1, 1.0f, 1.0f,
                x0, y1, 0.0f, 1.0f};

            glBindBuffer(GL_ARRAY_BUFFER, buttonVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buttonVertices), buttonVertices);

            glm::mat4 ortho = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
            buttonShader.use();
            buttonShader.setMat4("projection", ortho);

            glBindVertexArray(buttonVAO);
            if (overMascot)
                glBindTexture(GL_TEXTURE_2D, texture2);
            else
                glBindTexture(GL_TEXTURE_2D, texture1);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
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
//---------------------------------------------------------------inputs------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (g_screen && g_screen->mouseButtonCallbackEvent(button, action, mods)) {
        return; // Event handled by NanoGUI
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isFirstDown = false;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if ((buttonX0 < float(xpos)) && (float(xpos) < buttonX1) &&
                ((SCR_HEIGHT - buttonY0) > float(ypos)) && (float(ypos) > (SCR_HEIGHT - buttonY1)))
            {
                if (g_infoWindow) {
                    g_infoWindow->setVisible(true);
                }
                return;
            }
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

            float maxAngle = 90.0f;
            if (preRotY > maxAngle) preRotY = maxAngle;
            if (preRotY < -maxAngle) preRotY = -maxAngle;

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
            float sensitivity = 0.15f;
            rotX = (xpos - orgX) * sensitivity;
            rotY = (ypos - orgY) * sensitivity;
        }
    }

    if ((buttonX0 < float(xpos))&&(float(xpos) < buttonX1)&&((SCR_HEIGHT - buttonY0) > float(ypos))&&(float(ypos) > (SCR_HEIGHT - buttonY1))) {
        //std::cout << "hovering over button" << std::endl;
        overMascot = true;
    } else {
        //std::cout << "not hovering over button" << std::endl;
        overMascot = false;
    }
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_screen && g_screen->scrollCallbackEvent(xoffset, yoffset)) {
        return;
    }
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

bool depthUp = false;
bool depthDown = false;
bool generatePress = false;
bool hideUiPress = false;

void processInput(GLFWwindow *window)
{   
    float cappedDeltaTime = std::min(deltaTime, 0.016f);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        camera.ProcessKeyboard(IN, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        camera.ProcessKeyboard(OUT, cappedDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        preRotX = 0;
        preRotY = 0;
        rotX = 0;
        rotY = 0;
        camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
    }if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!depthDown && depthBox)
        {
            int val = depthBox->value();
            if (val > 0)
                depthBox->setValue(val - 1);
        }
        depthDown = true;
    }if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
        depthDown = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!depthUp && depthBox)
        {
            int val = depthBox->value();
            if (val < 15)
                depthBox->setValue(val + 1);
        }
        depthUp = true;
    }if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
        depthUp = false;
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!generatePress){
            generateButton->callback()();
            generatePress = true;
        }
    }if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE){
        if (generatePress){
            generatePress = false;
        }
    }if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!hideUiPress)
        {   
            if (!hideUi) hideUi = true;
            else hideUi = false;
        }
        hideUiPress = true;
    }if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
        hideUiPress = false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_screen && g_screen->keyCallbackEvent(key, scancode, action, mods)) {
        return;
    }
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    if (!((codepoint >= '0' && codepoint <= '9') || codepoint == '.')) {
        return;
    }
    if (g_screen && g_screen->charCallbackEvent(codepoint)) {
        return;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}