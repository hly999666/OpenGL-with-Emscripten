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
#include "./lyh_gl_lib/gl_helper.hpp"
#endif
#ifndef SHADER_H
#include "./lyh_gl_lib/shader.hpp"
#endif
void processInput(GLFWwindow* window)

{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}
const int width = 1920;
const int height = 1080;
 
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
	//init with glad
 

#ifdef __EMSCRIPTEN__
#else
	bool err = glewInit() != GLEW_OK;
	if (err) {
		std::cout << "GLEW_ERR" << std::endl;
	}
#endif
	glfwSwapInterval(1);

	Shader shader_1("/res/shader/basic_c6.glsl");

	//setup uniform
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	//setup geometry
	float vertices[] = {
		// positions 0-2 // colors 3-5
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top

	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(float),(void*)0);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(float),(void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(1);

	



	 

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



		glDrawArrays(GL_TRIANGLES, 0, 3);
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

}