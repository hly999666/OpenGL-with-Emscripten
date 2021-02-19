//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
void main()

{
	TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	
}
//shader fragment
#version 300 es
precision highp float;
in vec2 TexCoord;
uniform sampler2D texture1;
out vec4 FragColor;
void main()
{
	vec4 tex_color = texture(texture1, TexCoord);
	FragColor = vec4(tex_color.rgb, 1.0);
}