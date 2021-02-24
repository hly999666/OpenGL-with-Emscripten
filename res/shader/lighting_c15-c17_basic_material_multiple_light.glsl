//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
 
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat3 normal_mat;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normal_mat * aNormal;
    TexCoords = aUV;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;

struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

uniform float dirLight_coef;
uniform float pointLight_coef;
uniform float spotLight_coef;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec4 tex_diffuse = texture(material.diffuse_map, TexCoords);
    vec4 tex_specular = texture(material.specular_map, TexCoords);
    vec3 ambient = diff * light.ambient * tex_diffuse.rgb;
    vec3 diffuse = diff * light.diffuse * tex_diffuse.rgb;
    vec3 specular = spec * light.specular * tex_specular.rgb;
    return (ambient + diffuse + specular);
    //return Normal;
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse_map, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse_map, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular_map, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);

}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //float intensity =mix(0.0,1.0,(theta - light.outerCutOff) / epsilon);
  // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse_map, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse_map, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular_map, TexCoords));


    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    // ambient
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result_Dir = CalcDirLight(dirLight, norm, viewDir);
    vec3 result_Point = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result_Point += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    vec3 result_Spot = CalcSpotLight(spotLight, norm, FragPos, viewDir);
    vec3 result_final = result_Dir * dirLight_coef + result_Point * pointLight_coef + result_Spot * spotLight_coef;
    FragColor = vec4(result_final, 1.0);
    //FragColor = vec4(TexCoords.x, TexCoords.y,0.0, 1.0);
}