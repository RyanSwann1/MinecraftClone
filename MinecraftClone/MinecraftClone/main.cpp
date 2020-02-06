
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "glad.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

//https://hacknplan.com/

//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filePath)
{
	enum class eShaderType
	{
		eNone = -1,
		eVertex = 0,
		eFragment = 1,
		eTotal = 2
	};

	std::ifstream stream(filePath);
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
		}
	}

	return { stringStream[static_cast<int>(eShaderType::eVertex)].str(),
		stringStream[static_cast<int>(eShaderType::eFragment)].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source)
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
		int length = 0;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &length);
		//char message[length];
	}

	return ID;
}

static unsigned int createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
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

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be drawn

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Core;
	sf::Window window(sf::VideoMode(750, 750), "Minecraft", sf::Style::Default, settings);
	window.setFramerateLimit(60);
	gladLoadGL();
	glViewport(0, 0, 750, 750);
	
	ShaderProgramSource source = parseShader("Basic.shader");
	std::cout << source.vertexSource << source.fragmentSource << "\n";

	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);
	
	int location = glGetUniformLocation(shader, "uColor");
	glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);
	
	float r = 0.0f;
	float increment = 0.05f;

	//Counter Clock-Wise

	//-- Vertex Attributes
	float positions[] =
	{
		-0.5f, -0.5f, 0.0f, //0
		0.5f, -0.5f, 0.0f, //1
		0.5f,  0.5f, 0.0f, //2
		-0.5f, 0.5f, 0.0f, //3
	};

	float textCoords[] =
	{
		0.0f, 0.0f, //Lower Left Corner
		1.0f, 0.0f, //Lower Right Corner
		0.5f, 1.0f // Top Center
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	//-- Vertex Attributes

	VertexArray vertexArray;
	VertexBuffer vertexBuffer(positions, 4 * 3 * sizeof(float));

	VertexBufferLayout layout;
	layout.push<float>(3);

	vertexArray.addBuffer(vertexBuffer, layout);

	IndexBuffer indexBuffer(indices, 6);
	vertexArray.unbind();
	while (window.isOpen())
	{
		sf::Event sfmlEvent;
		if (window.pollEvent(sfmlEvent))
		{
			if (sfmlEvent.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	
		glClear(GL_COLOR_BUFFER_BIT);
		
		//Draw Calls
		vertexArray.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		window.display();
	}

	glDeleteProgram(shader);
}