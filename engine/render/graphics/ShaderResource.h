#pragma once
#include "core/app.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

class ShaderResource
{
public:
	static ShaderResource* Instance();

	GLuint CreateProgram(std::string vsPath, std::string fsPath);
	GLuint CreateComputeShader(std::string csPath);

private:
	static ShaderResource* instance;

	std::string ParseShader(std::string source);

	void Bind(std::string vertSrc, GLuint& program);
	void CreateShader(std::string shaderString, GLuint& shader, GLuint type, GLuint program);
	void LinkProgram(GLuint program, GLuint shader0 = 0, GLuint shader1 = 0);
	std::string ErrorLog(GLuint shader);

	GLuint CompileShader(GLuint type, const std::string& src);
	
	std::unordered_map<std::string, GLuint> programs;
	std::unordered_map<std::string, GLuint> vertexShaders;
	std::unordered_map<std::string, GLuint> fragmentShaders;
	std::unordered_map<std::string, GLuint> computeShaders;
	
	ShaderResource(){}
};

inline void
UniformMat4(const char* name, mat4 const& matrix, GLuint program)
{
	GLuint matLoc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &matrix[0].x);
}

inline void
UniformVec4(const char* name, vec4 v, GLuint program)
{
	GLuint loc = glGetUniformLocation(program, name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

inline void
UniformVec3(const char* name, vec3 v, GLuint program)
{
	GLuint loc = glGetUniformLocation(program, name);
	glUniform3f(loc, v.x, v.y, v.z);
}

inline void
UniformFloat(const char* name, float f, GLuint program)
{
	GLuint loc = glGetUniformLocation(program, name);
	glUniform1f(loc, f);
}

inline void
UniformInt(const char* name, int i, GLuint program)
{
	GLuint loc = glGetUniformLocation(program, name);
	glUniform1i(loc, i);
}