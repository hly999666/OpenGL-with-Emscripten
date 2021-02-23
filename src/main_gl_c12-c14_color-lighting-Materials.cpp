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
#include<string>
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX 
#else
#define GLM_FORCE_AVX2  
#endif
#include "glm.hpp"
#include "gtc/matrix_transform.hpp" 
#include "gtc/type_ptr.hpp" 

#ifndef GL_HELPER_H
#include "gl_helper.hpp"
#include "gl_geometry.hpp"
#endif
#ifndef SHADER_H
#include  "gl_shading.hpp"
#endif
 
#include  "gl_gui.hpp"
 
#include  "Optr_ol.hpp"
 

#include <thread>
#include <future>
#include <mutex>
#include <chrono>

 


//ui callback
std::function<void()> render_loop;
void main_loop() { render_loop(); }
std::function<void(GLFWwindow* window, int width, int height)>frame_buffer_resize;
void on_frame_buffer_resize(GLFWwindow* window, int width, int height) {
	frame_buffer_resize(window, width, height);
};
 
//std::function<void(GLFWwindow* window, int width, int height)> on_framebuffer_size_callback;
std::function<void(GLFWwindow* window, double xpos, double ypos)> mouse_callback;
void on_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_callback(window, xpos, ypos);
};
std::function<void(GLFWwindow* window, double xoffset, double yoffset)> scroll_callback;
void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	scroll_callback(window, xoffset, yoffset);
};
std::function<void(GLFWwindow* window)> processInput;
 
int main()

{

	  int width = 1920;
	  int height = 1080;
	//for camera and UI
		//camera
	lyh_gl::helper::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
	bool firstMouse = true;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const char* glsl_version = "#version 300 es";

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
 


	GLFWwindow* window = lyh_gl::helper::gl_init(width, height);

	//set up UI
	frame_buffer_resize = [&width, &height](GLFWwindow* window, int _width, int _height)->void {
		width = _width; height = _height;
		glViewport(0, 0, width, height);

	};
	glfwSetFramebufferSizeCallback(window,on_frame_buffer_resize);
	mouse_callback = [&](GLFWwindow* window, double xpos, double ypos)
	{

		
		 
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		if (state != GLFW_PRESS)return;
		camera.ProcessMouseMovement(xoffset, yoffset);
	};

 
	glfwSetCursorPosCallback(window, on_mouse_callback);

	scroll_callback = [&](GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);
	};
	glfwSetScrollCallback(window, on_scroll_callback);
	  processInput = [&deltaTime, &camera](GLFWwindow* _window)->void
	{
		using namespace lyh_gl::helper;
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(_window, true);
		}


		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			//std::cout << "Press_W" << std::endl;
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}
			
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			//std::cout << "Press_S" << std::endl;
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}
		
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			//std::cout << "Press_A" << std::endl;
			camera.ProcessKeyboard(LEFT, deltaTime);
		}
			
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			//std::cout << "Press_D" << std::endl;
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}
			
	};
	

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//create shader
	lyh_gl::shading::Shader shader_1("basic_c10_Camera.glsl");


	lyh_gl::shading::Shader lightingShader("lighting_c12-c14_basic_material.glsl");
	lyh_gl::shading::Shader lightCubeShader("lighting_c12-c14_light_cube.glsl");

	 

	unsigned int VBO, VAO, EBO;

	lyh_gl::geometry::buildBox(VBO, VAO, EBO);

	//glm::vec3 cubePositions[] = {
	//glm::vec3(0.0f,  0.0f,  0.0f),
	//glm::vec3(2.0f,  5.0f, -15.0f),
	//glm::vec3(-1.5f, -2.2f, -2.5f),
	//glm::vec3(-3.8f, -2.0f, -12.3f),
	//glm::vec3(2.4f, -0.4f, -3.5f),
	//glm::vec3(-1.7f,  3.0f, -7.5f),
	//glm::vec3(1.3f, -2.0f, -2.5f),
	//glm::vec3(1.5f,  2.0f, -2.5f),
	//glm::vec3(1.5f,  0.2f, -1.5f),
	//glm::vec3(-1.3f,  1.0f, -1.5f)
	//};
	////auto tex_1 = 0;
 // 	lyh_gl::helper::gl_texture tex_1{ "uv-grid.png"};
	//lyh_gl::helper::gl_texture tex_2{ "cat_2.jpg" };
	////set up texture in shader
	//shader_1.setTexUnit(tex_1); 
	//shader_1.setTexUnit(tex_2);

 
	lyh_gl::gui::set_up(window, glsl_version);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 20.0f);

 
	float col_diffuse[4] = { 0.5,0.5,0.5,1.0 };
	float col_light[4] = { 0.5,0.5,0.5,1.0 };
	render_loop = [&] {
		//timing
		float currentFrame = glfwGetTime();
		glfwMakeContextCurrent(window);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

	    processInput(window);
		 
	
		
		//setup viewport
		
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, width, height);


		//set up GUI
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont(font_1);


		ImGui::SetNextWindowPos({ 32,50 });
		ImGui::SetNextWindowSize({ 280,280 });
		ImGui::Begin("Input_diffuse_color");
		ImGui::ColorPicker4("diffuse_color", (float*)col_diffuse);
		 
		ImGui::End();
		ImGui::SetNextWindowPos({ 32+320,50 });
		ImGui::SetNextWindowSize({ 280,280 });
		ImGui::Begin("Input_light_color");
		ImGui::ColorPicker4("light_color", (float*)col_light);
		ImGui::End();

		ImGui::PopFont();
		ImGui::Render();

		shader_1.use();
		//set uniform 
		 
		//draw shaded cube
		lightingShader.use();

		//note using . to set struct 
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		glm::vec3 lightColor(col_light[0], col_light[1], col_light[2]);
	/*	lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);*/
	
		glm::vec3 diffuseColor = lightColor;
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);  
		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		auto diffuse_input = glm::vec3(col_diffuse[0], col_diffuse[1], col_diffuse[2]);
		lightingShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("material.diffuse", diffuse_input);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);  
		lightingShader.setFloat("material.shininess", 32.0f);


		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		auto normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

		lightingShader.setMat3("nomal_mat", normal_mat);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//draw light cube

		lightCubeShader.use();
		lightCubeShader.setVec4("_lightColor", glm::vec4(lightColor.r, lightColor.g, lightColor.b,1.0));
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));  
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

		glfwPollEvents();
	};
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (!glfwWindowShouldClose(window))main_loop();
#endif


	// Cleanup
	//cleanup imgui
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//cleanup glfw
	lyh_gl::gui::clear_up();
	lyh_gl::helper::gl_clear_up(window);

	return 0;

};

 