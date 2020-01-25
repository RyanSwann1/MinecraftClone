
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "glad.h"

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


int main()
{
	sf::RenderWindow window(sf::VideoMode(750, 750), "Minecraft", sf::Style::Default);
	
	while (window.isOpen())
	{
		gladLoadGL();
		sf::Event sfmlEvent;
		if (window.pollEvent(sfmlEvent))
		{
			if (sfmlEvent.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		
		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};

		//Shaders
		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
		glCompileShader(vertexShader);



		//Create Buffer
		unsigned int VBO;
		glGenBuffers(1, &VBO);

		//Bind the buffer - only one buffer can be bound at a time
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//Fill it with stuff
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//window.clear(sf::Color::Black);
		window.display();
	}
}