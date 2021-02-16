//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;
out vec2 vUV;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
};
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;
in vec2 vUV;
void main()
{
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
};