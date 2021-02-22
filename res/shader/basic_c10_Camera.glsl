//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoord;
out vec3 normal;
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	normal=aNormal;
}
//shader fragment
#version 300 es
precision highp float;
in vec2 TexCoord;
in vec3 normal;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 FragColor;
void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
	//FragColor=vec4(normal,1.0);
}