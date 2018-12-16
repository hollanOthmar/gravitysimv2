#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;
//layout (location = 2) in vec3 aNormal;

//out vec3 ourColor;
//out vec2 TexCoord;
//out vec3 Normal;
//out vec3 FragPos;

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectCenter;
uniform float spaceConst;
//uniform mat4 normalMatrix;
//vec3(-0.5,-0.5,0.5) || vec3(0.5,-0.5,0.5) || vec3(0.5,-0.5,-0.5) || vec3(0.5,0.5,-0.5) || vec3(-0.5,0.5,-0.5) || vec3(-0.5,0.5,0.5) || vec3(-0.5,-0.5,-0.5) || vec3(0.5,0.5,0.5)
void main()
{
    bvec3 res1 = notEqual(aPos, vec3(-0.5,-0.5,0.5));
    bvec3 res2 = notEqual(aPos, vec3(0.5,-0.5,0.5));
    bvec3 res3 = notEqual(aPos, vec3(0.5,-0.5,-0.5));
    bvec3 res4 = notEqual(aPos, vec3(0.5,0.5,-0.5));
    bvec3 res5 = notEqual(aPos, vec3(-0.5,0.5,-0.5));
    bvec3 res6 = notEqual(aPos, vec3(-0.5,0.5,0.5));
    bvec3 res7 = notEqual(aPos, vec3(-0.5,-0.5,-0.5));
    bvec3 res8 = notEqual(aPos, vec3(0.5,0.5,0.5));
    if (all(res1) && all(res2) && all(res3) && all(res4) && all(res5) && all(res6) && all(res7) && all(res8))
    {
        //calculate new point! Based on the inverse square law!
        vec3 distanceChange = aPos - objectCenter;
        float dist = length(distanceChange);

        float scale = spaceConst/ (dist * dist);
        //float scale = 0.7f;
        vec3 newPoint = objectCenter + scale * distanceChange;

        gl_Position = projection * view * model * vec4(newPoint, 1.0);
    }
    else
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    //TexCoord = aTexCoord;
    //Normal = mat3(normalMatrix) * aNormal;
    //FragPos = vec3(model * vec4(aPos, 1.0));
}
