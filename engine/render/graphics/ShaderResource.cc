#include "config.h"
#include "ShaderResource.h"

ShaderResource* ShaderResource::instance = nullptr;

ShaderResource* 
ShaderResource::Instance()
{
	if (instance == nullptr)
		instance = new ShaderResource();

	return instance;
}

GLuint
ShaderResource::CreateProgram(std::string vsPath, std::string fsPath)
{
	std::string key = vsPath + ";" + fsPath;

	if (programs.find(key) != programs.end())
		return programs[key];

	GLuint programId = glCreateProgram();

	// Add shaders to map if they arent loaded
	if (vertexShaders.find(vsPath) == vertexShaders.end())
	{
		GLuint id = 0;
		std::string vs = ParseShader(vsPath);
		CreateShader(vs, id, GL_VERTEX_SHADER, programId);
		vertexShaders[vsPath] = id;
	}
	if (fragmentShaders.find(vsPath) == fragmentShaders.end())
	{
		GLuint id = 0;
		std::string fs = ParseShader(fsPath);
		CreateShader(fs, id, GL_FRAGMENT_SHADER, programId);
		fragmentShaders[fsPath] = id;
	}

	GLuint vertId = vertexShaders[vsPath];
	GLuint fragId = fragmentShaders[fsPath];

	LinkProgram(programId, vertId, fragId);

	// Error messages
	std::string vsErrors = ErrorLog(vertId);
	std::string fsErrors = ErrorLog(fragId);

	if (!vsErrors.empty())
		std::cout << "Errors in " << vsPath << ": " << vsErrors << "\n";
	if (!fsErrors.empty())
		std::cout << "Errors in " << fsPath << ": " << fsErrors << "\n";

	programs[key] = programId;

	return programId;
}

GLuint ShaderResource::CreateComputeShader(std::string csPath)
{
	if (programs.find(csPath) != programs.end())
		return programs[csPath];

	GLuint programId = glCreateProgram();

	if (computeShaders.find(csPath) == computeShaders.end())
	{
		GLuint id = 0;
		std::string cs = ParseShader(csPath);
		CreateShader(cs, id, GL_COMPUTE_SHADER, programId);
		computeShaders[csPath] = id;
	}
	
	LinkProgram(programId, computeShaders[csPath]);

	std::string csErrors = ErrorLog(computeShaders[csPath]);

	if (!csErrors.empty())
	std::cout << "Errors in " << csPath << ": " << csErrors << '\n';

	return programId;
}


std::string
ShaderResource::ParseShader(std::string source)
{
	std::ifstream stream(source);

	if (!stream.good())
	{
		std::cout << "Could not find file at " << source << std::endl;
		return "";
	}
	// Read vert and frag shader and convert to string

	std::string line;
	std::stringstream sstream;

	while (getline(stream, line))
		sstream << line << '\n';

	return sstream.str();
}


GLuint
ShaderResource::CompileShader(GLuint type, const std::string& src)
{
	GLuint id = glCreateShader(type);
	const char* c_src = src.c_str();

	glShaderSource(id, 1, &c_src, NULL);
	glCompileShader(id);

	return id;
}

void
ShaderResource::CreateShader(std::string shaderString, GLuint& shader, GLuint type, GLuint program)
{

	// Create shader
	shader = ShaderResource::CompileShader(type, shaderString);

	// Give shader to GPU
	glAttachShader(program, shader);
}

void ShaderResource::LinkProgram(GLuint program, GLuint shader0, GLuint shader1)
{
	glLinkProgram(program);
	glValidateProgram(program);

	// Delete intermediates
	if (shader0)
		glDeleteShader(shader0);
	if (shader1)
		glDeleteShader(shader1);
}

void
ShaderResource::Bind(std::string vertSrc, GLuint& program)
{


}

std::string
ShaderResource::ErrorLog(GLuint shader)
{
	std::string error;

	GLint shaderLogSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(shader, shaderLogSize, NULL, buf);
		error = ("[SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	return error;
}