//shader vertex
#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
vec3 getColorID(int id){

   return vec3(float(id%256)/256.0,
                      float(id%(256*256))/(256.0*256.0),
                      float(id%(256*256*256))/(256.0*256.0*256.0)
                      );
}
uniform int showID;
out vec4 colorID;
void main()
{
    TexCoords = aTexCoords;
    vec4 _colorID=vec4(1.0,1.0,1.0,1.0);
    if(showID==1){
        _colorID=vec4(getColorID(gl_InstanceID),1.0);
    }
    colorID=_colorID;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f); 
}
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuse_map;
in vec4 colorID;
void main()
{


    FragColor = texture(diffuse_map, TexCoords)*colorID;
}
