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
#include <unordered_map>
#include "gl_helper.hpp"
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_AVX2  
#endif
#include "glm.hpp"
#include "gtc/matrix_transform.hpp" 
#include "gtc/type_ptr.hpp" 
namespace  lyh_gl {
	namespace shading {
		class Shader

		{
		public:
			bool isLoaded{ false };
			GLuint ID;
			GLuint texture_used_id{ 0 };
			std::unordered_map<std::string, GLuint>tex_unit_table;
			Shader(const std::string& path) :texture_used_id(0), isLoaded(false) {
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
			//TODO
			void cacheUniformLocation() {};
			inline void use() {
				glUseProgram(ID);
			};
			// utility uniform functions
			inline void setBool(const std::string& name, bool value) const {
				auto loc = glGetUniformLocation(ID, name.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name << std::endl; return;
				}
				glUniform1i(loc, (int)value);
			};
			inline void setInt(const std::string& name, int value) const {
				auto loc = glGetUniformLocation(ID, name.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name << std::endl; return;
				}
				glUniform1i(loc, value);
			};
			inline void setFloat(const std::string& name, float value) const {
				{

					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform1f(loc, value);
				}
			};
			inline void setVec3(const std::string& name, float x, float y, float z) const {
				{
					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform3f(loc,x,y,z);
				}
			};
			inline void setVec2(const std::string& name, float x, float y) const {
				{
					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform2f(loc, x, y);
				}
			};
			inline void setVec4(const std::string& name, float x, float y, float z,float w) const {
				{
					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform4f(loc, x, y, z,w);
				}
			};
			inline void setVec3(const std::string& name, const glm::vec3& v) const {
				{
					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform3fv(loc,1, glm::value_ptr(v));
				}
			};
			inline void setVec4(const std::string& name, const glm::vec4& v) const {
				{
					auto loc = glGetUniformLocation(ID, name.c_str());
					if (loc == -1) {
						std::cout << "No unform " << name << std::endl; return;
					}
					glUniform4fv(loc, 1, glm::value_ptr(v));
				}
			};
			inline void setMat4(const std::string& name, float* ptr) {
			
				auto loc = glGetUniformLocation(ID, name.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name << std::endl; return;
				}
				glUniformMatrix4fv(loc, 1, GL_FALSE, ptr);
			}
			inline void setMat4(const std::string& name, const glm::mat4& mat) {
				auto loc = glGetUniformLocation(ID, name.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name << std::endl; return;
				}
 
				glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
			}
			inline void setMat3(const std::string& name, const glm::mat3& mat) {
				auto loc = glGetUniformLocation(ID, name.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name << std::endl; return;
				}
				glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
			}
			inline void setTexUnit(const lyh_gl::helper::gl_texture& tex) {
				setTexUnit(tex.name);
			};
			inline void setTexUnit(const std::string& name) {
				tex_unit_table[name] = texture_used_id;
				texture_used_id++;
			};

			inline void bindTex(const std::string& name_in_shader, const lyh_gl::helper::gl_texture& tex) {
				auto tex_unit_id = tex_unit_table[tex.name];
				auto loc = glGetUniformLocation(ID, name_in_shader.c_str());
				if (loc == -1) {
					std::cout << "No unform " << name_in_shader << std::endl;
					return;
				}
				glActiveTexture(GL_TEXTURE0 + tex_unit_id);
				glBindTexture(GL_TEXTURE_2D, tex.texture_id);
				glUniform1i(loc, tex_unit_id);
				//glBindTexture(GL_TEXTURE_2D, NULL);
			};
		};
	    
	
	}
}
#endif