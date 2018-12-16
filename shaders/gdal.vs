#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float Max;
uniform float Min;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
    vec3 p = aPos;
    p.y *= Max;

    gl_Position = projection * view * model * vec4(p, 1.0);
    TexCoord0 = TexCoord;
    Normal0 = (model*vec4(Normal,0.0)).xyz;
    WorldPos0 = (model*vec4(p,1.0)).xyz;
}
