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
	Pickup,
	Skybox,
	UIItem,
	UIToolbar,
	UIFont,
	Max = UIFont
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
		unsigned int m_itemID;
		eShaderType m_type;
		std::unordered_map<std::string, int> m_uniformLocations;
	};

public:
	static std::unique_ptr<ShaderHandler> create(const glm::ivec2& windowSize);

	void setUniformMat4f(eShaderType shaderType, const std::string& uniformName, const glm::mat4& matrix);
	void setUniform1i(eShaderType shaderType, const std::string& uniformName, int value);
	void switchToShader(eShaderType shaderType);

private:
	ShaderHandler();
	eShaderType m_currentShaderType;
	
	std::array<Shader, static_cast<int>(eShaderType::Max) + 1> m_shader =
	{
		eShaderType::Chunk,
		eShaderType::Pickup,
		eShaderType::Skybox,
		eShaderType::UIItem,
		eShaderType::UIToolbar,
		eShaderType::UIFont
	};
};