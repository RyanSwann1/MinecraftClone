#include "TextureArray.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "SkyBox.h"
#include "glm/gtc/noise.hpp"
#include "ShaderHandler.h"
#include "Frustum.h"
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

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be 

//https://www.reddit.com/r/VoxelGameDev/comments/376vmv/how_do_you_implement_threading_in_your_game/

//View == Frustrum
//The reason why it can get away with that is because of other rendering optimizations.
//Try refraining from rendering any blocks that are not adjacent to air(less bandwidth), 
//buildingand optimizing meshes from chunk data(less vertices to draw), 
//or cutting down on the size of each vertex element(less bandwidth).There are plenty of other ways to increase performance with such an engine.

//https://devtalk.nvidia.com/default/topic/720651/opengl/access-violation-in-nvoglv32-dll-how-do-i-track-down-the-problem-/
//https://community.khronos.org/t/how-do-you-implement-texture-arrays/75315

//http://ogldev.atspace.co.uk/index.html

void keepMouseWithinWindow(const sf::Vector2u& windowSize, sf::Vector2i& mousePosition)
{
	if (mousePosition.x < 0)
	{
		mousePosition.x = 0;
	}
	else if (mousePosition.x > windowSize.x)
	{
		mousePosition.x = windowSize.x;
	}

	if (mousePosition.y < 0)
	{
		mousePosition.y = 0;
	}
	else if (mousePosition.y > windowSize.y)
	{
		mousePosition.y = windowSize.y;
	}
}

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
	sf::Vector2i windowSize(1980, 1080);
	sf::Window window(sf::VideoMode(windowSize.x, windowSize.y), "Minecraft Clone", sf::Style::Default, settings);
	window.setFramerateLimit(60);
	gladLoadGL();

	glCheck(glViewport(0, 0, windowSize.x, windowSize.y));
	glEnable(GL_DEPTH_TEST);

	std::unique_ptr<ShaderHandler> shaderHandler = ShaderHandler::create();
	assert(shaderHandler);
	if (!shaderHandler)
	{
		std::cout << "Unable to load shader handler\n";
		return -1;
	}

	std::unique_ptr<TextureArray> textureArray = TextureArray::create();
	assert(textureArray);
	if (!textureArray)
	{
		std::cout << "Failed to load textures\n";
		return -1;
	}
	textureArray->bind();

	std::unique_ptr<SkyBox> skybox = SkyBox::create();
	assert(skybox);
	if (!skybox)
	{
		std::cout << "Failed to load Skybox\n";
		return -1;
	}

	shaderHandler->setUniform1i(eShaderType::Skybox, "uSkyboxTexture", 0);
	shaderHandler->setUniform1i(eShaderType::Chunk, "uTexture", 0);

	Frustum frustum;
	Camera camera(Utilities::PLAYER_STARTING_POSITION);
	glm::vec3 cameraPosition;
	cameraPosition = camera.m_position;
	bool movePlayer = false;
	std::atomic<bool> resetGame = false;
	std::mutex renderingMutex;
	std::mutex cameraMutex;
	std::unique_ptr<ChunkManager> chunkManager = std::make_unique<ChunkManager>(camera.m_position);

	std::thread chunkGenerationThread([&](std::unique_ptr<ChunkManager>* chunkGenerator)
		{chunkGenerator->get()->update(std::ref(cameraPosition), std::ref(window), std::ref(resetGame), 
			std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkManager );

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	float deltaTime = 0.0f;
	int frames = 0;
	sf::Clock deltaClock;
	sf::Clock gameClock;
	float lastTime = gameClock.getElapsedTime().asSeconds();
	deltaClock.restart();
	float messageExpiredTime = 1.0f;
	float elaspedTime = 0.0f;
	while (window.isOpen())
	{
		deltaTime = deltaClock.restart().asSeconds();
		++frames;
		if (gameClock.getElapsedTime().asSeconds() - lastTime >= 1.0f)
		{
			//std::cout << 1000.0f / frames << "\n";
			frames = 0;
			lastTime += 1.0f;
		}

		sf::Vector2i mousePosition = sf::Mouse::getPosition();

		camera.mouse_callback(mousePosition.x, mousePosition.y);

		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (currentSFMLEvent.KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				{
					resetGame = true;
				}
			}
		}

		if (resetGame)
		{
			chunkGenerationThread.join();
			resetGame = false;
			camera.m_position = Utilities::PLAYER_STARTING_POSITION;
			cameraPosition = camera.m_position;
			chunkManager.reset();
			chunkManager = std::make_unique<ChunkManager>(cameraPosition);

			chunkGenerationThread = std::thread{ [&](std::unique_ptr<ChunkManager>* chunkManager)
				{chunkManager->get()->update(std::ref(cameraPosition), std::ref(window), std::ref(resetGame), 
					std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkManager };
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			camera.move(deltaTime);

			if (cameraMutex.try_lock())
			{
				cameraPosition = camera.m_position;
				cameraMutex.unlock();
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 1750.0f);

		frustum.update(projection * view);
		
		if (chunkManager)
		{
			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
			//Draw Scene
			std::lock_guard<std::mutex> renderingLock(renderingMutex);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			chunkManager->renderOpaque(frustum);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			chunkManager->renderTransparent(frustum);
			glDisable(GL_BLEND);

			//Draw Skybox
			glDepthFunc(GL_LEQUAL);
			shaderHandler->switchToShader(eShaderType::Skybox);
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uView", glm::mat4(glm::mat3(view)));
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uProjection", projection);

			skybox->render();
			glDepthFunc(GL_LESS);
		}

		window.display();
	}

	chunkGenerationThread.join();
	return 0;
}