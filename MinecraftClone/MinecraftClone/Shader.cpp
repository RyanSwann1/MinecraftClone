#include "Shader.h"
#include "glad.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filePath)
	: m_rendererID(0),
	m_filePath(filePath)
{
	ShaderProgramSource source = parseShader();

	m_rendererID = createShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_rendererID);
}

ShaderProgramSource Shader::parseShader()
{
	enum class eShaderType
	{
		eNone = -1,
		eVertex = 0,
		eFragment = 1,
		eTotal = 2
	};

	std::ifstream stream(m_filePath);
	std::string line;
	std::stringstream stringStream[static_cast<int>(eShaderType::eTotal)];
	eShaderType shaderType = eShaderType::eNone;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("Vertex") != std::string::npos)
			{
				shaderType = eShaderType::eVertex;
			}
			else if (line.find("Fragment") != std::string::npos)
			{
				shaderType = eShaderType::eFragment;
			}
		}
		else
		{
			stringStream[static_cast<int>(shaderType)] << line << "\n";
			std::cout << line << "\n";
		}
	}

	return { stringStream[static_cast<int>(eShaderType::eVertex)].str(),
		stringStream[static_cast<int>(eShaderType::eFragment)].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	unsigned int ID = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(ID, 1, &src, nullptr);
	glCompileShader(ID);

	//TODO: Error Handling
	int result = 0;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Failed\n";
	}

	return ID;
}

unsigned int Shader::createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	unsigned int program = glCreateProgram();
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);

	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	return program;
}

void Shader::bind() const
{
	glUseProgram(m_rendererID);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

int Shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.cend())
	{
		return m_uniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_rendererID, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: Uniform " << name << " Doesn't exist.\n";
	}

	m_uniformLocationCache[name] = location;
	return location;
}
