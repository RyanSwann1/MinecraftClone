
#include "glad.h"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <array>

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

void parseShaderFromFile(const std::string& filePath, std::string& vertexShaderSource, std::string& fragmentShaderSource)
{
	std::ifstream stream(filePath);
	std::string line;
	std::array<std::stringstream, 2> stringStream;
	int stringStreamIndex = 0;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("Vertex") != std::string::npos)
			{
				stringStreamIndex = 0;
			}
			else if (line.find("Fragment") != std::string::npos)
			{
				stringStreamIndex = 1;
			}
		}
		else
		{
			stringStream[stringStreamIndex] << line << "\n";
			std::cout << line << "\n";
		}
	}

	vertexShaderSource = stringStream[0].str();
	fragmentShaderSource = stringStream[1].str();
}

unsigned int createShaderProgram(const std::string& filePath)
{
	unsigned int shaderID = glCreateProgram();
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	parseShaderFromFile(filePath, vertexShaderSource, fragmentShaderSource);

	//Create Vertex Shader
	unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char* src = vertexShaderSource.c_str();
	glShaderSource(vertexShaderID, 1, &src, nullptr);
	glCompileShader(vertexShaderID);

	//Create Fragment Shader
	unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragSrc = vertexShaderSource.c_str();
	glShaderSource(fragmentShaderID, 1, &fragSrc, nullptr);

	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	glValidateProgram(shaderID);

	glUseProgram(shaderID);

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

	//unsigned int shaderID = createShaderProgram("Basic.shader");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	std::array<float, 12> positions
	{
		-0.5f, -0.5f, 0.0f, //0
		0.5f, -0.5f, 0.0f, //1
		0.5f, 0.5f, 0.0f, //2
		-0.5f, 0.5f, 0.0f //3
	};

	std::array<unsigned int, 6> indicies
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int positionVBO;
	glGenBuffers(GL_ARRAY_BUFFER, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, positions.size(), GL_FLOAT, GL_FALSE, 3, 0);

	unsigned int indiciesVBO;
	glGenBuffers(GL_ELEMENT_ARRAY_BUFFER, &indiciesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(float), indicies.data(), GL_STATIC_DRAW);

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

		glDrawElements(GL_ELEMENT_ARRAY_BUFFER, indicies.size(), GL_UNSIGNED_INT, nullptr);

		window.display();
	}

	return 0;
}