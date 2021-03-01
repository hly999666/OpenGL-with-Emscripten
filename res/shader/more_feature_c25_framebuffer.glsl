//shader vertex
#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

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
uniform sampler2D screenTexture;
uniform float radius;
uniform int mode_effect;
void main()
{
  float offset = 1.0 / radius;
    vec2 offsets[9] = vec2[](
       vec2(-offset, offset), // top-left
       vec2( 0.0f, offset), // top-center
       vec2( offset, offset), // top-right
      vec2(-offset, 0.0f), // center-left
      vec2( 0.0f, 0.0f), // center-center
      vec2( offset, 0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f, -offset), // bottom-center
      vec2( offset, -offset) // bottom-right
);

float kernel_sharpen[9] = float[](
-1, -1, -1,
-1, 9, -1,
-1, -1, -1
);
 
float kernel_blur[9] = float[](
1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0

);

    vec3 col = vec3(0.0);
    if(mode_effect==1){
        col = texture(screenTexture, TexCoords).rgb;
    }else if(mode_effect==2){
       col=vec3(1.0 - texture(screenTexture, TexCoords));
    }else if(mode_effect==3){
         col = texture(screenTexture, TexCoords).rgb;
       float average = (col.r + col.g + col.b) / 3.0;
       col = vec3(average, average, average); 
    } else if(mode_effect==4){
       vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
        {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st +offsets[i]));
         }


for(int i = 0; i < 9; i++){
    col += sampleTex[i] * kernel_sharpen[i];
}
    }else if(mode_effect==5){
       vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
        {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st +offsets[i]));
         }


for(int i = 0; i < 9; i++){
    col += sampleTex[i] * kernel_blur[i];
   }
    }



FragColor = vec4(col, 1.0);
} 