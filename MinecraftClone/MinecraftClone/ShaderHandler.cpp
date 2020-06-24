#include "ShaderHandler.h"
#include "glad.h"
#include "Globals.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace 
{
	const std::string SHADER_DIRECTORY = "OpenGLShaders/";
	constexpr int INVALID_UNIFORM_LOCATION = -1;

	bool parseShaderFromFile(const std::string& filePath, std::string& shaderSource)
	{
		std::ifstream stream(filePath);
		if (!stream.is_open())
		{
			return false;
		}
		std::string line;
		std::stringstream stringStream;

		while (getline(stream, line))
		{
			stringStream << line << "\n";
			std::cout << line << "\n";
		}

		shaderSource = stringStream.str();
		stream.close();
		stringStream.clear();
		
		return true;
	}

	bool createShaderProgram(unsigned int shaderID, const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
		std::string vertexShaderSource;
		if (!parseShaderFromFile(SHADER_DIRECTORY + vertexShaderFilePath, vertexShaderSource))
		{
			return false;
		}
		std::string fragmentShaderSource;
		if (!parseShaderFromFile(SHADER_DIRECTORY + fragmentShaderFilePath, fragmentShaderSource))
		{
			return false;
		}

		//Create Vertex Shader
		unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		const char* vertexSrc = vertexShaderSource.c_str();
		glShaderSource(vertexShaderID, 1, &vertexSrc, nullptr);
		glCompileShader(vertexShaderID);

		int vertexShaderResult = 0;
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &vertexShaderResult);
		if (vertexShaderResult == GL_FALSE)
		{
			int messageLength = 0;
			glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &messageLength);
			char* errorMessage = static_cast<char*>(alloca(messageLength * sizeof(char)));
			glGetShaderInfoLog(vertexShaderID, messageLength, &messageLength, errorMessage);
			std::cout << "Failed to compile: " << errorMessage << "\n";
			
			return false;
		}

		//Create Fragment Shader
		unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragSrc = fragmentShaderSource.c_str();
		glShaderSource(fragmentShaderID, 1, &fragSrc, nullptr);
		glCompileShader(fragmentShaderID);

		int fragmentShaderResult = 0;
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &fragmentShaderResult);
		if (fragmentShaderResult == GL_FALSE)
		{
			int messageLength = 0;
			glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &messageLength);
			char* errorMessage = static_cast<char*>(alloca(messageLength * sizeof(char)));
			glGetShaderInfoLog(fragmentShaderID, messageLength, &messageLength, errorMessage);
			std::cout << "Failed to compile: " << errorMessage << "\n";
			
			return false;
		}

		glAttachShader(shaderID, vertexShaderID);
		glAttachShader(shaderID, fragmentShaderID);
		glLinkProgram(shaderID);
		glValidateProgram(shaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		return true;
	}
}

ShaderHandler::ShaderHandler()
{}

std::unique_ptr<ShaderHandler> ShaderHandler::create()
{
	std::unique_ptr<ShaderHandler> shaderHandler = std::unique_ptr<ShaderHandler>(new ShaderHandler());
	bool shaderLoaded = false;
	for (const auto& shader : shaderHandler->m_shader)
	{
		switch (shader.getType())
		{
		case eShaderType::Chunk :
			shaderLoaded = createShaderProgram(shader.getID(), "ChunkVertexShader.glsl", "ChunkFragmentShader.glsl");
			break;
		case eShaderType::Skybox :
			shaderLoaded = createShaderProgram(shader.getID(), "SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
			break;
		case eShaderType::UI :
			shaderLoaded = createShaderProgram(shader.getID(), "UIVertexShader.glsl", "UIFragmentShader.glsl");
			break;
		default:
			assert(false);
		}
	}

	assert(shaderLoaded);
	return (shaderLoaded ? std::move(shaderHandler) : std::unique_ptr<ShaderHandler>());
}

void ShaderHandler::setUniformMat4f(eShaderType shaderType, const std::string& uniformName, const glm::mat4& matrix)
{
	int uniformLocation = m_shader[static_cast<int>(shaderType)].getUniformLocation(uniformName);
	assert(uniformLocation != INVALID_UNIFORM_LOCATION);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderHandler::setUniform1i(eShaderType shaderType, const std::string& uniformName, int value)
{
	int uniformLocation = m_shader[static_cast<int>(shaderType)].getUniformLocation(uniformName);
	assert(uniformLocation != INVALID_UNIFORM_LOCATION);
	glUniform1i(uniformLocation, value);
}

void ShaderHandler::switchToShader(eShaderType shaderType)
{
	glUseProgram(m_shader[static_cast<int>(shaderType)].getID());
}
ShaderHandler::Shader::Shader(eShaderType shaderType)
	: m_ID(glCreateProgram()),
	m_type(shaderType),
	m_uniformLocations()
{}

ShaderHandler::Shader::Shader(Shader&& orig) noexcept
	: m_ID(orig.m_ID),
	m_type(orig.m_type),
	m_uniformLocations(std::move(orig.m_uniformLocations))
{
	orig.m_ID = Globals::INVALID_OPENGL_ID;
}

ShaderHandler::Shader& ShaderHandler::Shader::operator=(Shader&& orig) noexcept
{
	m_ID = orig.m_ID;
	m_type = orig.m_type;
	m_uniformLocations = std::move(orig.m_uniformLocations);

	orig.m_ID = Globals::INVALID_OPENGL_ID;

	return *this;
}

ShaderHandler::Shader::~Shader()
{
	if (m_ID != Globals::INVALID_OPENGL_ID)
	{
		glDeleteProgram(m_ID);
	}
}

unsigned int ShaderHandler::Shader::getID() const
{
	return m_ID;
}

eShaderType ShaderHandler::Shader::getType() const
{
	return m_type;
}

int ShaderHandler::Shader::getUniformLocation(const std::string& uniformName)
{
	if (m_uniformLocations.find(uniformName) != m_uniformLocations.cend())
	{
		return m_uniformLocations[uniformName];
	}
	else
	{
		int location = glGetUniformLocation(m_ID, uniformName.c_str());
		if (location == -1)
		{
			std::cout << "Failed to find uniform: " << uniformName << "\n";
		}
		else
		{
			m_uniformLocations[uniformName] = location;
		}

		return location;
	}
}