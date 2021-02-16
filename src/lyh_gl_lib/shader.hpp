#ifndef SHADER_H
#define SHADER_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <GL/glew.h>   
#include <boost/filesystem.hpp>
#endif
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "gl_helper.hpp"
class Shader

{
public:
 
	unsigned int ID;

	Shader(const std::string& path) {
		std::string vs_src;
		std::string fs_src;
		if (!lyh::gl_helper::parseShader(path, vs_src, fs_src)) {
			std::cout << "parseShader error" << std::endl;
			return ;
		}
		ID = lyh::gl_helper::buildShaderProgram(vs_src, fs_src);
	};
	// use/activate the shader
	~Shader() {
		glDeleteProgram(ID);
	}
	void use() {
		glUseProgram(ID);
	};
	// utility uniform functions
	void setBool(const std::string& name, bool value) const {
	
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	};
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setFloat(const std::string& name, float value) const {
		{
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
	};

};

#endif