#include "Texture.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "glm/gtc/noise.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <unordered_map>
#include <thread>

#ifdef DEBUG_GL_ERRORS
#define glCheck(expr) expr; if(!GLAD_GL_KHR_debug){ glCheckError(__FILE__, __LINE__, #expr) };
#else
#define glCheck(call) call
#endif
#define DEBUG_GL_ERRORS

//https://hacknplan.com/
//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/
//https://www.youtube.com/watch?v=vuK8CYzDnBk&list=PLMZ_9w2XRxiZq1vfw1lrpCMRDufe2MKV_&index=17

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be drawns

int getUniformLocation(unsigned int shaderID, const std::string& uniformName, std::unordered_map<std::string, int>& uniformLocations)
{
	if (uniformLocations.find(uniformName) != uniformLocations.cend())
	{
		return uniformLocations[uniformName];
	}
	else
	{
		int location = glGetUniformLocation(shaderID, uniformName.c_str());
		if (location == -1)
		{
			std::cout << "Failed to find uniform: " << uniformName << "\n";
		}
		else
		{
			uniformLocations[uniformName] = location;
		}

		return location;
	}
}

void setUniformLocationVec2(unsigned int shaderID, const std::string& uniformName, glm::vec2 position, std::unordered_map<std::string, int>& uniformLocations)
{
	glUniform2f(getUniformLocation(shaderID, uniformName, uniformLocations), position.x, position.y);
}

void setUniformLocation1f(unsigned int shaderID, const std::string& uniformName, float value, std::unordered_map<std::string, int>& uniformLocations)
{
	glUniform1f(getUniformLocation(shaderID, uniformName, uniformLocations), value);
}

void setUniformMat4f(unsigned int shaderID, const std::string& uniformName, glm::mat4 matrix, std::unordered_map<std::string, int>& uniformLocations)
{
	glUniformMatrix4fv(getUniformLocation(shaderID, uniformName, uniformLocations), 1, GL_FALSE, glm::value_ptr(matrix));
}

void setUniform1i(unsigned int shaderID, const std::string& uniformName, int value, std::unordered_map<std::string, int>& uniformLocations)
{
	glUniform1i(getUniformLocation(shaderID, uniformName, uniformLocations), value);
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

//https://www.reddit.com/r/VoxelGameDev/comments/376vmv/how_do_you_implement_threading_in_your_game/

//View == Frustrum
//The reason why it can get away with that is because of other rendering optimizations.
//Try refraining from rendering any blocks that are not adjacent to air(less bandwidth), 
//buildingand optimizing meshes from chunk data(less vertices to draw), 
//or cutting down on the size of each vertex element(less bandwidth).There are plenty of other ways to increase performance with such an engine.

//https://devtalk.nvidia.com/default/topic/720651/opengl/access-violation-in-nvoglv32-dll-how-do-i-track-down-the-problem-/
//https://community.khronos.org/t/how-do-you-implement-texture-arrays/75315


//x + (y * width)
int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Core;
	sf::Vector2i windowSize(1280, 1080);
	sf::Window window(sf::VideoMode(windowSize.x, windowSize.y), "Minecraft", sf::Style::Default, settings);
	window.setFramerateLimit(120);
	gladLoadGL();

	glCheck(glViewport(0, 0, windowSize.x, windowSize.y));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int shaderID = createShaderProgram();
	Camera camera(glm::vec3(0.0f, 150.f, 0.0f));
	std::unique_ptr<Texture> texture = Texture::loadTexture("Atlas3.png");
	if (!texture)
	{
		std::cout << "couldn't load texture: " << "America.jpg" << "\n";
	}
	std::unordered_map<std::string, int> uniformLocations;

	texture->bind();
	setUniform1i(shaderID, "uTexture", texture->getCurrentSlot(), uniformLocations);

	std::unordered_map<glm::ivec3, VertexArray> VAOs;
	Rectangle visibilityRect(glm::vec2(camera.m_position.x, camera.m_position.z), Utilities::VISIBILITY_DISTANCE);

	ChunkManager chunkManager;
	chunkManager.generateInitialChunks(camera.m_position, VAOs, *texture);
	std::thread chunkGenerationThread([&](ChunkManager* chunkManager) {chunkManager->update(std::ref(visibilityRect), std::ref(VAOs), std::ref(camera),
		*texture, std::ref(window)); }, &chunkManager);
	std::mutex mutex;

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	sf::Clock clock;
	clock.restart();
	float messageExpiredTime = 1.0f;
	float elaspedTime = 0.0f;
	setUniformLocation1f(shaderID, "uAlpha", 1.0f, uniformLocations);
	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();
		sf::Vector2i mousePosition = sf::Mouse::getPosition();
		camera.mouse_callback(mousePosition.x, mousePosition.y);
		if (elaspedTime >= messageExpiredTime)
		{
			elaspedTime = 0.0f;
			glm::ivec2 playerPosition(camera.m_position.x, camera.m_position.z);
			//std::cout << playerPosition.x << "\n";
			//std::cout << playerPosition.y << "\n";
		}

		elaspedTime += deltaTime;

		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
			if (currentSFMLEvent.type == sf::Event::KeyPressed)
			{
				std::lock_guard<std::mutex> lock(mutex);
				camera.move(currentSFMLEvent, deltaTime);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
		setUniformMat4f(shaderID, "uView", view, uniformLocations);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 500.f);
		setUniformMat4f(shaderID, "uProjection", projection, uniformLocations);

		for (auto VAO = VAOs.begin(); VAO != VAOs.end();)
		{
			if (VAO->second.m_destroy)
			{
				VAO->second.destroy();
				VAO = VAOs.erase(VAO);
			}
			else if (VAO->second.m_attachOpaqueVBO)
			{
				VAO->second.attachOpaqueVBO();
				++VAO;
			}
			else
			{
				++VAO;
			}
		}

		for (const auto& VAO : VAOs)
		{
			if (VAO.second.m_opaqueVBODisplayable)
			{
				VAO.second.bindOpaqueVAO();
				glDrawElements(GL_TRIANGLES, VAO.second.m_vertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
				VAO.second.unbind();
			}
		}

		for (auto VAO = VAOs.begin(); VAO != VAOs.end(); ++VAO)
		{
			if (VAO->second.m_attachTransparentVBO)
			{
				VAO->second.attachTransparentVBO();
			}
		}

		setUniformLocation1f(shaderID, "uAlpha", Utilities::WATER_ALPHA_VALUE, uniformLocations);
		for (const auto& VAO : VAOs)
		{
			if (VAO.second.m_transparentVBODisplayable)
			{
				VAO.second.bindTransparentVAO();
				glDrawElements(GL_TRIANGLES, VAO.second.m_vertexBuffer.transparentIndicies.size(), GL_UNSIGNED_INT, nullptr);
				VAO.second.unbind();
			}
		}
		setUniformLocation1f(shaderID, "uAlpha", 1.0f, uniformLocations);

		window.display();
	}

	chunkGenerationThread.join();
	return 0;
}