#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
public:
	Shader(const std::string& filePath);
	~Shader();

	void bind() const;
	void unbind() const;

	//Set Uniforms
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	unsigned int m_rendererID;
	const std::string m_filePath;
	std::unordered_map<std::string, int> m_uniformLocationCache;

	ShaderProgramSource parseShader();
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	unsigned int getUniformLocation(const std::string& name);
};