#include <iostream>
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_SIMD_AVX2  
#endif
#include  "../externs/glm/glm/glm.hpp"
#include "../externs/glm/glm/gtc/matrix_transform.hpp"
#include "../externs/glm/glm/gtc/type_ptr.hpp"

//#include  "glm/glm.hpp"
//#include  "glm/gtc/matrix_transform.hpp"
//#include  "glm/gtc/type_ptr.hpp"
int main(){
	std::cout << "Hello, World!" << "\n";
	glm::vec4 v1(1, 2, 3, 4);
	glm::vec4 v2(1, 2, 3, 4);
	glm::mat4 mat1(1.0f);
	auto v3 = (v1 + v2);
	std::cout << v3.x << std::endl;
	return 0;
}
