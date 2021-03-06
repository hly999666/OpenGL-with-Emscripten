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
#include <vector>
#include <map>
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
#ifndef GL_MESH_H
#include  "gl_mesh.hpp"
#endif
#include  "gl_ui.hpp"
 
#include  "Optr_ol.hpp"
 

#include <thread>
#include <future>
#include <mutex>
#include <chrono>

 


//main render loop
std::function<void()> render_loop;
void main_loop() { render_loop(); }
//ui callback
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

	  int width = 1280;
	  int height = 720;
	//for camera and UI
		//camera
	lyh_gl::helper::Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
	bool firstMouse = true;
	float movingSpeed = 1.0;
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const char* glsl_version = "#version 330 core";

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
     

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = lyh_gl::helper::gl_init(width, height,"anti aliasing");

	//set up UI
	{
		frame_buffer_resize = [&width, &height](GLFWwindow* window, int _width, int _height)->void {
			width = _width; height = _height;
			glViewport(0, 0, width, height);

		};
		glfwSetFramebufferSizeCallback(window, on_frame_buffer_resize);
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
		/*scroll_callback = [&](GLFWwindow* window, double xoffset, double yoffset)
		{
			camera.ProcessMouseScroll(yoffset);
		};
		glfwSetScrollCallback(window, on_scroll_callback);*/
		processInput = [&deltaTime, &camera, movingSpeed](GLFWwindow* _window)->void
		{
			using namespace lyh_gl::helper;
			if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(_window, true);
			}


			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
				//std::cout << "Press_W" << std::endl;
				camera.ProcessKeyboard(FORWARD, deltaTime* movingSpeed);
			}

			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
				//std::cout << "Press_S" << std::endl;
				camera.ProcessKeyboard(BACKWARD, deltaTime * movingSpeed);
			}

			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
				//std::cout << "Press_A" << std::endl;
				camera.ProcessKeyboard(LEFT, deltaTime * movingSpeed);
			}

			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
				//std::cout << "Press_D" << std::endl;
				camera.ProcessKeyboard(RIGHT, deltaTime * movingSpeed);
			}

		};
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

 
	//create shader

	lyh_gl::shading::Shader rock_shader("more_feature_c32_anti_aliasing.glsl");

	lyh_gl::shading::Shader screen_shader("more_feature_c32_anti_aliasing_post.glsl");

	//load model and texture
	auto meshes = lyh_gl::loadModelGLTF("rock/rock.gltf");
	auto tex_diffuse = std::make_shared<lyh_gl::helper::gl_texture>("model/rock/rock.jpg", "async_joinable", GL_RGB, "diffuse_map", false);
	tex_diffuse->loading_thread_join_blocking();
	meshes[0].textures.push_back(tex_diffuse);
 
	rock_shader.setTexUnit(*tex_diffuse);
 
    //create screen quad geometry
	unsigned int quadVAO, quadVBO;
	{
		float quadVertices[] = { 
		  // positions   // texCoords
		  -1.0f,  1.0f,  0.0f, 1.0f,
		  -1.0f, -1.0f,  0.0f, 0.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,

		  -1.0f,  1.0f,  0.0f, 1.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,
		   1.0f,  1.0f,  1.0f, 1.0f
		};
		
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
	//create framebuffer with msaa
	unsigned int framebuffer;	unsigned int textureColorBufferMultiSampled;		unsigned int rbo_msaa;
	{
		
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// create a multisampled color attachment texture
	
		glGenTextures(1, &textureColorBufferMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
		// create a (also multisampled) renderbuffer object for depth and stencil attachments

		glGenRenderbuffers(1, &rbo_msaa);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_msaa);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_msaa);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//create post_effect fbo
	unsigned int intermediateFBO;	unsigned int screenTexture; unsigned int rbo_normal;
	{
		
		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
		// create a color attachment texture
	
		glGenTextures(1, &screenTexture);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
		glGenRenderbuffers(1, &rbo_normal);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_normal);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_normal);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	screen_shader.use();
	screen_shader.setInt("screenTexture", 0);
//set up ui
	lyh_gl::ui::set_up(window, glsl_version);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 16.0f);
	  
	 
	int mode = 1;
 
	float exploding_dist = 0.0;
	float normal_length = 0.2;
	int render_count = 10000;
	int msaa = 1; int grayscale = 1;
	render_loop = [&] {
		//timing
		float currentFrame = glfwGetTime();
		glfwMakeContextCurrent(window);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

	    processInput(window);
	
		glEnable(GL_DEPTH_TEST);
 


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		 //imGui
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::PushFont(font_1);


			ImGui::SetNextWindowPos({ 32,50 });
			ImGui::SetNextWindowSize({ 300,150 });
			ImGui::Begin("Input");
			ImGui::RadioButton("msaa ON", &msaa,1);
			ImGui::RadioButton("msaa OFF", &msaa, 0);
			ImGui::RadioButton("grayscale ON", &grayscale, 1);
			ImGui::RadioButton("grayscale OFF", &grayscale, 0);
			ImGui::End();

			ImGui::PopFont();
			ImGui::Render();
		}
		if (msaa == 1) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
		}
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		rock_shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		rock_shader.setMat4("projection", projection);
		rock_shader.setMat4("view", view);
		rock_shader.setMat4("model", model); 
		meshes[0].Draw(rock_shader);
		glBindVertexArray(0);
		//blit msaa output to normal fbo
		if (msaa == 1) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		//render screen

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// draw Screen quad
		screen_shader.use();
		screen_shader.setInt("grayscale", grayscale);
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture); 
		glDrawArrays(GL_TRIANGLES, 0, 6);


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

		glfwPollEvents();
	};
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (!glfwWindowShouldClose(window))main_loop();
#endif

	 

	//cleanup glfw
	lyh_gl::ui::clear_up();
	lyh_gl::helper::gl_clear_up(window);

	return 0;
	
};

 