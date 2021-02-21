//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aVertColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 vertColor;
void main()

{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	vertColor = aVertColor;
}
//shader fragment
#version 300 es
precision highp float;
in vec2 TexCoord;
in vec3 vertColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float u_time;

out vec4 FragColor;
void main()
{
	vec4 tex_col_1= texture(texture1, TexCoord);
	vec4 tex_col_2 = texture(texture2, TexCoord);
	vec3 tex_col = tex_col_1.rgb * tex_col_2.rgb;
	//FragColor = vec4(0.6,0.6,0.9,1.0);
	FragColor = vec4(tex_col.rgb, 1.0)+vec4(u_time, 0.0, 0.0,1.0);
}