#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform DirectionalLight directional_light;

#define NR_POINT_LIGHTS 7
uniform PointLight point_lights[NR_POINT_LIGHTS];


uniform vec3 cam_pos;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 reflect_direction = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflect_direction), 0.0), material.shininess);
    float amb = 1.0f;


    vec3 ambient  = amb * light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = diff * light.diffuse * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specular = spec * light.specular * texture(material.texture_specular1, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float amb = 1.0;

    vec3 ambient = amb * light.ambient * texture(material.texture_diffuse1, TexCoords).rgb; // ambiant strength (pixel value) * ambient color * diffuse map
    vec3 diffuse = diff * light.diffuse * texture(material.texture_diffuse1, TexCoords).rgb; // diffuse strength (pixel value) * diffuse color * diffuse map
    vec3 specular = spec * light.specular * texture(material.texture_specular1, TexCoords).rgb; // specular strength (pixel value) * specular color * specular map

    return (ambient + diffuse + specular) * attenuation;

}

void main() {
    vec3 color = vec3(0.0f);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cam_pos - FragPos);

    color += calculate_directional_light(directional_light, norm, viewDir);
    
    for (int i = 0; i < NR_POINT_LIGHTS; i++) 
        color += calculate_point_light(point_lights[i], norm, FragPos, viewDir);
    
    // emission
    vec3 emission = texture(material.texture_emission1, TexCoords).rgb;
    // color += emission;

    FragColor = vec4(color, 1.0);
}

// TODO:
// 1) Sort out Spotlight
// 2) Learn what all the vec3 values for ambient, diffuse, and specular means. My guess here is they are the color but I may be wrong.