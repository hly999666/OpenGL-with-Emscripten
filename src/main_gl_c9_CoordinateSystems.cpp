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
#define GLM_FORCE_SIMD_AVX 
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
const int width = 800;
const int height = 600;

std::function<void()> render_loop;
void main_loop() { render_loop(); }
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
	GLFWwindow* window = glfwCreateWindow(width, height, "Test_1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, 
		[](GLFWwindow* window, int width, int height)
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

	lyh_gl::Shader shader_1("/res/shader/basic_c9_CoordinateSystems.glsl");

 

	unsigned int VBO, VAO, EBO;

	lyh_gl::geometry::buildBox(VBO, VAO, EBO);

	auto tex_1 = lyh_gl::helper::loadTexture("/res/texture/cat_2.jpg");
 
	glUniform1i(glGetUniformLocation(shader_1.ID, "texture1"), 0);

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





	char str_buf[128];
	float fval = 0;

	render_loop = [&] {
		glfwPollEvents();
		processInput(window);

		glViewport(0, 0, width, height);
				
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 
		//glUseProgram(shaderProgram);
		shader_1.use();
		//glBindVertexArray(VAO);
		//setup viewport


		//set uniform 
		/*float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		 glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		//shader_1.setTex("texture1", tex_1,0);
		//set transform
		// make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f); 
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		shader_1.setMat4("projection", projection);
		shader_1.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, tex_1);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
 
			auto m_t = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			float angle = 20.0f * i;
			auto m_r = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			auto model = m_t * m_r;
			shader_1.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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
	//cleanup gl
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//cleanup glfw

	std::function<void(int,int)> f = [](int x,int y)->void {};
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;

};