//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;
//note TexCoords is vec3 because is using for sampling cubemap
 
//input as uniform buffer object using std140 memory layout
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

void main()
{
      gl_Position = projection * view * model * vec4(aPos, 1.0);
}  
//shader fragment
#version 300 es
precision highp float;

uniform vec4 out_color;
out vec4 FragColor;
uniform int color_range_x;
void main()
{    
    if(gl_FragCoord.x<float(color_range_x)){
          FragColor = out_color;
    }else{
       FragColor = vec4(0.5,0.5,0.5,1.0);
    }
  
}