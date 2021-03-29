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
	lyh_gl::helper::Camera camera(glm::vec3(0.0f, 0.0f,5.0f));
	//camera.Pitch= 180.0f;
	//camera.updateCameraVectors();
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
	bool firstMouse = true;
	float movingSpeed = 1.0;
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const char* glsl_version = "#version 330 core";

 
     

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = lyh_gl::helper::gl_init(width, height,"Deferred Shading");

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
 


	lyh_gl::shading::Shader geometry_pass("more_lighting_c41_deferred_shading_geometry_pass.glsl");

	lyh_gl::shading::Shader light_cube("more_lighting_c41_deferred_shading_light_cube.glsl");

	lyh_gl::shading::Shader light_pass("more_lighting_c41_deferred_shading_light_pass.glsl");
	//load model and texture
 
	auto meshes = lyh_gl::loadModelGLTF("backpack/backpack.gltf");
	auto tex_diffuse = std::make_shared<lyh_gl::helper::gl_texture>("model/backpack/diffuse.jpg", "async_joinable", GL_RGB, "texture_diffuse1", false);
	auto tex_spec    = std::make_shared<lyh_gl::helper::gl_texture>("model/backpack/specular.jpg", "async_joinable", GL_RGB, "texture_specular1", false);
	tex_diffuse->loading_thread_join_blocking();
	meshes[0].textures.push_back(tex_diffuse);
	tex_spec->loading_thread_join_blocking();
	meshes[0].textures.push_back(tex_spec);

	geometry_pass.setTexUnit(*tex_diffuse);
	geometry_pass.setTexUnit(*tex_spec);

	std::vector<glm::vec3> objectPositions;
	{
		objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
		objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
		objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
		objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
		objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
		objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
		objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
		objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
		objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));
	}
	
	// configure g-buffer framebuffer
// ------------------------------
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	{
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		// position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		// color + specular color buffer
		glGenTextures(1, &gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		// create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
   

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	{
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; 
			float gColor = ((rand() % 100) / 200.0f) + 0.5; 
			float bColor = ((rand() % 100) / 200.0f) + 0.5;
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}
	}
 
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	auto renderCube = [&]()
	{
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	};
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	auto renderQuad = [&]()
	{
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	};
	//set up gBuffer for light pass
	light_pass.use();
	light_pass.setInt("gPosition", 0);
	light_pass.setInt("gNormal", 1);
	light_pass.setInt("gAlbedoSpec", 2);


//set up ui
	lyh_gl::ui::set_up(window, glsl_version);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 16.0f);
	   
    
	int  hdr_mode = 1;
	float exposure = 1.0f;
	float main_light_intense = 200.0;
	bool bloom = true;
	float bloom_threshold = 1.0;
	int blur_iteration = 5;
	float sample_radius = 1.0;

	int render_output = 0;
	float pos_range = 3.0;
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
	
		//moving light
		 
		 //imGui
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::PushFont(font_1);


			ImGui::SetNextWindowPos({ 32,50 });
			ImGui::SetNextWindowSize({ 400,200 });
			ImGui::Begin("Render Output");

			ImGui::RadioButton("Final Color", &render_output, 0);
			ImGui::RadioButton("Texture Color", &render_output, 1);
			ImGui::RadioButton("Normal", &render_output, 2);
			ImGui::RadioButton("Specular", &render_output, 3);
			ImGui::RadioButton("Position", &render_output,4);
			ImGui::SliderFloat("Position Range", &pos_range, 0.0, 5.0);
			/*ImGui::Checkbox("Bloom", &bloom);
			ImGui::SliderFloat("EV", &exposure, 0.0, 10.0);
			ImGui::SliderFloat("bloom threshold", &bloom_threshold, 0.0, 2.0);
			ImGui::SliderInt("blur iteration", &blur_iteration, 0.0, 20);
			ImGui::SliderFloat("sample radius", &sample_radius, 0.0,10.0);*/
		/*	ImGui::SliderFloat3("Light Position", &lightPos.x, -10.0, 10.0);
			ImGui::Text("Parallax Mode");
			ImGui::RadioButton("Basic",&parallax_mode,0);
			ImGui::RadioButton("Steep", &parallax_mode,1);
			ImGui::RadioButton("Occlusion", &parallax_mode, 2);
			ImGui::SliderFloat("Height Scale", &heightScale,0.0,1.0);*/
			
			//ImGui::SliderFloat("Normal Intense", &normal_intense,0.0, 2.0);
			/*ImGui::Checkbox("Texture Inpu,t Gamma", &input_gamma);
			ImGui::SliderFloat4("Light mixer", light_factor,0,2.0);*/
			ImGui::End();

			ImGui::PopFont();
			ImGui::Render();
		}
		// 1. geometry pass 
 			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 model = glm::mat4(1.0f);
			geometry_pass.use();
			geometry_pass.setMat4("projection", projection);
			geometry_pass.setMat4("view", view);
		/*	geometry_pass.setInt("texture_diffuse1", 1);
			geometry_pass.setInt("texture_specular1", 2);*/
			for (unsigned int i = 0; i < objectPositions.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, objectPositions[i]);
				model = glm::scale(model, glm::vec3(0.25f));
				model = glm::rotate(model,glm::radians(90.0f),glm::vec3(1.0,0.0,0.0));
				geometry_pass.setMat4("model", model);
				meshes[0].Draw(geometry_pass);
			}
			
		}

		// 2. lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			light_pass.use();
			light_pass.setFloat("pos_range", pos_range);
			light_pass.setInt("render_output", render_output);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
			// send light relevant uniforms
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				light_pass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				light_pass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
				// update attenuation parameters and calculate radius
				const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
				const float linear = 0.7;
				const float quadratic = 1.8;
				light_pass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
				light_pass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
				// then calculate radius of light volume/sphere
				const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
				float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
				light_pass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
			}
			light_pass.setVec3("viewPos", camera.Position);
			// finally render quad
			renderQuad();
		
		}

		//2.5 copy geometry pass's zbuffer to current framebuffer
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		//3. render lights  

		{
			light_cube.use();
			light_cube.setMat4("projection", projection);
			light_cube.setMat4("view", view);
			auto model = glm::mat4(1.0f);
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.125f));
				light_cube.setMat4("model", model);
				light_cube.setVec3("lightColor", lightColors[i]);
				renderCube();
			}

		}
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

 