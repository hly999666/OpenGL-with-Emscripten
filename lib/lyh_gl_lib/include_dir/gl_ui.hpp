#ifndef GL_GUI_H
#define GL_GUI_H

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
#include <vector>
/*#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX 
#else
#define GLM_FORCE_AVX2  
#endif
#include  "../externs/glm/glm/glm.hpp"
#include "../externs/glm/glm/gtc/matrix_transform.hpp" 
#include "../externs/glm/glm/gtc/type_ptr.hpp"*/ 
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "implot.h"
namespace lyh_gl {

	namespace ui {
		//TODO
		class ui_envir {

		};
		void set_up(GLFWwindow* window, const char* glsl_version, bool withImPlot = true) {
			ImGui::CreateContext();
			if (withImPlot)ImPlot::CreateContext();

			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init(glsl_version);
		}

		void clear_up(bool withImPlot = true) {
			ImGui_ImplGlfw_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();
			if (withImPlot)ImPlot::DestroyContext();
			ImGui::DestroyContext();
		
		}
		void drawLineSegments(const float* x, const float* y, int count, ImU32 col = 0x00ff00ff, float thickness = 2.0f) {
			//auto draw_list = ImPlot::GetPlotDrawList();
			for (int i = 0; i < count / 2; i++) {

				ImPlot::PlotLine("line", x + 2 * i, y + 2 * i, 2);
			}
		}
		void readClickPointAndPlot(std::vector<float>& point_x, std::vector<float>& point_y) {
			if (ImGui::IsMouseClicked(1)) {
				auto pos = ImPlot::GetPlotMousePos();
				point_x.push_back(pos.x); point_y.push_back(pos.y);
			}
		
			if (point_x.size() != 0 && point_y.size() != 0) {
				ImPlot::PushColormap(ImPlotColormap_Pastel);
				//std::cout << "point nums = " << point_x.size() << std::endl;
				ImPlot::PlotScatter("clicked point", point_x.data(), point_y.data(), point_x.size());
			}
		
			ImPlot::PopColormap();
		}
	};
};
#endif