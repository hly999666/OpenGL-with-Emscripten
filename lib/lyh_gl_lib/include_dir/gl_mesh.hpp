#ifndef GL_MESH_H

 

#define GL_MESH_H

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
#include <vector>
#include <memory>
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
#include "gl_helper.hpp"
#include "gl_shading.hpp"
#define TINYGLTF_IMPLEMENTATION
#define  TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"
typedef  std::string string;
namespace lyh_gl {


	struct Vertex {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
	};
	class gl_mesh {
	public:
		// mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<	std::weak_ptr< lyh_gl::helper::gl_texture>> textures;
		GLuint VAO;
		GLuint EBO;
		GLuint VBO;
 
	
	public:
		gl_mesh() = default;
		gl_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<	std::shared_ptr< lyh_gl::helper::gl_texture>> textures) {
			vertices = std::move(vertices);
			indices = std::move(indices); ;
			textures = std::move(textures);
			setupMesh();
		};
		gl_mesh(gl_mesh&& pre_mesh) {
			vertices= std::move(pre_mesh.vertices);
			indices= std::move(pre_mesh.indices);
			textures= std::move(pre_mesh.textures);
			VAO = pre_mesh.VAO;
			EBO = pre_mesh.EBO;
			VBO = pre_mesh.VBO;
			pre_mesh.VAO = NULL;
			pre_mesh.EBO = NULL;
			pre_mesh.VBO = NULL;
			//delete &pre_mesh;
		}
		~gl_mesh() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1,&VBO);
			glDeleteBuffers(1, &EBO);
		}
		void Draw(lyh_gl::shading::Shader& shader) {
			 //shader.use();
			 for (auto& tex : textures) {
				 auto tex_shared_ptr = tex.lock();
				
			 	if (tex_shared_ptr->name_in_shader == "N/A")continue;
			 shader.bindTex(tex_shared_ptr->name_in_shader, *tex_shared_ptr);
			}
			
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
 
			glActiveTexture(GL_TEXTURE0);
		}
	 
		void bufferIndices(tinygltf::Accessor& acc, tinygltf::BufferView& bv, tinygltf::Buffer& bf) {
 
			unsigned char* data =(unsigned char*) bf.data.data();
			unsigned short* _data16 =(unsigned short*)(data + bv.byteOffset + acc.byteOffset);
			unsigned int* _data32 = (unsigned int*)(data + bv.byteOffset + acc.byteOffset);
			std::vector<unsigned short> v;
			int by_per_unit = bv.byteLength / acc.count;
			for (int i = 0; i < acc.count; i++) {
			   if(by_per_unit==2)indices.push_back((unsigned int)_data16[i]);
			   if (by_per_unit ==4)indices.push_back((unsigned int)_data32[i]);
			}

		 
		}

		void bufferAttributes(tinygltf::Accessor& acc, tinygltf::BufferView& bv, tinygltf::Buffer& bf, const std::string& usage) {
		 
			auto address = (unsigned char*)(bf.data.data()) + bv.byteOffset + acc.byteOffset;

			auto _address = (float*)address;
			if (usage == "POSITION") {

				int count =  acc.count*3;
				for (int i = 0; i < count; i++) {
					vertices[i / 3].Position[i % 3] = _address[i];
				}
			 
			}
			if (usage == "NORMAL") {
				int count = acc.count * 3;
				for (int i = 0; i < count; i++) {
					vertices[i / 3].Normal[i % 3] = _address[i];
				}
	 
			}
			if (usage == "TEXCOORD_0") {
				int count = acc.count * 2;
				for (int i = 0; i < count; i++) {
					vertices[i /2].TexCoords[i %2] = _address[i];
				}
			 
			}
		 
		}
 

		void setupMesh() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			// vertex tangent
			//glEnableVertexAttribArray(3);
			//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			//// vertex bitangent
			//glEnableVertexAttribArray(4);
			//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			glBindVertexArray(0);




		};
	
	};


	std::vector<gl_mesh> loadModelGLTF(std::string path) {
		using namespace tinygltf;
		Model model;
		TinyGLTF loader;
		std::string err;
		std::string warn;

		int vert_size = sizeof(Vertex);
		std::string f_path = "";
		std::ifstream fs;
  #ifdef __EMSCRIPTEN__

		f_path = "res/model/"+ path;
 #else

		auto cwd = boost::filesystem::current_path();
		f_path = cwd.string() + "/res/model/" + path;
#endif
	 
		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, f_path.c_str());
		//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)
		std::vector<gl_mesh>result;
		if (!warn.empty()) {
			printf("Warn: %s\n", warn.c_str());
		}

		if (!err.empty()) {
			printf("Err: %s\n", err.c_str());
		}

		if (!ret) {
			printf("Failed to parse glTF\n");
			return result;
			
		}
	
		for (auto& mesh : model.meshes) {
			std::vector<Vertex> vertices; std::vector<unsigned int> indices;
			//auto now_mesh = std::make_shared<gl_mesh>();
			gl_mesh  now_mesh;
			for (auto& primitive : mesh.primitives) {
				 GLenum mode = primitive.mode;
				 auto& index_accessor = model.accessors[primitive.indices];
			     auto& index_bufferView= model.bufferViews[index_accessor.bufferView];
				 auto& index_buffer  = model.buffers[index_bufferView.buffer];
				 now_mesh.bufferIndices(index_accessor, index_bufferView, index_buffer);
				 auto& attrib_1 = primitive.attributes;
				 auto& acc_1 = model.accessors[attrib_1["POSITION"]];
				 int count = acc_1.count;
				 now_mesh.vertices.resize(count);
				 for (auto& attribute : primitive.attributes) {
					 auto& name = attribute.first;
					 auto& attribute_accessor = model.accessors[attribute.second];
					 auto& attribute_bufferView = model.bufferViews[attribute_accessor.bufferView];
					 auto& attribute_buffer = model.buffers[attribute_bufferView.buffer];
					 now_mesh.bufferAttributes(attribute_accessor, attribute_bufferView, attribute_buffer, name);
				 }
				 now_mesh.setupMesh();
				 //tex_diffuse->loading_thread_join_blocking();
				 //now_mesh->textures.push_back(tex_diffuse);
				 result.push_back(std::move(now_mesh));

			}
		
		}
		
		//now_mesh.setupMesh();
	  
		return result;
	}

}
#endif 