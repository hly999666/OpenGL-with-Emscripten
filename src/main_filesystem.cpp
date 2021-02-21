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
#ifndef GL_HELPER_H
#include "gl_helper.hpp"
#include "gl_geometry.hpp"
#endif
#ifndef SHADER_H
#include  "shader.hpp"
#endif
#include  "Optr_ol.hpp"

#include "../../../externs/glm/glm/gtc/type_ptr.hpp"

#include <thread>
#include <future>
#include <mutex>
#include <chrono>

std::thread t1;
void processInput(GLFWwindow* window)

{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}
const int width = 1920;
const int height = 1080;

std::function<void()> render_loop;
void main_loop() { render_loop(); }
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
	glfwMakeContextCurrent(window);
	if (window == NULL)

	{

		std::cout << "Failed to create GLFW window" << std::endl;

		glfwTerminate();

		return -1;

	}
	glfwMakeContextCurrent(window);
	//init with glad


#ifdef __EMSCRIPTEN__
#else
	bool err = glewInit() != GLEW_OK;
	if (err)std::cout << "GLEW_ERR" << std::endl;
#endif
	glfwSwapInterval(1);

	lyh_gl::Shader shader_1("basic_shader.glsl");

	//setup uniform
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	//setup geometry

	unsigned int VBO, VAO, EBO;

	lyh_gl::geometry::buildRect(VBO, VAO, EBO);
	//auto tex_1 = 0;
  	lyh_gl::helper::gl_texture tex_1{ "uv-grid.png"};
	lyh_gl::helper::gl_texture tex_2{ "cat_2.jpg" };
	//set up texture in shader
	shader_1.setTexUnit(tex_1); 
	shader_1.setTexUnit(tex_2);

 


	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});
	//int thread_count = 0;
 //   t1= std::thread(thread_fun, std::ref(thread_count)); t1.detach();

	char str_buf[128];
	float fval = 0;
	int frame_count = 0;
	render_loop = [&] {
		//if (frame_count % 100 == 0) {
		//	std::cout <<"render_loop :"<< frame_count << std::endl;
		//	std::cout << "render_loop : thread_count" << thread_count << std::endl;
		//}
		//frame_count++;
		glfwMakeContextCurrent(window);
		glfwPollEvents();
		processInput(window);


		//glUseProgram(shaderProgram);
	
		glBindVertexArray(VAO);
		//setup viewport
		glViewport(0, 0, width, height);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		//set uniform 
		float timeValue = glfwGetTime();
		float _val = (sin(timeValue) / 2.0f) + 0.5f;
		
		shader_1.use();
		shader_1.setFloat("u_time", _val);

		 
		 tex_1.wait_for(1);
		 shader_1.bindTex("texture1",tex_1);
		 tex_2.wait_for(1);
		 shader_1.bindTex("texture2", tex_2);
		//glActiveTexture(GL_TEXTURE0 + 0);
		//glBindTexture(GL_TEXTURE_2D, tex_1.texture_id);

		//glUniform1i(glGetUniformLocation(shader_1.ID, "texture1"), 0);
		//must unbind all before draw UI
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(NULL);
		glUseProgram(NULL);
		glfwSwapBuffers(window);
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