#include "Texture.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkGenerator.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "CubeMap.h"
#include "glm/gtc/noise.hpp"
#include "ShaderHandler.h"
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

bool loadTextures(TextureArray & textureArray)
{
	if (!textureArray.addTexture("grass.png") ||
		!textureArray.addTexture("grass_side.png") ||
		!textureArray.addTexture("sand.png") ||
		!textureArray.addTexture("stone.png") ||
		!textureArray.addTexture("water.png") ||
		!textureArray.addTexture("log.png") ||
		!textureArray.addTexture("logtop.png") ||
		!textureArray.addTexture("leaves.png") ||
		!textureArray.addTexture("common_cactus_side.png") ||
		!textureArray.addTexture("common_dead_shrub.png") ||
		!textureArray.addTexture("common_tall_grass.png") ||
		!textureArray.addTexture("error.png"))
	{
		return false;
	}

	return true;
}

//https://www.reddit.com/r/VoxelGameDev/comments/376vmv/how_do_you_implement_threading_in_your_game/

//View == Frustrum
//The reason why it can get away with that is because of other rendering optimizations.
//Try refraining from rendering any blocks that are not adjacent to air(less bandwidth), 
//buildingand optimizing meshes from chunk data(less vertices to draw), 
//or cutting down on the size of each vertex element(less bandwidth).There are plenty of other ways to increase performance with such an engine.

//https://devtalk.nvidia.com/default/topic/720651/opengl/access-violation-in-nvoglv32-dll-how-do-i-track-down-the-problem-/
//https://community.khronos.org/t/how-do-you-implement-texture-arrays/75315


//http://ogldev.atspace.co.uk/index.html

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
	sf::Window window(sf::VideoMode(windowSize.x, windowSize.y), "Minecraft", sf::Style::Default, settings);
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
	shaderHandler->switchToShader(eShaderType::Chunk);

	//unsigned int chunkShaderID = createShaderProgram("ChunkVertexShader.glsl", "ChunkFragmentShader.glsl");
	//unsigned int skyboxShaderID = createShaderProgram("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
	//switchToShader(chunkShaderID);
	Camera camera(Utilities::PLAYER_STARTING_POSITION);

	//std::unique_ptr<CubeMap> cubeMap = CubeMap::create();
	//assert(cubeMap);
	//if (!cubeMap)
	//{
	//	std::cout << "Failed to load cube map\n";
	//	return -1;
	//}

	TextureArray textureArray;
	textureArray.bind();
	bool texturesLoaded = loadTextures(textureArray);
	assert(texturesLoaded);
	if (!loadTextures)
	{
		return -1;
	}
	std::unordered_map<std::string, int> uniformLocations;
	shaderHandler->setUniform1i(eShaderType::Chunk, "uTexture", textureArray.getCurrentSlot());
	//setUniform1i(chunkShaderID, "uTexture", textureArray.getCurrentSlot(), uniformLocations);
	
	glm::vec3 cameraPosition;
	bool movePlayer = false;
	std::atomic<bool> resetGame = false;
	std::mutex renderingMutex;
	std::mutex cameraMutex;
	std::unique_ptr<ChunkGenerator> chunkGenerator = std::make_unique<ChunkGenerator>(camera.m_position);

	cameraPosition = camera.m_position;
	std::thread chunkGenerationThread([&](std::unique_ptr<ChunkGenerator>* chunkGenerator)
		{chunkGenerator->get()->update(std::ref(cameraPosition), std::ref(window), std::ref(resetGame), 
			std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkGenerator );

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	float deltaTime = 0.0f;
	sf::Clock clock;
	clock.restart();
	float messageExpiredTime = 1.0f;
	float elaspedTime = 0.0f;
	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();
		sf::Vector2i mousePosition = sf::Mouse::getPosition();
		camera.mouse_callback(mousePosition.x, mousePosition.y);
		//if (elaspedTime >= messageExpiredTime)
		//{
		//	elaspedTime = 0.0f;
		//	glm::ivec2 playerPosition(camera.m_position.x, camera.m_position.z);
		//	//std::cout << playerPosition.x << "\n";
		//	//std::cout << playerPosition.y << "\n";
		//}

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
			chunkGenerator = std::make_unique<ChunkGenerator>(cameraPosition);

			chunkGenerationThread = std::thread{ [&](std::unique_ptr<ChunkGenerator>* chunkGenerator)
				{chunkGenerator->get()->update(std::ref(cameraPosition), std::ref(window), std::ref(resetGame), 
					std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkGenerator };
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

		//Bitmasking
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
		shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 1000.0f); //1000.0f);//(
		shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);

		if (chunkGenerator)
		{
			std::lock_guard<std::mutex> renderingLock(renderingMutex);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			chunkGenerator->renderOpaque();
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			chunkGenerator->renderTransparent();
			glDisable(GL_BLEND);
		}

		window.display();
	}

	chunkGenerationThread.join();
	return 0;
}