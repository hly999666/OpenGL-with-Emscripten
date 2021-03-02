//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;
//note TexCoords is vec3 because is using for sampling cubemap
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    //replace z with w,after perspective division,z mapping to 1.0,is the largest in the clip space
    gl_Position = pos.xyww;
}  
//shader fragment
#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}