//shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(model) * aNormal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
//shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;

//faster normal using dFd
uniform int tex_mode;
vec3 getNormalFromMap()
{
    if(tex_mode==0)return  normalize(Normal);
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// Distribution
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// Geometry-Schlick
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
//Geometry-Smith
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
//fresnel-Schlick
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

//render ui input 
uniform int render_mode;
uniform vec3 _albedo;
uniform float _metalness;
uniform float _roughness;
uniform float _ao;
void main()
{          
    //get parameter from texture   
    vec3 albedo     = _albedo;
    float metallic  =_metalness;
    float roughness =_roughness;
     float ao =_ao;
 
    if(tex_mode==1){
      albedo   =  pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
      metallic =texture(metallicMap, TexCoords).r;
      roughness=texture(roughnessMap, TexCoords).r;
      ao= texture(aoMap, TexCoords).r;
    }

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
   //base reflectance
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

//
   vec3 diffuse= vec3(0.0);
   vec3 gloss= vec3(0.0);
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
         
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't be above 1.0
 
        vec3 kD = vec3(1.0) - kS;
        // metallic no diffuse
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

         vec3 diffuse_lo=kD * albedo / PI * radiance * NdotL;  
         vec3 gloss_lo=specular* radiance * NdotL;
         diffuse+=diffuse_lo;
         gloss+=gloss_lo;
        //real-time version of render equation,add to outgoing radiance Lo
        //Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
        //Lo += diffuse_lo+gloss_lo;
    }   
    Lo=diffuse+gloss;
    // ambient lighting
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color =vec3(0.0);
    if(render_mode==0){
        color= ambient + Lo;
    }else if(render_mode==1){
        color=diffuse;
    }else if(render_mode==2){
       color=gloss;
    }
    
    //  tonemapping
    color = color / (color + vec3(1.0));
    // gamma 
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
