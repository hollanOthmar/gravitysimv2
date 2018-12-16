#ifndef SHADER_HPP
#define SHADER_HPP
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int loadShader();
class Shader
{
private:
    void checkCompileErrors(unsigned int shader, std::string type);
public:
    unsigned int ID;
    Shader(const char * vertexPath, const char * fragmentPath);
    void use() const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4f(const std::string &name, glm::mat4 &mat) const;
    void setVec3f(const std::string &name, glm::vec3 &vec) const;
};
#endif
