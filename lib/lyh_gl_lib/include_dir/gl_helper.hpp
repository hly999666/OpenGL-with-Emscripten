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
#include <thread>
#include <chrono>
#include <future>
#include <functional>
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
		struct texture_info {
			unsigned  char* data{nullptr};
			int width{0};
			int height{ 0 };
			int nrChannels{ 0 };
			GLenum input_format{ GL_RGB };
			GLenum input_type{ GL_UNSIGNED_BYTE };
		};
		void set_up_texture(texture_info tex_1_info, GLuint  texture, GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			auto data = tex_1_info.data; auto width = tex_1_info.width;	auto height = tex_1_info.height; auto nrChannels = tex_1_info.nrChannels;
			input_format = tex_1_info.input_format; input_type = tex_1_info.input_type;

			if (data)

			{

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, input_format, input_type, data);

				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//clear up


				//stbi_image_free(data);
				glBindTexture(GL_TEXTURE_2D, 0);
				glFinish();
				//glfwDestroyWindow(threadWin);
				//return texture;
			}
			else
			{


				glDeleteTextures(1, &texture);
				glFinish();
				//stbi_image_free(data);
				//glfwDestroyWindow(threadWin);
				std::cout << "Failed to load texture" << std::endl;
				//return 0;
			}
		};
		void loadTexture_async(const std::string f_path, GLuint  texture,GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			
	 
			std::packaged_task<texture_info()> task([&] {
				texture_info tex_1_info;
				stbi_set_flip_vertically_on_load(true);
				tex_1_info.data = stbi_load(f_path.c_str(), &tex_1_info.width, &tex_1_info.height, &tex_1_info.nrChannels, 0);
				return tex_1_info;
			});
			auto f1 = task.get_future();  
			std::thread task_td(std::move(task));
			task_td.join();
			auto tex_1_info = f1.get();


			set_up_texture(tex_1_info, texture,   input_format, input_type);
		}
		unsigned int loadTexture(const std::string& path,GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {
			
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
			loadTexture_async(f_path, texture, input_format, input_type);
			return texture;
 
		
		
		}
		class texture_loader {
		public:
			 std::string path;
			GLuint texture_id;
			std::future<texture_info>future;
			texture_info info;
			std::string status{ "N/A" };
			texture_loader(const std::string _path,const std::string mode="async") {
			 
				glGenTextures(1, &texture_id);
				glBindTexture(GL_TEXTURE_2D, texture_id);
			 
			 
          #ifdef __EMSCRIPTEN__
				path = "res/texture/" + _path;
         #else
				auto cwd = boost::filesystem::current_path();
				path = cwd.string() + "/res/texture/" + _path;
         #endif
				//std::thread t1(loadTexture_async, f_path, texture, window, input_format, input_type);t1.detach();
				std::packaged_task<texture_info()> task([&] {
					texture_info tex_1_info;
					stbi_set_flip_vertically_on_load(true);
					auto _p = path;
					tex_1_info.data = stbi_load(path.c_str(), &tex_1_info.width, &tex_1_info.height, &tex_1_info.nrChannels, 0);
					return tex_1_info;
					});
				future = task.get_future();
			
				if (mode == "async") {
					std::thread task_td(std::move(task));
					task_td.detach();
					status = "loading";
				}
				else if (mode == "sync") {
					//run task
					task();
				}
				
			    //task_td.join();
			
			};
			~texture_loader() {
				stbi_image_free(info.data);
			}
			void wait_for(unsigned int time_micro_s) {
				if (status == "loaded")return;
				auto f_status = future.wait_for(std::chrono::microseconds(time_micro_s));
				if (f_status == std::future_status::ready) {
					info = future.get();
					set_up_texture(info, texture_id, info.input_format, info.input_type);
					status = "loaded";
				}
			   }
		 
		};

};
#endif