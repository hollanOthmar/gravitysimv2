#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;
layout (location = 1) in vec3 aNormal;

//out vec3 ourColor;
//out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //TexCoord = aTexCoord;
    Normal = mat3(normalMatrix) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}
