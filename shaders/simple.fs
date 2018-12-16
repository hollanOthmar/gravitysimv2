#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;
in vec4 vertexColor;

uniform vec4 ourColor_old;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
        FragColor = vertexColor;
}