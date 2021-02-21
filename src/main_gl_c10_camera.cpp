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
#include  "../externs/glm/glm/glm.hpp"
#include "../externs/glm/glm/gtc/matrix_transform.hpp" 
#include "../externs/glm/glm/gtc/type_ptr.hpp" 

#ifndef GL_HELPER_H
#include "gl_helper.hpp"
#include "gl_geometry.hpp"
#endif
#ifndef SHADER_H
#include  "shader.hpp"
#endif
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
std::function<void((GLFWwindow* window, double xpos, double ypos))> mouse_callback;
void on_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_callback(window, xpos, ypos);
};
std::function<void((GLFWwindow* window, double xoffset, double yoffset))> scroll_callback;
void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	scroll_callback(window, xoffset, yoffset);
};
std::function<void((GLFWwindow* window))> processInput;
 void thread_fun(int& count) {
	 while (true) {
		 if ((count) % 100000000 == 0) {
			 std::cout << "thread_count :" << count << std::endl;
		 }
		 count++;
		 //std::this_thread::sleep_for(std::chrono::milliseconds(200));
	 }
 }

int main()

{

	  int width = 800;
	  int height = 600;
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
	//init glfw
	glfwInit();
#ifdef __EMSCRIPTEN__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//init window
 
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	
	if (window == NULL)

	{

		std::cout << "Failed to create GLFW window" << std::endl;

		glfwTerminate();

		return -1;

	}
	glfwMakeContextCurrent(window);


#ifdef __EMSCRIPTEN__
#else
	bool err = glewInit() != GLEW_OK;
	if (GLEW_OK != err)
	{
	 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 0;
	}
	
#endif
	glfwSwapInterval(1);
	
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
	lyh_gl::Shader shader_1("basic_c10_Camera.glsl");

	//setup uniform
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	//setup geometry

	unsigned int VBO, VAO, EBO;

	lyh_gl::geometry::buildBox(VBO, VAO, EBO);

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	//auto tex_1 = 0;
  	lyh_gl::helper::gl_texture tex_1{ "uv-grid.png"};
	lyh_gl::helper::gl_texture tex_2{ "cat_2.jpg" };
	//set up texture in shader
	shader_1.setTexUnit(tex_1); 
	shader_1.setTexUnit(tex_2);

 


 
 
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
		shader_1.use();
		//set uniform 
		float timeValue = glfwGetTime();
		float _val = (sin(timeValue) / 2.0f) + 0.5f;
		
		shader_1.use();
		shader_1.setFloat("u_time", _val);

		 
		 tex_1.wait_for(1);
		 shader_1.bindTex("texture1",tex_1);
		 tex_2.wait_for(1);
		 shader_1.bindTex("texture2", tex_2);
	     
		 //set up uniform
		 glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		 shader_1.setMat4("projection", projection);
		 glm::mat4 view = camera.GetViewMatrix();
		 shader_1.setMat4("view", view);

		 glBindVertexArray(VAO);

		 for (unsigned int i = 0; i < 10; i++)
		 {
			 auto m_t = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			 float angle = 20.0f * i;
			 auto m_r = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			 auto model = m_t * m_r;
			 shader_1.setMat4("model", model);
			 glDrawArrays(GL_TRIANGLES, 0, 36);
		 }



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


	//cleanup glfw
	 
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;

};

 