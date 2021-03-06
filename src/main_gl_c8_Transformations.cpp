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
#endif
#ifndef GL_GEOMETRY_H
#include "gl_geometry.hpp"
#endif
#ifndef SHADER_H
#include  "shader.hpp"
#endif
#include  "Optr_ol.hpp"
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SSE3 
#else
#define GLM_FORCE_AVX2  
#endif
#include  "../externs/glm/glm/glm.hpp"
#include "../externs/glm/glm/gtc/matrix_transform.hpp" 
#include "../externs/glm/glm/gtc/type_ptr.hpp"
//#include  <glm/glm.hpp>
//#include  <glm/gtc/matrix_transform.hpp>
//#include  <glm/gtc/type_ptr.hpp>
void processInput(GLFWwindow* window)

{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}
const int width = 1024;
const int height = 1024;

std::function<void()> render_loop;
void main_loop() { render_loop(); }
int main()

{
	glm::vec4 v1(1, 2, 3, 4);
	glm::vec4 v2(1, 2, 3, 4);
	glm::mat4 mat1(1.0f);
	auto v3 =(v1+v2);
	std::cout << v3.x << std::endl;
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
	GLFWwindow* window = glfwCreateWindow(width, height, "Test_1", NULL, NULL);
	if (window == NULL)

	{

		std::cout << "Failed to create GLFW window" << std::endl;

		glfwTerminate();

		return -1;

	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
 
		glViewport(0, 0, width, height);
	});
 

#ifdef __EMSCRIPTEN__
#else
	bool err = glewInit() != GLEW_OK;
	if (err) {
		std::cout << "GLEW_ERR" << std::endl;
	}
#endif
	glfwSwapInterval(1);

	lyh_gl::Shader shader_1("/res/shader/basic_c8_Transformations.glsl");

	//setup uniform
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	//set up uniform 
	unsigned int transformLoc = glGetUniformLocation(shader_1.ID, "transform");

	//setup geometry

	unsigned int VBO, VAO, EBO;

	lyh_gl::geometry::buildRect(VBO, VAO, EBO);

	auto tex_1 = lyh_gl::helper::loadTexture("/res/texture/cat_2.jpg");
	//set up texture






	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});


	char str_buf[128];
	float fval = 0;

	render_loop = [&] {
		glfwPollEvents();
		processInput(window);


		//glUseProgram(shaderProgram);
		shader_1.use();
		glBindVertexArray(VAO);
		//setup viewport
		glViewport(0, 0, width, height);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		//set uniform 
		/*float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		 glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		unsigned int tex_index = 0;
		glActiveTexture(GL_TEXTURE0 + tex_index);
		glBindTexture(GL_TEXTURE_2D, tex_1);

		glUniform1i(glGetUniformLocation(shader_1.ID, "texture1"), tex_index);
		//set transform
		 
		auto m_t = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
		auto m_r = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		auto transform = m_t * m_r;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//must unbind all before draw UI
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