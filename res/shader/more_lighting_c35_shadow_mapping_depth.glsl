//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}