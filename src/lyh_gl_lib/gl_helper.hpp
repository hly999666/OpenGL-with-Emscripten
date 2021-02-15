#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#include <boost/filesystem.hpp>
#endif
#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
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
		bool  parseShader(const std::string& path, std::string& out_vs, std::string& out_fs) {
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
	};
	
	

};
#endif