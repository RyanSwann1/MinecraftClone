#include "ShaderHandler.h"
#include "glad.h"
#include "Utilities.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace 
{
	const std::string SHADER_DIRECTORY = "OpenGLShaders/";

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
	: m_shaders()
{
	m_shaders.emplace_back(eShaderType::Chunk);
	m_shaders.emplace_back(eShaderType::Skybox);
}

ShaderHandler::Shader* ShaderHandler::getShader(eShaderType shaderType)
{
	auto shader = std::find_if(m_shaders.begin(), m_shaders.end(), [shaderType](const auto& shader)
	{
		return shader.type == shaderType;
	});

	return (shader != m_shaders.end() ? &(*shader) : nullptr);
}

std::unique_ptr<ShaderHandler> ShaderHandler::create()
{
	std::unique_ptr<ShaderHandler> shaderHandler = std::unique_ptr<ShaderHandler>(new ShaderHandler());
	for (const auto& shader : shaderHandler->m_shaders)
	{
		switch (shader.type)
		{
		case eShaderType::Chunk :
		{
			bool shaderLoaded = createShaderProgram(shader.ID, "ChunkVertexShader.glsl", "ChunkFragmentShader.glsl");
			assert(shaderLoaded);
			if (!shaderLoaded)
			{
				return std::unique_ptr<ShaderHandler>();
			}
		}
			break;
		case eShaderType::Skybox :
		{
			bool shaderLoaded = createShaderProgram(shader.ID, "SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
			assert(shaderLoaded);
			if (!shaderLoaded)
			{
				return std::unique_ptr<ShaderHandler>();
			}
		}
			break;
		default:
			assert(false);
		}
	}

	return shaderHandler;
}

void ShaderHandler::setUniformMat4f(eShaderType shaderType, const std::string& uniformName, const glm::mat4& matrix)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		glUniformMatrix4fv(shader->getUniformLocation(uniformName), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	else
	{
		std::cout << "Couldn't find shader." + uniformName;
	}
}

void ShaderHandler::setUniform1i(eShaderType shaderType, const std::string& uniformName, int value)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		glUniform1i(shader->getUniformLocation(uniformName), value);
	}
	else
	{
		std::cout << "Couldn't find shader." + uniformName;
	}
}

void ShaderHandler::switchToShader(eShaderType shaderType)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		glUseProgram(shader->ID);
	}
	else
	{
		std::cout << "Couldn't find shader.\n";
	}
}
ShaderHandler::Shader::Shader(eShaderType shaderType)
	: ID(glCreateProgram()),
	type(shaderType),
	uniformLocations()
{}

ShaderHandler::Shader::Shader(Shader&& orig) noexcept
	: ID(orig.ID),
	type(orig.type),
	uniformLocations(std::move(orig.uniformLocations))
{
	orig.ID = Utilities::INVALID_OPENGL_ID;
}

ShaderHandler::Shader& ShaderHandler::Shader::operator=(Shader&& orig) noexcept
{
	ID = orig.ID;
	type = orig.type;
	uniformLocations = std::move(orig.uniformLocations);

	orig.ID = Utilities::INVALID_OPENGL_ID;

	return *this;
}

ShaderHandler::Shader::~Shader()
{
	if (ID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteProgram(ID);
	}
}

unsigned int ShaderHandler::Shader::getUniformLocation(const std::string& uniformName)
{
	if (uniformLocations.find(uniformName) != uniformLocations.cend())
	{
		return uniformLocations[uniformName];
	}
	else
	{
		int location = glGetUniformLocation(ID, uniformName.c_str());
		if (location == -1)
		{
			std::cout << "Failed to find uniform: " << uniformName << "\n";
		}
		else
		{
			uniformLocations[uniformName] = location;
		}

		return location;
	}
}