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
	: m_shaders()
{
	for (int i = 0; i <= static_cast<int>(eShaderType::Max); ++i)
	{
		m_shaders.emplace_back(static_cast<eShaderType>(i));
	}
}

ShaderHandler::Shader* ShaderHandler::getShader(eShaderType shaderType)
{
	auto shader = std::find_if(m_shaders.begin(), m_shaders.end(), [shaderType](const auto& shader)
	{
		return shader.getType() == shaderType;
	});

	return (shader != m_shaders.end() ? &(*shader) : nullptr);
}

std::unique_ptr<ShaderHandler> ShaderHandler::create()
{
	std::unique_ptr<ShaderHandler> shaderHandler = std::unique_ptr<ShaderHandler>(new ShaderHandler());
	for (const auto& shader : shaderHandler->m_shaders)
	{
		switch (shader.getType())
		{
		case eShaderType::Chunk :
		{
			bool shaderLoaded = createShaderProgram(shader.getID(), "ChunkVertexShader.glsl", "ChunkFragmentShader.glsl");
			assert(shaderLoaded);
			if (!shaderLoaded)
			{
				return std::unique_ptr<ShaderHandler>();
			}
		}
			break;
		case eShaderType::Skybox :
		{
			bool shaderLoaded = createShaderProgram(shader.getID(), "SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
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
		int uniformLocation = shader->getUniformLocation(uniformName);
		assert(uniformLocation != INVALID_UNIFORM_LOCATION);
		if (uniformLocation != INVALID_UNIFORM_LOCATION)
		{
			glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		else
		{
			std::cout << "Invalid uniform location\n";
		}
	}
	else
	{
		std::cout << "Couldn't find shader." + uniformName << "\n";
	}
}

void ShaderHandler::setUniformVec3(eShaderType shaderType, const std::string& uniformName, const glm::vec3& v)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		int uniformLocation = shader->getUniformLocation(uniformName);
		assert(uniformLocation != INVALID_UNIFORM_LOCATION);
		if (uniformLocation != INVALID_UNIFORM_LOCATION)
		{
			glUniform3fv(uniformLocation, 1, glm::value_ptr(v));
		}
		else
		{
			std::cout << "Unable to find uniform location\n";
		}

	}
	else
	{
		std::cout << "Couldn't find shader." + uniformName << "\n";
	}
}

void ShaderHandler::setUniform1i(eShaderType shaderType, const std::string& uniformName, int value)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		int uniformLocation = shader->getUniformLocation(uniformName);
		assert(uniformLocation != INVALID_UNIFORM_LOCATION);
		if (uniformLocation != INVALID_UNIFORM_LOCATION)
		{
			glUniform1i(uniformLocation, value);
		}
		else
		{
			std::cout << "Unable to find uniform location\n";
		}
		
	}
	else
	{
		std::cout << "Couldn't find shader." + uniformName << "\n";
	}
}

void ShaderHandler::switchToShader(eShaderType shaderType)
{
	Shader* shader = getShader(shaderType);
	assert(shader);
	if (shader)
	{
		glUseProgram(shader->getID());
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

unsigned int ShaderHandler::Shader::getID() const
{
	return ID;
}

eShaderType ShaderHandler::Shader::getType() const
{
	return type;
}

int ShaderHandler::Shader::getUniformLocation(const std::string& uniformName)
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