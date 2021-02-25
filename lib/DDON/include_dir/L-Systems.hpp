#ifdef __EMSCRIPTEN__
#define GLM_FORCE_SIMD_AVX
#else
#define GLM_FORCE_AVX2  
#endif
 
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include<vector>
#include<functional>
#include<string>
#include<unordered_map>
#include<memory>
namespace DDoN {
    class geometry_info {
    public:
        std::string type;
        glm::vec3 begin;
        glm::vec3 end;
    };
    class turtle_state {
        public:
            glm::vec3 position{0,0,0};
            glm::mat4 mat;
            
    };
    class  turtle {
       public:
           turtle_state current_state;
           std::vector<geometry_info>result;
           std::stack<turtle_state>state_stack;
           std::unordered_map<char, std::function<void()>> repertoire;
       public:
           turtle() = default;
           std::vector<geometry_info> eval(std::string& str, double para_1) {
               result.clear();
               for (auto c : str) {
                   
               
               }
               return result;
           }
    };
    class LSystem {
    public:
        std::string init;
        std::unordered_map<std::string, std::string> replacement_rule;
    public:
        std::vector<geometry_info> eval( int n, double para_1) {
             //string replacement
            std::string result_str= eval_rule(n);
            turtle t1;
            return t1.eval(result_str, para_1);
        }
        std::string eval_rule(int n) {
            //string replacement
            std::string str;
           
            return str;
        }
    };


}