#version 330 core
// in vec2 TexCoord;
// in vec3 Normal;
// in vec3 FragPos;

out vec4 FragColor;
// uniform vec4 ourColor_old;
// uniform sampler2D texture1;
// uniform sampler2D texture2;

// uniform vec3 lightColor;
// uniform vec3 lightPos;
// uniform vec3 viewPos;

void main()
{
        
        // //Ambient calcs
        // float ambientStrength = 0.1;
        // vec3 ambient = ambientStrength * lightColor;

        // //Diffuse calcs
        // vec3 norm = normalize(Normal);
        // vec3 lightDir = normalize(lightPos - FragPos);
        // float diff = max(dot(norm, lightDir), 0.0);
        // vec3 diffuse = diff * lightColor;

        // //specular calcs
        // float specularStrength = 0.5;
        // vec3 viewDir = normalize(viewPos - FragPos);
        // vec3 reflectDir = reflect(-lightDir, norm);

        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        // vec3 specular = specularStrength * spec * lightColor;

        // vec3 result = ambient * objectColor;
        // FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2) * vec4((ambient+diffuse+specular), 1.0f);
        FragColor = vec4(0.9f,0.9f,0.9f,0.3f);
}
