//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;
uniform vec4 _lightColor;
void main()
{
	  FragColor =_lightColor;  
}