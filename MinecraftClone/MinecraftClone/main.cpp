#include "Texture.h"
#include "Camera.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>

//https://hacknplan.com/
//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be drawns

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

void setUniformMat4f(unsigned int shaderID, const std::string& uniformName, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(shaderID, uniformName), 1, GL_FALSE, glm::value_ptr(matrix));
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

//View == Frustrum
//The reason why it can get away with that is because of other rendering optimizations.
//Try refraining from rendering any blocks that are not adjacent to air(less bandwidth), 
//buildingand optimizing meshes from chunk data(less vertices to draw), 
//or cutting down on the size of each vertex element(less bandwidth).There are plenty of other ways to increase performance with such an engine.

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Core;
	sf::Vector2i windowSize(750, 750);
	sf::Window window(sf::VideoMode(windowSize.x, windowSize.y), "Minecraft", sf::Style::Default, settings);
	window.setFramerateLimit(60);
	gladLoadGL();

	glViewport(0, 0, windowSize.x, windowSize.y);
	glEnable(GL_DEPTH_TEST);
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

	Camera camera;

	texture->bind();
	setUniform1i(shaderID, "uTexture", texture->getCurrentSlot());
	
	const std::array<GLfloat, 12> FRONT_FACE = { 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };

	std::array<float, 180> vertices =
	{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		//x & y
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,


		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	std::array<unsigned int, 36> indicies
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		19, 20, 16,

		21, 22, 23,
		24, 25, 21
	};

	std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> chunk;
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; z++)
			{
				chunk[x][y][z] = glm::vec3(x, y, z);
			}
		}
	}

	//std::array<float, 12> positions
	//{
	//	-1.0, -1.0, 0.0f,  // bottom left
	//	1.0, -1.0, 0.0f,  // bottom right
	//	1.0,  1.0, 0.0f,  // top right
	//	-1.0, 1.0, 0.0f   // top left 
	//};



	std::array<float, 8> textCoords
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int verticesVBO;
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

	//unsigned int textCoordsVBO;
	//glGenBuffers(1, &textCoordsVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
	//glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(float), textCoords.data(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

	unsigned int indiciesVBO;
	glGenBuffers(1, &indiciesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	sf::Clock clock;
	clock.restart();

	while (window.isOpen())
	{
		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (currentSFMLEvent.type == sf::Event::KeyPressed)
			{
				camera.move(currentSFMLEvent, clock.restart().asSeconds());
			}
		}

		sf::Vector2i mousePosition = sf::Mouse::getPosition();
		camera.mouse_callback(mousePosition.x, mousePosition.y);

		glBindVertexArray(VAO);

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		//
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		setUniformMat4f(shaderID, "uModel", model);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
		setUniformMat4f(shaderID, "uView", view);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 100.f);
		setUniformMat4f(shaderID, "uProjection", projection);

		//glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//for (int x = 0; x < 16; ++x)
		//{
		//	for (int y = 0; y < 16; ++y)
		//	{
		//		for (int z = 0; z < 16; z++)
		//		{
		//			glm::mat4 model = glm::translate(glm::mat4(1.0f), chunk[x][y][z]);
		//			setUniformMat4f(shaderID, "uModel", model);	
		//			glDrawArrays(GL_TRIANGLES, 0, 36);
		//		}
		//	}
		//}
		
		glBindVertexArray(0);
		window.display();
	}

	glDeleteBuffers(1, &verticesVBO);
	glDeleteBuffers(1, &indiciesVBO);
	glDeleteVertexArrays(1, &VAO);

	return 0;
}

//std::array<float, 120> positions =
//{
//	//Back Face
//	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
//	1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
//	1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
//	-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
//
//	//Front Face
//	-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
//	1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
//	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
//
//	//Left Face
//	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
//	-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
//	-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
//
//	//Right Face
//	1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
//	1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
//	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	1.0f, -1.0f, 1.0f, 0.0f, 1.0f
//
//	//Bottom Face
//   - 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
//   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
//   1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
//   -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
//
//	//Top Face
//	-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
//	1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
//	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
//};