#include "glad.h"
#include "Texture.h"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>

//#include "VertexBufferLayout.h"
//#include "VertexBuffer.h"
//#include "IndexBuffer.h"
//#include "VertexArray.h"
//#include "Shader.h"
//#include "Renderer.h"
//#include "Texture.h"

//https://hacknplan.com/
//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be drawn

//int main()
//{
//	sf::ContextSettings settings;
//	settings.depthBits = 24;
//	settings.stencilBits = 8;
//	settings.antialiasingLevel = 4;
//	settings.majorVersion = 3;
//	settings.minorVersion = 3;
//	settings.attributeFlags = sf::ContextSettings::Core;
//	sf::Window window(sf::VideoMode(750, 750), "Minecraft", sf::Style::Default, settings);
//	window.setFramerateLimit(60);
//	gladLoadGL();
//
//	Shader shader("Basic.shader");
//	shader.bind();
//	
//	float r = 0.0f;
//	float increment = 0.05f;
//
//	Counter Clock-Wise
//
//	-- Vertex Attributes
//	float positions[] =
//	{
//		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//0
//		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //1
//		0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//2
//		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f//3
//	};
//
//	float textCoords[] =
//	{
//		0.0f, 0.0f, //Lower Left Corner
//		1.0f, 0.0f, //Lower Right Corner
//		0.5f, 1.0f // Top Center
//	};
//
//	unsigned int indices[] =
//	{
//		0, 1, 2,
//		2, 3, 0
//	};
//	-- Vertex Attributes
//
//	VertexArray vertexArray;
//	VertexBuffer vertexBuffer(positions, 4 * 5 * sizeof(float));
//
//	VertexBufferLayout layout;
//	layout.push<float>(3);
//	layout.push<float>(2);
//
//	vertexArray.addBuffer(vertexBuffer, layout);
//
//	IndexBuffer indexBuffer(indices, 6);
//	Renderer renderer;
//
//	uniform vec4 uColor;
//	uniform sampler2D uTexture;
//
//	Texture texture("America.jpg");
//	texture.bind();
//
//	shader.setUniform4f("uColor", 0.8f, 0.3f, 0.8f, 1.0f);
//	shader.setUniform1i("uTexture", 0);
//	
//	vertexArray.unbind();
//	shader.unbind();
//	indexBuffer.unbind();
//	vertexBuffer.unbind();
//
//	while (window.isOpen())
//	{
//		sf::Event sfmlEvent;
//		if (window.pollEvent(sfmlEvent))
//		{
//			if (sfmlEvent.type == sf::Event::Closed)
//			{
//				window.close();
//			}
//		}
//	
//		renderer.clear();
//		
//		Draw Calls
//		renderer.draw(vertexArray, indexBuffer, shader);
//
//		window.display();
//	}
//}

int getUniformLocation(unsigned int shaderID, const std::string& uniformName)
{
	int location = glGetUniformLocation(shaderID, uniformName.c_str());
	if (location == -1)
	{
		std::cout << "Failed to find uniform: " << uniformName << "\n";
		return location;
	}

	return location;
}

void setUniform1i(unsigned int shaderID, const std::string& uniformName, int value)
{
	glUniform1i(getUniformLocation(shaderID, uniformName), value);
}

void parseShaderFromFile(const std::string& filePath, std::string& shaderSource)
{
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream stringStream;

	while (getline(stream, line))
	{
		stringStream << line << "\n";
		std::cout << line << "\n";
	}

	shaderSource = stringStream.str();
}

unsigned int createShaderProgram()
{
	unsigned int shaderID = glCreateProgram();

	std::string vertexShaderSource;
	parseShaderFromFile("VertexShader.shader", vertexShaderSource);
	std::string fragmentShaderSource;
	parseShaderFromFile("FragmentShader.shader", fragmentShaderSource);

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
	}

	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	glValidateProgram(shaderID);

	glUseProgram(shaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	
	return shaderID;
}

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
	unsigned int shaderID = createShaderProgram();
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	std::unique_ptr<Texture> texture = Texture::loadTexture("America.jpg");
	if (!texture)
	{
		std::cout << "couldn't load texture: " << "America.jpg" << "\n";
		return -1;
	}

	texture->bind();
	setUniform1i(shaderID, "uTexture", texture->getCurrentSlot());

	std::array<float, 12> positions
	{
		-0.5f, -0.5f, 0.0f,  // bottom left
		0.5f, -0.5f, 0.0f,  // bottom right
		0.5f,  0.5f, 0.0f,  // top right
		-0.5f, 0.5f, 0.0f   // top left 
	};

	std::array<unsigned int, 6> indicies
	{
		0, 1, 2,
		2, 3, 0
	};

	std::array<float, 8> textCoords
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int positionVBO;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	unsigned int textCoordsVBO;
	glGenBuffers(1, &textCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(float), textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

	unsigned int indiciesVBO;
	glGenBuffers(1, &indiciesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	while (window.isOpen())
	{
		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, nullptr);
		window.display();
	}

	glDeleteBuffers(1, &positionVBO);
	glDeleteBuffers(1, &indiciesVBO);
	glDeleteVertexArrays(1, &VAO);

	return 0;
}