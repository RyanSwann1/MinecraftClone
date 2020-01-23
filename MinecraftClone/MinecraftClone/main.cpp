
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "glad.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(750, 750), "Minecraft", sf::Style::Default);

	std::string gameName("MineCraft");
	std::cout << gameName << "\n";
	
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


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//window.clear(sf::Color::Black);
		window.display();
	}
}
