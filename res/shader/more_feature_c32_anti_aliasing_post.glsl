//shader vertex
#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoords;
uniform int grayscale;
uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    if(grayscale==1){
          float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
         FragColor = vec4(vec3(grayscale), 1.0);
    }else{
          FragColor = vec4(col, 1.0);
    }
  
} 
