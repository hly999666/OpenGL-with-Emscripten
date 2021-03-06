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
#include<cstdio>
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

class point2Di {
public:
    int x{ 0 };
    int y{ 0 };
    int id{ -1 };
    point2Di(int _x = 0, int _y = 0,int _id=-1) :x(_x), y(_y), id(_id){};
};
void convex_hull(std::vector<float>& point_ch_x,std::vector<float>& point_ch_y) {
    int count = 0;
    scanf("%d", &count);
    std::vector<point2Di>points; points.resize(count);
    for (int i = 0; i < count; i++) {
        int x = 0; int y = 0;
        scanf("%d %d", &x,&y);
        points[i].x = x;
        points[i].y = y;
        points[i].id = i+1;
    }
    int a = 666;

    auto comp_llt = [](const point2Di& v1, const point2Di& v2)->bool {
         
        if (v1.y> v2.y)return true;
        if (v1.y< v2.y)return false;
        if (v1.x> v2.x)return true;
        if (v1.x < v2.x)return false;
        return false;
    };

    auto llt_itr = std::min_element(points.begin(), points.end(), comp_llt);
    const auto const_llt = *llt_itr;
    auto back_itr = points.end() - 1;
    std::swap(*llt_itr, *back_itr);
    points.pop_back();

    auto area2 = [](const point2Di& p, const point2Di& q, const point2Di& s)->int {

        return p.x * q.y - p.y * q.x
            + q.x * s.y - q.y * s.x
            + s.x * p.y - s.y * p.x;
    };

    auto  to_left = [area2](const point2Di& p, const point2Di& q, const point2Di& s)->bool {

        return area2(p, q, s) >= 0;
    };
    auto comp_to_left_llt = [&const_llt, area2](const point2Di& v1, const point2Di& v2)->bool {
        //TODO Check conditional near equal,i.e three point in a line,will have problem with numerical precision
        auto area = area2(const_llt, v1, v2);
        if (area > 0.0)return false;
        if (area < 0.0)return true;
        return true;
    };

    std::sort(points.begin(), points.end(), [comp_to_left_llt](const point2Di& v1, const point2Di& v2)->bool {return !comp_to_left_llt(v1,v2); });
    std::vector<point2Di>current_result;
    current_result.push_back(const_llt); current_result.push_back(points.back());
    points.pop_back();
    while (!points.empty()) {
        auto back = points.back(); points.pop_back();
        while (to_left(*(current_result.end() - 2), *(current_result.end() - 1), back)) {
            current_result.pop_back();
        }
        current_result.push_back(back);
    }
    //output
    printf("%d   // ( %d", current_result.size()+1, current_result[0].id);
    for (int i = current_result.size() - 1; i >= 1;i--) {
        printf(" x %d", current_result[i].id);
    }
    printf(" ) %% (%d + 1)\n", count);
}
int main()
{
    const char* glsl_version = "#version 300 es";
    GLFWwindow* window = nullptr;
    {
        // glfw: initialize and configure
// ------------------------------
        glfwInit();

      
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        // glfw window creation
        // --------------------
          window = glfwCreateWindow(1920, 1080, "Fitting", NULL, NULL);
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
    }

    std::vector<float> point_ch_x;   std::vector<float> point_ch_y;
    convex_hull(point_ch_x, point_ch_y);




    auto shaderProgram = buildShader();

    unsigned int VBO, VAO;
    {
        float vertices[] = {
            // positions         // colors
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

        };

       
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }


    auto draw_tri = [&]() {
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);



        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray((GLuint)NULL);
        glUseProgram((GLuint)NULL);
    };
 
    lyh_gl::ui::set_up(window, glsl_version);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf",20.0f);
 
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    float x_data[6] = { 1,2,2,3,4,6 };
    float y_data[6] = { 1,2,2,3,4,6 };
    std::vector<float> point_x;   std::vector<float> point_y;
  
    std::vector<float>x_pos; std::vector<float>y_pos;
    float theta = 80.0f;    float s = 0.577;
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
        {

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
        }

        auto name = "Plot_windows";
        ImPlot::GetStyle().AntiAliasedLines = true;
        ImGui::Begin("Convex hull");
        ImPlot::SetNextPlotLimits(-10, 10, -10, 10, ImGuiCond_Always);
        int case_size =100;
        if (ImPlot::BeginPlot("Plot_1", "x", "y", { 1024,1024 })) {
            //x_pos.clear(); y_pos.clear();
            //DDoN::fractalTree2D_Basic(glm::radians(theta) * 0.5, s, x_pos, y_pos);
            //lyh_gl::gui::drawLineSegments(x_pos.data(), y_pos.data(), x_pos.size());

            //plot click point
            std::string type = "Point";
            if (ImGui::IsMouseClicked(1)) {
                /*auto pos = ImPlot::GetPlotMousePos();
              if (point_x.size() == case_size && point_y.size()== case_size) {
                point_x.clear(); point_y.clear();

            }
                point_x.push_back(pos.x); point_y.push_back(pos.y);

            }*/
                point_x.clear(); point_y.clear();
                point_x.push_back(7);  point_y.push_back(9);
                point_x.push_back(-8);  point_y.push_back(-1);
                point_x.push_back(-3);  point_y.push_back(-1);
                point_x.push_back(1);  point_y.push_back(4);
                point_x.push_back(-3);  point_y.push_back(9);
                point_x.push_back(6);  point_y.push_back(-4);
                point_x.push_back(7);  point_y.push_back(5);
                point_x.push_back(6);  point_y.push_back(6);
                point_x.push_back(-6);  point_y.push_back(10);
                point_x.push_back(0);  point_y.push_back(8);


                if (point_x.size() > 0 && point_y.size() > 0) {

                    ImPlot::PlotScatter(type.c_str(), point_x.data(), point_y.data(), point_x.size());
                    lyh_cg::convex_hull_Granham_Scan(point_x, point_y, point_ch_x, point_ch_y);
                    if (point_ch_x.size() > 0) {
                        ImPlot::PlotLine("Convex Hull", point_ch_x.data(), point_ch_y.data(), point_ch_x.size());
                    }
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