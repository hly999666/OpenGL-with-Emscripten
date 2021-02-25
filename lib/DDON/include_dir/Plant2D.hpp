#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_AVX2  
#endif
//#include  "../../../externs/glm/glm/glm.hpp"
//#include "../../../externs/glm/glm/gtc/matrix_transform.hpp"
//#include "../../../externs/glm/glm/gtc/type_ptr.hpp"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include<vector>

    void fractalTree2D_re(double theta, double s, std::vector<float>& x, std::vector<float>& y, glm::mat4 current_mat, double len) {
        using namespace glm;
        vec4 begin(0.0, 0.0, 0.0, 1.0);
        vec4 end(0.0, len, 0.0, 1.0);
        begin = current_mat * begin;
        end = current_mat * end;
        auto current_len = length(end - begin);
        if (current_len < 1e-1)return;
        x.push_back(begin.x); x.push_back(end.x);
        y.push_back(begin.y); y.push_back(end.y);
        mat4 m_t = translate(mat4(1.0f), vec3(0, len, 0));
        mat4 m_rl = rotate(mat4(1.0f), (float)-theta, vec3(0, 0, 1));
        mat4 m_rr = rotate(mat4(1.0f), (float)theta, vec3(0, 0, 1));
        mat4 m_s = scale(mat4(1.0f), vec3(s, s, s));
        fractalTree2D_re(theta, s, x, y, m_t * m_rl * m_s * current_mat, len);
        fractalTree2D_re(theta, s, x, y, m_t * m_rr * m_s * current_mat, len);
    }
namespace DDoN {
    void fractalTree2D_Basic(double theta, double s, std::vector<float>& x, std::vector<float>& y,double len=5.0) {
        fractalTree2D_re(theta,s,x,y,glm::mat4(1.0),len);
    }

}