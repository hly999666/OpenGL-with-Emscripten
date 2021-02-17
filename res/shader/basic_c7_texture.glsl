//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
//uniform vec4 ourColor;
out vec3 ourColor;
out vec2 TexCoord;
void main()

{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
}
//shader fragment
#version 300 es
precision highp float;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor;
void main()
{
	vec4 tex_color= texture(ourTexture, TexCoord);
	FragColor = vec4(tex_color.rgb, 1.0);
}