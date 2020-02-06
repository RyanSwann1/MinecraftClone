
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

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

	Shader shader("Basic.shader");
	shader.bind();
	
	float r = 0.0f;
	float increment = 0.05f;

	//Counter Clock-Wise

	//-- Vertex Attributes
	float positions[] =
	{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//0
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //1
		0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//2
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f//3
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
	VertexBuffer vertexBuffer(positions, 4 * 5 * sizeof(float));

	VertexBufferLayout layout;
	layout.push<float>(3);
	layout.push<float>(2);

	vertexArray.addBuffer(vertexBuffer, layout);

	IndexBuffer indexBuffer(indices, 6);
	Renderer renderer;

	//uniform vec4 uColor;
	//uniform sampler2D uTexture;

	Texture texture("America.jpg");
	texture.bind();

	shader.setUniform4f("uColor", 0.8f, 0.3f, 0.8f, 1.0f);
	//shader.setUniform1i("uTexture", 0);
	
	//vertexArray.unbind();
	//shader.unbind();
	//indexBuffer.unbind();
	//vertexBuffer.unbind();

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
	
		renderer.clear();
		
		//Draw Calls
		renderer.draw(vertexArray, indexBuffer, shader);

		window.display();
	}
}