
 
#include <GL/glew.h>      
 
#include<imgui.h>
#include<imgui_impl_opengl3.h>
#include<imgui_impl_glfw.h>
#include<implot.h>
#include <stdio.h>

#include <glfw/glfw3.h>
#ifndef GL_HELPER_H
#include "./lyh_gl_lib/gl_helper.hpp"

#endif
 

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
 
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
 
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);   
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            
 
  
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Dear ImGui OpenGL3+GLFW example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    bool err = glewInit() != GLEW_OK;
 

 
    //my code 
    std::string vs_src;
    std::string fs_src;
    if (!lyh::gl_helper::parseShader("/res/shader/basic_tri_1.glsl", vs_src, fs_src)) {
        std::cout << "parseShader error" << std::endl;
        return 0;
    }

    unsigned int shaderProgram
        = lyh::gl_helper::buildShaderProgram(vs_src, fs_src);
    //setup geometry
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f, 0.5f, 0.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    auto draw_tri = [&]() {
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);



        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(NULL);
        glUseProgram(NULL);
    };

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
 
      
    ImGuiIO& io = ImGui::GetIO(); (void)io;
 
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
      
        glfwPollEvents();

    
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

      
   
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
     
            float x_data[10] = {1,2,3,4,5,6,7,8,9,10};
            float y_data[10] = { 1,2,3,4,5,6,7,8,9,10 };

            ImGui::Begin("Plot_windows");
            if (ImPlot::BeginPlot("Plot_1")) {
          
                ImPlot::PlotLine("Line_Plot_1", x_data, y_data, 10);
                 ImPlot::EndPlot();
            }
            ImGui::End();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_tri();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}