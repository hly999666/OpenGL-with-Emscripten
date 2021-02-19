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
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_AVX2  
#endif
#include  "../externs/glm/glm/glm.hpp"
#include "../externs/glm/glm/gtc/matrix_transform.hpp"
#include "../externs/glm/glm/gtc/type_ptr.hpp"
namespace  lyh_gl {
	class Shader

	{
	public:

		unsigned int ID;
		unsigned int texture_used_id{ 0 };
		Shader(const std::string& path):texture_used_id(0){
			std::string vs_src;
			std::string fs_src;
			if (!helper::parseShader(path, vs_src, fs_src)) {
				std::cout << "parseShader error" << std::endl;
				return;
			}
			ID = helper::buildShaderProgram(vs_src, fs_src);
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
 	 
		void setMat4(const std::string& name,float* ptr) {
			unsigned int transformLoc = glGetUniformLocation(ID, name.c_str());
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, ptr);
		}
		void setMat4(const std::string& name, const glm::mat4& mat) {
			unsigned int transformLoc = glGetUniformLocation(ID, name.c_str());
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mat));
		}
		//TODO
		void setTexUnit(const std::string& name) {};
		void bindTex(const std::string& name, GLuint tex_id) {};
	};
}
#endif