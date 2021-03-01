//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
//shader fragment
#version 300 es
precision highp float;

uniform sampler2D tex_1;
uniform int mode;
uniform float discard_threshold;
in vec2 TexCoords;
out vec4 FragColor;
void main()
{
    vec4 texColor = texture(tex_1, TexCoords);
    if(mode==1){
       if(texColor.a < discard_threshold){
           discard;
       }
    }
    FragColor = texColor;
}