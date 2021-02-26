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
 
int main()

{

	  int width = 1280;
	  int height = 720;
	//for camera and UI
		//camera
	lyh_gl::helper::Camera camera(glm::vec3(3.0f, 0.0f, 8.0f));
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
	bool firstMouse = true;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const char* glsl_version = "#version 300 es";

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
 


	GLFWwindow* window = lyh_gl::helper::gl_init(width, height,"Depth Test");

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
		processInput = [&deltaTime, &camera](GLFWwindow* _window)->void
		{
			using namespace lyh_gl::helper;
			if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(_window, true);
			}


			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
				//std::cout << "Press_W" << std::endl;
				camera.ProcessKeyboard(FORWARD, deltaTime);
			}

			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
				//std::cout << "Press_S" << std::endl;
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			}

			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
				//std::cout << "Press_A" << std::endl;
				camera.ProcessKeyboard(LEFT, deltaTime);
			}

			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
				//std::cout << "Press_D" << std::endl;
				camera.ProcessKeyboard(RIGHT, deltaTime);
			}

		};
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	//create shader
 
	lyh_gl::shading::Shader lightingShader("lighting_c22_Depth_Testing.glsl");
	lyh_gl::shading::Shader shader_depth_test("lighting_c22_Depth_Testing_Basic_Shader.glsl");

	

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};


	//build geometry
	unsigned int VBO, VAO, EBO;
	lyh_gl::geometry::buildBox(VBO, VAO, EBO);
	//auto meshes = lyh_gl::loadModelGLTF("backpack/backpack.gltf");
	//auto tex_diffuse=std::make_shared<lyh_gl::helper::gl_texture>("model/backpack/diffuse.jpg", "async_joinable", GL_RGB, "material.diffuse_map",false);
	auto marble_diffuse = std::make_shared<lyh_gl::helper::gl_texture>("texture/marble.jpg", "async_joinable", GL_RGB, "diffuse_map", false);
	auto metal_diffuse = std::make_shared<lyh_gl::helper::gl_texture>("texture/metal.jpg", "async_joinable", GL_RGB, "diffuse_map", false);
	//meshes[0].setupMesh();
	//lyh_gl::geometry::buildRect(meshes[0].VBO, meshes[0].VAO, meshes[0].EBO);
	//set up texture
	//lyh_gl::helper::gl_texture tex_diffuse{ "texture/container2.png","async_joinable" };
	//tex_diffuse.name_in_shader = "material.diffuse_map";
	//lyh_gl::helper::gl_texture tex_specular{ "texture/container2_specular.png","async_joinable" };
	//tex_specular.name_in_shader = "specular.diffuse_map";
	//tex_diffuse.loading_thread_join_blocking(); tex_specular.loading_thread_join_blocking();
	//set up texture in shader
	//lightingShader.setTexUnit(tex_diffuse);
	//lightingShader.setTexUnit(tex_specular);
	unsigned int planeVBO, planeVAO, planeEBO;
	lyh_gl::geometry::buildRect(planeVBO, planeVAO, planeEBO);



	//tex_diffuse->loading_thread_join_blocking();
	
	//meshes[0].textures.push_back(tex_diffuse);
	//lightingShader.setTexUnit(*tex_diffuse);
	marble_diffuse->loading_thread_join_blocking();
	metal_diffuse->loading_thread_join_blocking();
	shader_depth_test.setTexUnit(*marble_diffuse);
    shader_depth_test.setTexUnit(*metal_diffuse);
	//set up ui
	lyh_gl::ui::set_up(window, glsl_version);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto font_1 = io.Fonts->AddFontFromFileTTF("./res/font/Roboto-Medium.ttf", 16.0f);

 
	float col_diffuse[4] = { 0.5,0.5,0.5,1.0 };
	float col_light[4] = { 0.5,0.5,0.5,1.0 };

	  float dirLight_coef=1.0;
	  float pointLight_coef = 1.0;
	  float spotLight_coef = 1.0;
	  float spotLight_outerCutOff = 15.0f;
	  int mode = 0;
	render_loop = [&] {
		//timing
		float currentFrame = glfwGetTime();
		glfwMakeContextCurrent(window);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

	    processInput(window);
		 
	
		
		//setup viewport and basic rendering state
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, width, height);


		//set up GUI
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont(font_1);


		ImGui::SetNextWindowPos({ 32,50 });
		ImGui::SetNextWindowSize({ 150,120 });
		ImGui::Begin("Render Mode");
		//ImGui::SliderFloat("dirLight_coef",&dirLight_coef,0.0,3.0);
		//ImGui::SliderFloat("pointLight_coef", &pointLight_coef, 0.0,3.0);
		//ImGui::SliderFloat("spotLight_coef", &spotLight_coef, 0.0, 3.0);
		//ImGui::SliderFloat("spotLight_outerCutOff", &spotLight_outerCutOff, 15.0f,45.0f);
		ImGui::RadioButton("color", &mode, 0);// ImGui::SameLine();
		ImGui::RadioButton("depth linear", &mode, 1); //ImGui::SameLine();
		ImGui::RadioButton("depth unlinear", &mode, 2);


		ImGui::End();

		ImGui::PopFont();
		ImGui::Render();
 
		//set uniform 
		 
	
	
		//assign from ui
		//glm::vec3 lightColor(col_light[0], col_light[1], col_light[2]);
		//auto diffuse_input = glm::vec3(col_diffuse[0], col_diffuse[1], col_diffuse[2]);
		//draw shaded cube
		//lightingShader.use();
		//set up uniform for rendering
		{
			lightingShader.setFloat("dirLight_coef", dirLight_coef);
			lightingShader.setFloat("pointLight_coef", pointLight_coef);
			lightingShader.setFloat("spotLight_coef", spotLight_coef);

			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);


			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032);
			// point light 2
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
			lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.09);
			lightingShader.setFloat("pointLights[1].quadratic", 0.032);
			// point light 3
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
			lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.09);
			lightingShader.setFloat("pointLights[2].quadratic", 0.032);
			// point light 4
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
			lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.09);
			lightingShader.setFloat("pointLights[3].quadratic", 0.032);


			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09);
			lightingShader.setFloat("spotLight.quadratic", 0.032);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLight_outerCutOff)));
		
		}


	


		float _near = 3.0f; float _far = 20.0f;
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, _near, _far);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);


		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);


		//lightingShader.bindTex("material.diffuse_map", tex_diffuse);
		//lightingShader.bindTex("material.specular_map", tex_specular);

		   //glBindVertexArray(VAO);

	 //draw backpack
			// calculate the model matrix for each object and pass it to shader before drawing
			  model = glm::mat4(1.0f);
			//model = glm::translate(model, cubePositions[i]);
			// angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0, 0.0));
			lightingShader.setMat4("model", model);

			auto normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));
			lightingShader.setMat3("normal_mat", normal_mat);


			  //meshes[0].Draw(lightingShader);
		      //glDrawArrays(GL_TRIANGLES, 0, 36);
			//meshes[0].Draw(lightingShader);
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	 



		//draw light cube

		shader_depth_test.use();
		
		shader_depth_test.setMat4("view", view);
		shader_depth_test.setMat4("projection", projection);
		//shader_depth_test.setVec4("_lightColor", 0.8f, 0.8f, 0.8f, 1.0);

		glBindVertexArray(VAO);
      	model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(3.0f)); // Make it a smaller cube
		shader_depth_test.setMat4("model", model);
		shader_depth_test.setVec2("uvScale", 1.0f,1.0f);
		shader_depth_test.bindTex("diffuse_map", *metal_diffuse);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0, 0.0, -6.0));
		model = glm::scale(model, glm::vec3(3.0f)); // Make it a smaller cube
		shader_depth_test.setMat4("model", model);
		shader_depth_test.setVec2("uvScale", 1.0f, 1.0f);
		shader_depth_test.bindTex("diffuse_map", *metal_diffuse);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		 glBindVertexArray(planeVAO);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0, -1.5, 0.0));
			model = glm::scale(model, glm::vec3(30.0f)); // Make it a smaller cube
			model = glm::rotate(model,glm::radians(90.0f),  glm::vec3(1.0,0.0,0.0));
			shader_depth_test.setMat4("model", model);
			shader_depth_test.setInt("mode", mode);
			shader_depth_test.setVec2("uvScale", 3.0f,3.0f);
			shader_depth_test.setVec2("clipRange", _near, _far);
			shader_depth_test.bindTex("diffuse_map", *marble_diffuse);
			//meshes[0].Draw(shader_depth_test);
			 //glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	 
		 


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

		glfwPollEvents();
	};
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (!glfwWindowShouldClose(window))main_loop();
#endif


	// Cleanup
	//cleanup imgui
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//cleanup glfw
	lyh_gl::ui::clear_up();
	lyh_gl::helper::gl_clear_up(window);

	return 0;
	
};

 