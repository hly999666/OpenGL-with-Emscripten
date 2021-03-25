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
#include "glm.hpp"
#include "gtc/matrix_transform.hpp" 
#include "gtc/type_ptr.hpp" 
typedef  std::string string;
namespace  lyh_gl {
	namespace helper {
		GLFWwindow* gl_init(unsigned int width, unsigned int height,const char* title= "Hello World") {
			glfwInit();
           #ifdef __EMSCRIPTEN__
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            #else
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            #endif
			//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

			//init window

			glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
			GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

			if (window == NULL)

			{

				std::cout << "Failed to create GLFW window" << std::endl;

				glfwTerminate();

				return nullptr;

			}
			glfwMakeContextCurrent(window);


   #ifdef __EMSCRIPTEN__
    #else
			bool err = glewInit() != GLEW_OK;
			if (GLEW_OK != err)
			{

				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
				return nullptr;
			}

   #endif
			glfwSwapInterval(1);
			return window;
		}
		void gl_clear_up(GLFWwindow* window) {
			glfwDestroyWindow(window);
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}
		unsigned int buildShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& geometryShaderSource) {
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
				//exit(-1);
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
				//exit(-1);
			}
			unsigned int geometryShader=NULL;
			if (geometryShaderSource.size()!=0) {
				const char* gShaderCode = geometryShaderSource.c_str();
				geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometryShader, 1, &gShaderCode, NULL);
				glCompileShader(geometryShader);
				glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" <<
						infoLog << std::endl;
					//exit(-1);
				}
			}
			unsigned int shaderProgram;
			shaderProgram = glCreateProgram();

			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			if (geometryShaderSource.size() != 0)
				glAttachShader(shaderProgram, geometryShader);
			glLinkProgram(shaderProgram);

			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::SHADERPROGRAM::LINKING_FAILED\n" <<
					infoLog << std::endl;
				//exit(-1);
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteShader(geometryShader);
			return shaderProgram;
		};
		bool  parseShader(const std::string path, std::string& out_vs, std::string& out_fs,std::string& out_gs) {

			std::ifstream fs;
#ifdef __EMSCRIPTEN__
			fs.open("res/shader/" + path);
#else
			auto cwd = boost::filesystem::current_path();
			fs.open(cwd.string() + "/res/shader/" + path);
#endif

			//std::ifstream fs; fs.open("/res/preload-file/shader/" + path);
			std::string now_line;
			std::stringstream ss_vs; std::stringstream ss_fs; std::stringstream ss_gs;
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
					else if (now_line.find("geometry") != std::string::npos) {
						now_t = 'g';
					}
					continue;
				}
				if (now_t == 'v')ss_vs << now_line << std::endl;
				if (now_t == 'f') ss_fs << now_line << std::endl;
				if (now_t == 'g') ss_gs << now_line << std::endl;
			}
			out_vs = ss_vs.str();
			out_fs = ss_fs.str();
			out_gs = ss_gs.str();
			if (out_vs.size() == 0 || out_fs.size() == 0) return false;
			else return true;
		}
		struct texture_info {
			unsigned  char* data{ nullptr };
			int width{ 0 };
			int height{ 0 };
			int nrChannels{ 0 };
			GLenum internal_format{ GL_RGB };
			GLenum input_format{ GL_RGB };
			GLenum  type{ GL_UNSIGNED_BYTE };
		};
		void set_up_texture(texture_info tex_1_info, GLuint  texture, GLenum _internal_format = GL_RGB, GLenum  _input_format= GL_RGB, GLenum _type = GL_UNSIGNED_BYTE) {
			auto data = tex_1_info.data; auto width = tex_1_info.width;	auto height = tex_1_info.height; auto nrChannels = tex_1_info.nrChannels;
			auto internal_format = tex_1_info.internal_format; 
			auto input_format = tex_1_info.internal_format; 
			auto type = tex_1_info.type;

			if (data)

			{
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, input_format, type, data);

				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//clear up


				//stbi_image_free(data);
				glBindTexture(GL_TEXTURE_2D, NULL);
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
		void loadTexture_async(const std::string f_path, GLuint  texture, GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {


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


			set_up_texture(tex_1_info, texture, input_format, input_type);
		}
		unsigned int loadTexture(const std::string& path, GLenum input_format = GL_RGB, GLenum input_type = GL_UNSIGNED_BYTE) {

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			std::string f_path = "";
			std::ifstream fs;
#ifdef __EMSCRIPTEN__

			f_path = "res/texture/" + path;
#else
		 
			auto cwd = boost::filesystem::current_path();
			f_path = cwd.string() + "/res/texture/" + path;
#endif
			//std::thread t1(loadTexture_async, f_path, texture, window, input_format, input_type);t1.detach();
			loadTexture_async(f_path, texture, input_format, input_type);
			return texture;



		}
		GLuint loadCubemap(std::vector<std::string> faces) {
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
			auto cwd = boost::filesystem::current_path();
			int width, height, nrComponents;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				std::string file_path = cwd.string() + "/res/texture/" + faces[i];
				unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrComponents, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureID;
		}
		class gl_texture {
		public:
			std::string name;
			std::string path;
			GLuint texture_id;
			std::future<texture_info>future;
			texture_info info;
			std::string status{ "N/A" };
			std::thread* loading_thread;
			std::string name_in_shader{ "N/A" };
			gl_texture(const std::string _path, const std::string mode = "async", GLenum input_format = GL_RGB, std::string _name_in_shader = "N/A",bool isFlipY=true)
				: name_in_shader(_name_in_shader) {

				glGenTextures(1, &texture_id);
				glBindTexture(GL_TEXTURE_2D, texture_id);

				name = _path;
#ifdef __EMSCRIPTEN__
				path = "res/" + _path;
#else
				auto cwd = boost::filesystem::current_path();
				path = cwd.string() + "/res/" + _path;
#endif
				//std::thread t1(loadTexture_async, f_path, texture, window, input_format, input_type);t1.detach();
				std::packaged_task<texture_info()> task([&] {
					texture_info tex_1_info;
					if(isFlipY)stbi_set_flip_vertically_on_load(true);
					auto _p = path;
					tex_1_info.data = stbi_load(path.c_str(), &tex_1_info.width, &tex_1_info.height, &tex_1_info.nrChannels, 0);
					return tex_1_info;
					});
				future = task.get_future();

				if (mode == "async") {
					loading_thread=new std::thread(std::move(task));
					loading_thread->detach();
					status = "loading";
				}
				else if (mode == "sync") {
					//run task
					task();
					status = "loaded";
				}
				else if (mode == "async_joinable") {
					//run task
					loading_thread = new std::thread(std::move(task));
					status = "loading";
				}

				//task_td.join();

			};
			~gl_texture() {
				stbi_image_free(info.data);
			}
			void loading_thread_join_blocking() {
				loading_thread->join();
				wait_for(1);
			};
			void wait_for(unsigned int time_micro_s) {
				if (status == "loaded")return;
				auto f_status = future.wait_for(std::chrono::microseconds(time_micro_s));
				if (f_status == std::future_status::ready) {
					info = future.get();
					if (std::strstr(name.c_str(), ".png") != NULL) {
						info.internal_format = GL_RGBA;
						info.input_format = GL_RGBA;
					}
					set_up_texture(info, texture_id);
					status = "loaded";
				}
				delete loading_thread;
			}

		};
		enum Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT
		};
		// Default camera values
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SPEED = 2.5f;
		const float SENSITIVITY = 0.1f;
		const float ZOOM = 45.0f;
		class Camera
		{
		public:
			// camera Attributes
			glm::vec3 Position;
			glm::vec3 Front;
			glm::vec3 Up;
			glm::vec3 Right;
			glm::vec3 WorldUp;
			// euler Angles
			float Yaw;
			float Pitch;
			// camera options
			float MovementSpeed;
			float MouseSensitivity;
			float Zoom;

			// constructor with vectors
			Camera(
				glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
				float yaw = YAW, float pitch = PITCH
			) :
				Front(glm::vec3(0.0f, 0.0f, -1.0f)),
				MovementSpeed(SPEED),
				MouseSensitivity(SENSITIVITY),
				Zoom(ZOOM)
			{
				Position = position;
				WorldUp = up;
				Yaw = yaw;
				Pitch = pitch;
				updateCameraVectors();
			}
			// constructor with scalar values
			Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
				Front(glm::vec3(0.0f, 0.0f, -1.0f)),
				MovementSpeed(SPEED),
				MouseSensitivity(SENSITIVITY),
				Zoom(ZOOM)
			{
				Position = glm::vec3(posX, posY, posZ);
				WorldUp = glm::vec3(upX, upY, upZ);
				Yaw = yaw;
				Pitch = pitch;
				updateCameraVectors();
			}

			// returns the view matrix calculated using Euler Angles and the LookAt Matrix
			glm::mat4 GetViewMatrix()
			{
				//return glm::lookAt(Position, Position + Front, Up);
				return _lookAt();
			}
			glm::mat4 _lookAt() {
				using namespace glm;
				vec3 d = -Front;
				vec3 r = cross(WorldUp, d);
				vec3 u = cross(d, r);
				mat4 rot{
					 r.x,r.y,r.z,0,
					 u.x,u.y,u.z,0,
					 d.x,d.y,d.z,0,
					 0,0,0,1
				};
				rot = transpose(rot);
				mat4 trans = translate(mat4(1.0f), -Position);
				return rot * trans;
			}
			// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
			void ProcessKeyboard(Camera_Movement direction, float deltaTime)
			{
				float velocity = MovementSpeed * deltaTime;
				if (direction == FORWARD)
					Position += Front * velocity;
				if (direction == BACKWARD)
					Position -= Front * velocity;
				if (direction == LEFT)
					Position -= Right * velocity;
				if (direction == RIGHT)
					Position += Right * velocity;
			}

			// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
			void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
			{


				xoffset *= MouseSensitivity;
				yoffset *= MouseSensitivity;

				Yaw += xoffset;
				Pitch += yoffset;

				// make sure that when pitch is out of bounds, screen doesn't get flipped
				if (constrainPitch)
				{
					if (Pitch > 89.0f)
						Pitch = 89.0f;
					if (Pitch < -89.0f)
						Pitch = -89.0f;
				}

				// update Front, Right and Up Vectors using the updated Euler angles
				updateCameraVectors();
			}

			// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
			void ProcessMouseScroll(float yoffset)
			{
				Zoom -= (float)yoffset;
				if (Zoom < 1.0f)
					Zoom = 1.0f;
				if (Zoom > 45.0f)
					Zoom = 45.0f;
			}

		public:
			// calculates the front vector from the Camera's (updated) Euler Angles
			void updateCameraVectors()
			{
				// calculate the new Front vector
				using namespace glm;

				vec4 vf(-1, 0, 0, 0);
				mat4 m_pitch = glm::rotate(mat4(1.0f), radians(Pitch), vec3(1.0, 0.0, 0.0));
				mat4 m_yaw = glm::rotate(mat4(1.0f), radians(Yaw), vec3(0.0, 1.0, 0.0));
				vf = m_yaw * m_pitch * vf;
				vec3 _front = vec3(vf.x, vf.y, vf.z);

				vec3 front;
				front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				front.y = sin(glm::radians(Pitch));
				front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

				Front = glm::normalize(front);
				// also re-calculate the Right and Up vector
				Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				Up = glm::normalize(glm::cross(Right, Front));
			};
		};
		//TODO abstract  framebuffer
		class Framebuffer {




		};
	
	
	}
	
};
#endif