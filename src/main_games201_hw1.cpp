#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <GL/glew.h>     
#endif
#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <vector>
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX 
#else
#define GLM_FORCE_AVX2  
#endif
#include  "glm.hpp"
#include "gtc/matrix_transform.hpp" 
#include "gtc/type_ptr.hpp" 
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "implot.h"
#include "gl_ui.hpp"
#include "Plant2D.hpp"  
#include "comp_geo_common.hpp"
#include "convex_hull.hpp"
#include "hw1_data_fiting.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 300 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = 
     "#version 300 es\n"
      "precision highp float;\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

unsigned int buildShader() {
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
 
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
std::function<void()> loop;
void main_loop() { loop(); }
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();  
 
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Data Fitting", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
#ifdef __EMSCRIPTEN__
#else
    bool err = glewInit() != GLEW_OK;
    if (err) {
        std::cout << "GLEW_ERR" << std::endl;
    }
#endif

    auto shaderProgram = buildShader();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    auto draw_tri = [&]() {
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);



        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray((GLuint)NULL);
        glUseProgram((GLuint)NULL);
    };
   //IMGUI_CHECKVERSION();
    /*ImGui::CreateContext();
    ImPlot::CreateContext();


    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);*/
    lyh_gl::ui::set_up(window, glsl_version);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf",20.0f);
 
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    float x_data[6] = { 1,2,2,3,4,6 };
    float y_data[6] = { 1,2,2,3,4,6 };
    std::vector<float> point_x;   std::vector<float> point_y;
    std::vector<float> point_output_x;   std::vector<float> point_output_y;
    std::vector<float>x_pos; std::vector<float>y_pos;
       float s = 0.577;

    int sample_num = 64;
    float theta = 2;
    int highest_order =5;
    float riddge_lambda =4.0;
    loop = [&] {

      
        if (!processInput(window))return;


        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_tri();
      
        //draw gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(font_1);
        //ImGui::ShowDemoWindow();

        //ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //ImGui::Text("Hello from another window!");
        //if (ImGui::Button("Close Me"))
        //    show_another_window = false;
        //ImGui::End();


        //ImPlot::ShowDemoWindow();

        //ImGui::SetNextWindowPos({ 32,50 });
        //ImGui::SetNextWindowSize({ 256,128 });
        //ImGui::Begin("Control");
        //ImGui::SliderFloat("theta", &theta, 0, 180);
        //ImGui::SliderFloat("S", &s, 0, sqrt(2) * 0.5);
        //ImGui::End();
        //ImGui::SetNextWindowPos({ 320,50 });
        //ImGui::SetNextWindowSize({ 1024,1024 });
        ImGui::SetNextWindowPos({ 32,50 });
        ImGui::SetNextWindowSize({ 400,200 });
   
        ImGui::Begin("Control");
        ImGui::SliderInt("sample num", &sample_num, 16, 512);
 
        ImGui::SliderFloat("Gauss theta", &theta, 0, 16.0);
        ImGui::SliderInt("LSM highest order", &highest_order, 4, 16);
        ImGui::SliderFloat("Riddge_lambda", &riddge_lambda, 0.0,128.0);
        ImGui::End();
        auto name = "Plot_windows";
        ImPlot::GetStyle().AntiAliasedLines = true;
        ImGui::Begin("Data Fitting");
        //ImGui::SetNextWindowPos({ 512,50 });
        ImPlot::SetNextPlotLimits(-10, 10, -10, 10, ImGuiCond_Always);
        int case_size =100;
        if (ImPlot::BeginPlot("Plot_1", "x", "y", { 1024,1024 })) {
            //x_pos.clear(); y_pos.clear();
            //DDoN::fractalTree2D_Basic(glm::radians(theta) * 0.5, s, x_pos, y_pos);
            //lyh_gl::gui::drawLineSegments(x_pos.data(), y_pos.data(), x_pos.size());

            //plot click point
            std::string type = "Point";
            if (ImGui::IsMouseClicked(1)) {
                auto pos = ImPlot::GetPlotMousePos();
              if (point_x.size() == case_size && point_y.size()== case_size) {
                point_x.clear(); point_y.clear();

            }
                point_x.push_back(pos.x); point_y.push_back(pos.y);

            }
           
             
       
            if (point_x.size() > 0 && point_y.size() > 0) {
            
                ImPlot::PlotScatter(type.c_str(), point_x.data(), point_y.data(), point_x.size());
                int order = highest_order;
                if (point_x.size() > 2)gmp::hw1::lagrange_polynomial(point_x, point_y, point_output_x, point_output_y, sample_num);
                if (point_output_x.size() > 0) {
                    ImPlot::PlotLine("Lagrange", point_output_x.data(), point_output_y.data(), point_output_x.size());
                }
                if (point_x.size() > 2)gmp::hw1::gauss(point_x, point_y, point_output_x, point_output_y, sample_num, theta);
                if (point_output_x.size() > 0) {
                    ImPlot::PlotLine("Gauss", point_output_x.data(), point_output_y.data(), point_output_x.size());
                } 
                
              
                if (point_x.size() >2)gmp::hw1::LSM(point_x, point_y, point_output_x, point_output_y, sample_num, order);
                if (point_output_x.size() >2) {
                    ImPlot::PlotLine("LSM", point_output_x.data(), point_output_y.data(), point_output_x.size());
                } 

                if (point_x.size() > 2)gmp::hw1::ridge_regression(point_x, point_y, point_output_x, point_output_y, sample_num, order, riddge_lambda);
                if (point_output_x.size() > 2) {
                    ImPlot::PlotLine("ridge_regression", point_output_x.data(), point_output_y.data(), point_output_x.size());
                }
            
            }

       


            ImPlot::EndPlot();
        }
        ImGui::PopFont();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    };
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while (!glfwWindowShouldClose(window))main_loop();
        
#endif

    //clearup
  
    lyh_gl::ui::clear_up();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
bool processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
 
        glfwSetWindowShouldClose(window, true);
        return false;
    }
    return true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}