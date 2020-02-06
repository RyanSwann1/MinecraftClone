
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "glad.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/

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
	float vertices[] =
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

	glTexParameteri()

	//Index Buffer
	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	////Create Buffer
	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	////Bind the buffer - only one buffer can be bound at a time
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //Bind == Select
	////Fill it with stuff
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), &vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindVertexArray(vao);
	while (window.isOpen())
	{
		//sf::Event sfmlEvent;
		//if (window.pollEvent(sfmlEvent))
		//{
		//	if (sfmlEvent.type == sf::Event::Closed)
		//	{
		//		window.close();
		//	}
		//}
	
		//window.clear();
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		

		//Draw Calls
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//window.clear(sf::Color::Black);
		window.display();
	}

	glDeleteProgram(shader);
}