
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>


#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
	
	Shader shader("Basic.shader");
	shader.bind();
	
	shader.setUniform4f("uColor", 0.8f, 0.3f, 0.8f, 1.0f);
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
}