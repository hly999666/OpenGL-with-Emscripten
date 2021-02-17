#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
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
#include "../../externs/stb/stb_image.h"
#endif
#ifndef GL_HELPER_H
#define GL_HELPER_H

namespace  lyh {
	namespace  gl_helper {

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
			auto cwd = boost::filesystem::current_path();
			std::ifstream fs; fs.open(cwd.string()+path);
			std::string now_line;
			std::stringstream ss_vs; std::stringstream ss_fs;
			char now_t = 'n';
			while (std::getline(fs, now_line)) {
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
		unsigned int loadTexture(const std::string& path,GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			auto cwd = boost::filesystem::current_path();
			auto f_path = cwd.string() + path;
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(f_path.c_str(), &width, &height,&nrChannels, 0);
			if (data)

			{
				unsigned int texture;
		
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, input_format,input_type, data);

				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//clear up
			
				glBindTexture(GL_TEXTURE_2D, NULL);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
					GL_UNSIGNED_BYTE, data);

				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data);
				glBindTexture(GL_TEXTURE_2D, 0);
				return texture;
			}
			else
			{
				stbi_image_free(data);
				std::cout << "Failed to load texture" << std::endl;
				return 0;
			}
		
		}

		void buildRect(unsigned int& VBO, unsigned int&  VAO, unsigned int& EBO) {
			float vertices[] = {
				// positions          // colors           // texture coords
				 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
				 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
				-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
				-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
			};
			unsigned int indices[] = {
				0, 1, 3, // first triangle
				1, 2, 3  // second triangle
			};
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			// texture coord attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);


			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			glBindVertexArray(NULL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		}
	};
	
	

};
#endif