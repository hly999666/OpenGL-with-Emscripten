//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
out vec3 Normal;
out vec3 Position;
out vec2 vUV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //normal and position is in world space
    vUV=aUV;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  
//shader fragment
#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 vUV;


uniform vec3 cameraPos;
uniform sampler2D diffuse_map;
uniform samplerCube skybox;

uniform float ratio;
uniform int mode;
void main()
{    
    vec3 I = normalize(Position - cameraPos);
    if(mode==1){
       FragColor=vec4( texture(diffuse_map, vUV).rgb,1.0);
    }else if(mode==2){
          vec3 R_1 = reflect(I, normalize(Normal));
     FragColor=vec4 (texture(skybox, R_1).rgb,1.0);
  
    }else if(mode==3){

          vec3 R_2 = refract(I, normalize(Normal),1.0/ ratio);
  
 
       FragColor=vec4(texture(skybox, R_2).rgb,1.0);
    }else{
        FragColor=vec4(0.5,0.5,0.5,1.0);
    }
   
  
   
}