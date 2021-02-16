//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;  
layout(location = 1) in vec3 aColor;
//uniform vec4 ourColor;
out vec3 ourColor;
void main()

{
	gl_Position = vec4(aPos, 1.0); 
	ourColor = aColor;
}
//shader fragment
#version 300 es
precision highp float;
in vec3 ourColor;
out vec4 FragColor;
void main()
{
	FragColor = vec4(ourColor.rgb, 1.0);
}