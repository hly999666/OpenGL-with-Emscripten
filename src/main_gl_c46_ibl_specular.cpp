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
 

#include <thread>
#include <future>
#include <mutex>
#include <chrono>

 
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

int main()

{

	  int win_width = 1280;
	  int win_height = 720;
	 
	//for camera and UI
	//camera
	lyh_gl::helper::Camera camera(glm::vec3(-1.0f, 0.0f,10.0f));
	//camera.Pitch= 180.0f;
	//camera.updateCameraVectors();
	float lastX = win_width / 2.0f;
	float lastY = win_height / 2.0f;
	bool firstMouse = true;
	float movingSpeed = 1.0;
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const char* glsl_version = "#version 330 core";

 
     

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = lyh_gl::helper::gl_init(win_width, win_height,"IBL_specular");

	//set up UI
	{
		frame_buffer_resize = [&win_width, &win_height](GLFWwindow* window, int _width, int _height)->void {
			win_width = _width; win_height = _height;
			glViewport(0, 0, win_width, win_height);

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


	// configure global opengl state
 
	glEnable(GL_DEPTH_TEST);
	// depth trick for skybox   
	glDepthFunc(GL_LEQUAL);
	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
 
	//create shader
 

 


	lyh_gl::shading::Shader pbrShader("c46_PBR_ibl_specular_pbr.glsl");
	lyh_gl::shading::Shader equirectangularToCubemapShader("c46_PBR_ibl_specular_equirectangular_to_cubemap.glsl");
	lyh_gl::shading::Shader irradianceShader("c46_PBR_ibl_specular_irradiance_convolution.glsl");
	lyh_gl::shading::Shader prefilterShader("c46_PBR_ibl_specular_prefilter.glsl");
	lyh_gl::shading::Shader brdfShader("c46_PBR_ibl_specular_brdf.glsl");
	lyh_gl::shading::Shader backgroundShader("c46_PBR_ibl_specular_background.glsl");
	//setup shader texture
	pbrShader.use();
	pbrShader.setInt("irradianceMap", 0);
	pbrShader.setInt("prefilterMap", 1);
	pbrShader.setInt("brdfLUT", 2);
	pbrShader.setInt("albedoMap", 3);
	pbrShader.setInt("normalMap", 4);
	pbrShader.setInt("metallicMap", 5);
	pbrShader.setInt("roughnessMap", 6);
	pbrShader.setInt("aoMap", 7);
 
	backgroundShader.use();
	backgroundShader.setInt("environmentMap", 0);

	//set up geometry
	 
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	{
		// initialize (if necessary)
 
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

	// render Cube
	auto  renderCube = [&]()->void
	{
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	};

	// renders a sphere
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
		for (unsigned int i = 0; i < positions.size(); ++i)
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


	// render Quad 
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
	auto renderQuad = [&]()->void
	{

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	};
	// load  material textures
// --------------------------
// rusted iron
	unsigned int ironAlbedoMap = loadTexture(std::string("./res/texture/pbr/rusted_iron/albedo.jpg").c_str());
	unsigned int ironNormalMap = loadTexture(std::string("./res/texture/pbr/rusted_iron/normal.jpg").c_str());
	unsigned int ironMetallicMap = loadTexture(std::string("./res/texture/pbr/rusted_iron/metallic.jpg").c_str());
	unsigned int ironRoughnessMap = loadTexture(std::string("./res/texture/pbr/rusted_iron/roughness.jpg").c_str());
	unsigned int ironAOMap = loadTexture(std::string("./res/texture/pbr/rusted_iron/ao.jpg").c_str());

	// gold
	unsigned int goldAlbedoMap = loadTexture(std::string("./res/texture/pbr/gold/albedo.jpg").c_str());
	unsigned int goldNormalMap = loadTexture(std::string("./res/texture/pbr/gold/normal.jpg").c_str());
	unsigned int goldMetallicMap = loadTexture(std::string("./res/texture/pbr/gold/metallic.jpg").c_str());
	unsigned int goldRoughnessMap = loadTexture(std::string("./res/texture/pbr/gold/roughness.jpg").c_str());
	unsigned int goldAOMap = loadTexture(std::string("./res/texture/pbr/gold/ao.jpg").c_str());

	// grass
	unsigned int grassAlbedoMap = loadTexture(std::string("./res/texture/pbr/grass/albedo.jpg").c_str());
	unsigned int grassNormalMap = loadTexture(std::string("./res/texture/pbr/grass/normal.jpg").c_str());
	unsigned int grassMetallicMap = loadTexture(std::string("./res/texture/pbr/grass/metallic.jpg").c_str());
	unsigned int grassRoughnessMap = loadTexture(std::string("./res/texture/pbr/grass/roughness.jpg").c_str());
	unsigned int grassAOMap = loadTexture(std::string("./res/texture/pbr/grass/ao.jpg").c_str());

	// plastic
	unsigned int plasticAlbedoMap = loadTexture(std::string("./res/texture/pbr/plastic/albedo.jpg").c_str());
	unsigned int plasticNormalMap = loadTexture(std::string("./res/texture/pbr/plastic/normal.jpg").c_str());
	unsigned int plasticMetallicMap = loadTexture(std::string("./res/texture/pbr/plastic/metallic.jpg").c_str());
	unsigned int plasticRoughnessMap = loadTexture(std::string("./res/texture/pbr/plastic/roughness.jpg").c_str());
	unsigned int plasticAOMap = loadTexture(std::string("./res/texture/pbr/plastic/ao.jpg").c_str());

	// wall
	unsigned int wallAlbedoMap = loadTexture(std::string("./res/texture/pbr/wall/albedo.jpg").c_str());
	unsigned int wallNormalMap = loadTexture(std::string("./res/texture/pbr/wall/normal.jpg").c_str());
	unsigned int wallMetallicMap = loadTexture(std::string("./res/texture/pbr/wall/metallic.jpg").c_str());
	unsigned int wallRoughnessMap = loadTexture(std::string("./res/texture/pbr/wall/roughness.jpg").c_str());
	unsigned int wallAOMap = loadTexture(std::string("./res/texture/pbr/wall/ao.jpg").c_str());

	// lights
   // ------
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	// pbr: setup framebuffer
// ----------------------
	unsigned int captureFBO;
	unsigned int captureRBO;
	{
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	}

	// pbr: load the HDR environment map
// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(std::string("./res/texture/newport_loft.hdr").c_str(), &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}
	//set up ui

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap;
	{
		glGenTextures(1, &envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	}

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
// pbr: convert HDR equirectangular environment map to cubemap equivalent
 // ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setInt("equirectangularMap", 0);
	equirectangularToCubemapShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	unsigned int irradianceMap;
	{
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	
	}

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
   // -----------------------------------------------------------------------------
	{
		irradianceShader.use();
		irradianceShader.setInt("environmentMap", 0);
		irradianceShader.setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// pbr: create a pre-filter cubemap binding
   // --------------------------------------------------------------------------------
	unsigned int prefilterMap;
	{
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
	}

	// pbr: run a quasi monte-carlo integrating cubemap.
	// ----------------------------------------------------------------------------------------------------
	{
		prefilterShader.use();
		prefilterShader.setInt("environmentMap", 0);
		prefilterShader.setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			//note roughness change over miplevel
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader.setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader.setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	// pbr: generate a 2D LUT from the BRDF equations used.
 // ----------------------------------------------------
	unsigned int brdfLUTTexture;
	{
		glGenTextures(1, &brdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		brdfShader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// initialize object shader uniforms before rendering
 
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100.0f);
	pbrShader.use();
	pbrShader.setMat4("projection", projection);
	backgroundShader.use();
	backgroundShader.setMat4("projection", projection);

	//  configure the viewport 
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);


	lyh_gl::ui::set_up(window, glsl_version);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 16.0f);
	   
     
	bool background_is_blur = false;
	bool IBL_Diffuse= true;
	bool IBL_Specular = true;
	int background_mode = 1;
	bool brdf_prefilter = false;
	render_loop = [&] {
		//timing
		float currentFrame = glfwGetTime();
		glfwMakeContextCurrent(window);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glEnable(GL_DEPTH_TEST);
		//for sky box trick
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		 //imGui
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::PushFont(font_1);


			ImGui::SetNextWindowPos({ 32,50 });
			ImGui::SetNextWindowSize({ 240,210 });
			ImGui::Begin("Render Input");
			 
			ImGui::Checkbox("Using IBL diffuse", &IBL_Diffuse);
			ImGui::Checkbox("Using IBL specular", &IBL_Specular);
			ImGui::Checkbox("Show BRDF prefilter", &brdf_prefilter);
			ImGui::Text("Background_mode");
			ImGui::RadioButton("skybox", &background_mode,1);
			ImGui::RadioButton("diffuse irradiance map", &background_mode, 2);
			ImGui::RadioButton("specular prefilter map", &background_mode, 3);
		
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
		//  view matrix setup
		pbrShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		pbrShader.setMat4("view", view);
		pbrShader.setVec3("camPos", camera.Position);
		pbrShader.setInt("IBL_Diffuse",int(IBL_Diffuse));
		pbrShader.setInt("IBL_Specular", int(IBL_Specular));
		// bind pre-computed IBL data
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture); 
		}

		// set up light and render light position as a sphere
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader.setMat4("model", model);
			renderSphere();
		}
		// rusted iron
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, ironAlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, ironNormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, ironMetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, ironRoughnessMap);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, ironAOMap);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
			pbrShader.setMat4("model", model);
			renderSphere();

		}
		// gold
		{
		
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, goldNormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, goldAOMap);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
			pbrShader.setMat4("model", model);
			renderSphere();

		
		}

		// grass
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, grassAlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, grassNormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, grassMetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, grassRoughnessMap);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, grassAOMap);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
			pbrShader.setMat4("model", model);
			renderSphere();
		}
		// plastic
		{
		
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, plasticAlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, plasticNormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, plasticMetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, plasticRoughnessMap);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, plasticAOMap);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
			pbrShader.setMat4("model", model);
			renderSphere();
		
		}

		// wall
		{
		
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, wallAlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, wallNormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, wallMetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, wallRoughnessMap);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, wallAOMap);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
			pbrShader.setMat4("model", model);
			renderSphere();
		}

		// render skybox (render as last to prevent overdraw)
		{
			backgroundShader.use();

			backgroundShader.setMat4("view", view);
			glActiveTexture(GL_TEXTURE0);
			if (background_mode == 1) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			}
			else if (background_mode == 2) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
			}
			else if (background_mode == 3) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			}

			renderCube();
		
		}
		 

		if (brdf_prefilter) {
			// render BRDF map to screen
			brdfShader.use();
			renderQuad();

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

 