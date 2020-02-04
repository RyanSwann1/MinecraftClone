
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "glad.h"

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
	sf::RenderWindow window(sf::VideoMode(750, 750), "Minecraft", sf::Style::Default);
	gladLoadGL();
	glViewport(0, 0, 750, 750);

	std::string vertexShaderSource =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec3 position;"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position.x, position.y, position.z, 1.0);"
		"}\n";
	std::string fragmentShaderSource =
		"#version 330 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

	unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);
	glUseProgram(shader);

	float vertices[9] =
	{
		//Vertex Attributes
		-0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	////Create Buffer
	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	////Bind the buffer - only one buffer can be bound at a time
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //Bind == Select
	////Fill it with stuff
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), &vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

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

		window.clear();
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//window.clear(sf::Color::Black);
		window.display();
	}

	glDeleteProgram(shader);
}