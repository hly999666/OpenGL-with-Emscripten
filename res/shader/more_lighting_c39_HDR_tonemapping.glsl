//shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
//shader fragment
#version 330 core
//precision highp float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform int hdr_mode;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr_mode==0){
      vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }else if(hdr_mode==1)
    {
        // reinhard
        vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure
        //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }else if(hdr_mode==2)
    {
       vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
      
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}