#ifndef GL_HELPER_H
#define GL_HELPER_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include<filesystem>
#else
#include <GL/glew.h>    
 #include <boost/filesystem.hpp>
#endif
#include <glfw/glfw3.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include "../../../externs/stb/stb_image.h"
#endif
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_AVX2  
#endif
#include  "../../../externs/glm/glm/glm.hpp"
#include "../../../externs/glm/glm/gtc/matrix_transform.hpp"

namespace  lyh_gl::helper {

		unsigned int buildShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
			unsigned int vertexShader;
			vertexShader = glCreateShader(GL_VERTEX_SHADER);
			auto vs_ptr = vertexShaderSource.c_str();
			glShaderSource(vertexShader, 1, &vs_ptr, NULL);
			glCompileShader(vertexShader);

			int success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
					infoLog << std::endl;
			}
			unsigned int fragmentShader;
			auto fs_ptr = fragmentShaderSource.c_str();
			fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fs_ptr, NULL);

			glCompileShader(fragmentShader);


			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
					infoLog << std::endl;
			}

			unsigned int shaderProgram;
			shaderProgram = glCreateProgram();

			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);

			glLinkProgram(shaderProgram);

			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::SHADERPROGRAM::LINKING_FAILED\n" <<
					infoLog << std::endl;
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return shaderProgram;
		};
		bool  parseShader(const std::string path, std::string& out_vs, std::string& out_fs) {
			
			std::ifstream fs; 
           #ifdef __EMSCRIPTEN__
			fs.open("res/shader/" + path);
            #else
			auto cwd = boost::filesystem::current_path();
			fs.open(cwd.string() + "/res/shader/" + path);
           #endif
	
			//std::ifstream fs; fs.open("/res/preload-file/shader/" + path);
			std::string now_line;
			std::stringstream ss_vs; std::stringstream ss_fs;
			char now_t = 'n';
			while (std::getline(fs, now_line)) {
				//std::cout << now_line << std::endl;
				if (now_line.find("//shader") != std::string::npos) {
					if (now_line.find("vertex") != std::string::npos) {
						now_t = 'v';
					}
					else if (now_line.find("fragment") != std::string::npos) {
						now_t = 'f';
					}
					continue;
				}
				if (now_t == 'v')ss_vs << now_line << std::endl;
				if (now_t == 'f') ss_fs << now_line << std::endl;
			}
			out_vs = ss_vs.str();
			out_fs = ss_fs.str();
			if (out_vs.size() == 0 || out_fs.size() == 0) return false;
			else return true;
		}
		void loadTexture_async(const std::string f_path, GLuint  texture, GLFWwindow* window,GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(f_path.c_str(), &width, &height, &nrChannels, 0);
			if (data)

			{
				 
			   glfwMakeContextCurrent(window);
              #ifdef __EMSCRIPTEN__
               #else
			   bool err = glewInit() != GLEW_OK;
			   if (err)std::cout << "GLEW_ERR" << std::endl;
              #endif
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, input_format, input_type, data);

				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//clear up

				 
				stbi_image_free(data);
				glBindTexture(GL_TEXTURE_2D, 0);
				glFinish();
				//glfwDestroyWindow(threadWin);
				//return texture;
			}
			else
			{
		 
			    glfwMakeContextCurrent(window);
               #ifdef __EMSCRIPTEN__
               #else
				bool err = glewInit() != GLEW_OK;
				if (err)std::cout << "GLEW_ERR" << std::endl;
                #endif 
				glDeleteTextures(1, &texture);
				glFinish();
				stbi_image_free(data);
				//glfwDestroyWindow(threadWin);
				std::cout << "Failed to load texture" << std::endl;
				//return 0;
			}
		}
		unsigned int loadTexture(const std::string& path, GLFWwindow* window,GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			std::string f_path = f_path;
			std::ifstream fs;
            #ifdef __EMSCRIPTEN__
			     f_path = "res/texture/" + path; 
            #else
			auto cwd = boost::filesystem::current_path();
			f_path= cwd.string() + "/res/texture/" + path;
            #endif
			//std::thread t1(loadTexture_async, f_path, texture, window, input_format, input_type);t1.detach();
			loadTexture_async(f_path, texture, window,input_format, input_type);
			return texture;
 
		
		
		}


};
#endif