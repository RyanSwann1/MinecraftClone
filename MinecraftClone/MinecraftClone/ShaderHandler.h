#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>

enum class eShaderType
{
	Chunk,
	Skybox
};

class ShaderHandler : private NonCopyable, private NonMovable
{
	class Shader : private NonCopyable
	{
	public:
		Shader(eShaderType shaderType);
		Shader(Shader&&) noexcept;
		Shader& operator=(Shader&&) noexcept;
		~Shader();

		unsigned int getID() const;
		eShaderType getType() const;
		int getUniformLocation(const std::string& uniformName);

	private:
		unsigned int ID;
		eShaderType type;
		std::unordered_map<std::string, int> uniformLocations;
	};

public:
	static std::unique_ptr<ShaderHandler> create();

	void setUniformMat4f(eShaderType shaderType, const std::string& uniformName, const glm::mat4& matrix);

	void setUniform1i(eShaderType shaderType, const std::string& uniformName, int value);
	void switchToShader(eShaderType shaderType);

private:
	ShaderHandler();
	
	std::vector<Shader> m_shaders;

	Shader* getShader(eShaderType shaderType);
};