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
#include <random>
#include <cmath>
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
 
#include <stb_image.h>
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
float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

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
	GLFWwindow* window = lyh_gl::helper::gl_init(width, height,"SSAO");

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
 

	lyh_gl::shading::Shader pbr_shader("PBR_c43-c44_Baisc.glsl");
	//load model and texture
	pbr_shader.use();
	pbr_shader.setInt("albedoMap", 0);
	pbr_shader.setInt("normalMap", 1);
	pbr_shader.setInt("metallicMap", 2);
	pbr_shader.setInt("roughnessMap", 3);
	pbr_shader.setInt("aoMap", 4);


	pbr_shader.setVec3("_albedo", 0.5f, 0.0f, 0.0f);
	pbr_shader.setFloat("_ao", 1.0f);
	pbr_shader.setInt("tex_mode", 1);
 //texture
	auto albedo = std::make_shared<lyh_gl::helper::gl_texture>("texture/pbr/rusted_iron/albedo.jpg", "async_joinable", GL_RGB, "albedoMap", false);
	auto normal = std::make_shared<lyh_gl::helper::gl_texture>("texture/pbr/rusted_iron/normal.jpg", "async_joinable", GL_RGB, "normalMap", false);
	auto metallic = std::make_shared<lyh_gl::helper::gl_texture>("texture/pbr/rusted_iron/metallic.jpg", "async_joinable", GL_RGB, "metallicMap", false);
	auto roughness = std::make_shared<lyh_gl::helper::gl_texture>("texture/pbr/rusted_iron/roughness.jpg", "async_joinable", GL_RGB, "roughnessMap", false);
	auto ao = std::make_shared<lyh_gl::helper::gl_texture>("texture/pbr/rusted_iron/ao.jpg", "async_joinable", GL_RGB, "aoMap", false);

	albedo->loading_thread_join_blocking();
	normal->loading_thread_join_blocking();
	metallic->loading_thread_join_blocking();
	roughness->loading_thread_join_blocking();
	ao->loading_thread_join_blocking();
//geometry

	unsigned int sphereVAO = 0;
	unsigned int indexCount;

	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (std::size_t i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}
 
	auto renderSphere = [&]()->void
	{
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	};
	// lights
	glm::vec3 lightPositions[] = {
		glm::vec3(0.0f, 0.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(150.0f, 150.0f, 150.0f),
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	//project
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
	pbr_shader.use();
	pbr_shader.setMat4("projection", projection);
 
	//set up ui
	lyh_gl::ui::set_up(window, glsl_version);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 16.0f);
	   
     
	float radius=  0.5;
	bool texture_mode = 1;
	int render_mode = 0;
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
			ImGui::SetNextWindowSize({ 200,150 });
			ImGui::Begin("Render Input");
			ImGui::Checkbox("With texture", &texture_mode);
			ImGui::RadioButton("Final", &render_mode, 0);
			ImGui::RadioButton("Diffuse Only", &render_mode, 1);
			ImGui::RadioButton("Gloss Only", &render_mode, 2);
			/*ImGui::RadioButton("With texture", &render_mode,1);
			ImGui::RadioButton("No texutre", &render_mode, 0);
			ImGui::SliderFloat("SSAO Radius", &radius, 0.0, 4.0); */
			/*
			ImGui::RadioButton("Normal", &render_output, 2);
			ImGui::RadioButton("Specular", &render_output, 3);
			ImGui::RadioButton("Position", &render_output,4);
			ImGui::SliderFloat("Position Range", &pos_range, 0.0, 5.0);*/
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
		pbr_shader.use();
		pbr_shader.setInt("tex_mode", (int)texture_mode);
		pbr_shader.setInt("render_mode", (int)render_mode);
		glm::mat4 view = camera.GetViewMatrix();
		pbr_shader.setMat4("view", view);
		pbr_shader.setVec3("camPos", camera.Position);
		//bind texture
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo->texture_id);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal->texture_id);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metallic->texture_id);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, roughness->texture_id);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, ao->texture_id);
		}
		glm::mat4 model = glm::mat4(1.0f);
		//setup light and draw light source as ball
		{
			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				newPos = lightPositions[i];
				pbr_shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
				pbr_shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				pbr_shader.setMat4("model", model);
				renderSphere();
			}
		}
		//draw sphere object
		{
			glm::mat4 model = glm::mat4(1.0f);
			for (int row = 0; row < nrRows; ++row)
			{
				pbr_shader.setFloat("_metalness", (float)row / (float)nrRows);
				for (int col = 0; col < nrColumns; ++col)
				{

					pbr_shader.setFloat("_roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(float)(col - (nrColumns / 2)) * spacing,
						(float)(row - (nrRows / 2)) * spacing,
						0.0f
					));
					pbr_shader.setMat4("model", model);
					renderSphere();
				}
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

 