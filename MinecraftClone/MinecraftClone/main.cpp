#include "TextureArray.h"
#include "Player.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "VertexBuffer.h"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "SkyBox.h"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderHandler.h"
#include "Frustum.h"
#include "Gui.h"
#include "Pickup.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <unordered_map>
#include <thread>

#include "BoundingBox.h"

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

//Good OpenGL Tutorials
//https://ahbejarano.gitbook.io/lwjglgamedev/chapter12

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
	glm::ivec2 windowSize(1980, 1080);
	sf::Window window(sf::VideoMode(windowSize.x, windowSize.y), "Minecraft Clone", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);
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

	std::unique_ptr<Texture> widjetsTexture = Texture::create("widgets.png");
	assert(widjetsTexture);
	if (!widjetsTexture)
	{
		std::cout << "Couldn't load widjets texture\n";
		return -1;
	}

	textureArray->bind();

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSize.x),
		static_cast<float>(windowSize.y), 0.0f, -1.0f, 1.0f);

	shaderHandler->switchToShader(eShaderType::Chunk);
	shaderHandler->setUniform1i(eShaderType::Chunk, "uTexture", 0);

	shaderHandler->switchToShader(eShaderType::Pickup);
	shaderHandler->setUniform1i(eShaderType::Pickup, "uTexture", 0);

	shaderHandler->switchToShader(eShaderType::Skybox);
	shaderHandler->setUniform1i(eShaderType::Skybox, "uSkyboxTexture", 1);
	
	shaderHandler->switchToShader(eShaderType::UIItem);
	shaderHandler->setUniform1i(eShaderType::UIItem, "uTexture", 0);
	shaderHandler->setUniformMat4f(eShaderType::UIItem, "uProjection", projection);

	shaderHandler->switchToShader(eShaderType::UIToolbar);
	shaderHandler->setUniform1i(eShaderType::UIToolbar, "uTexture", 1);
	shaderHandler->setUniformMat4f(eShaderType::UIToolbar, "uProjection", projection);
	
	std::unique_ptr<ChunkManager> chunkManager = std::make_unique<ChunkManager>();
	Gui gui;
	std::vector<Pickup> pickUps;
	Frustum frustum;
	Player player;
	std::atomic<bool> resetGame = false;
	std::mutex renderingMutex;
	std::mutex playerMutex;

	std::thread chunkGenerationThread([&](std::unique_ptr<ChunkManager>* chunkGenerator)
		{chunkGenerator->get()->update(std::ref(player), std::ref(window), std::ref(resetGame), 
			std::ref(playerMutex), std::ref(renderingMutex)); }, &chunkManager );

	player.spawn(*chunkManager, playerMutex);

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	float deltaTime = 0.0f;
	sf::Clock deltaClock;
	deltaClock.restart();

	while (window.isOpen())
	{
		deltaTime = deltaClock.restart().asSeconds();

		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
			if (currentSFMLEvent.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				{
					resetGame = true;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.close();
				}
			}

			assert(chunkManager);
			player.handleInputEvents(pickUps, currentSFMLEvent, *chunkManager, playerMutex, window, gui);
		}

		assert(chunkManager);
		player.update(deltaTime, playerMutex, *chunkManager.get());
		glm::ivec3 startingPosition = Globals::getClosestMiddlePosition(player.getPosition());
		Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Globals::VISIBILITY_DISTANCE);

		if (resetGame)
		{
			chunkGenerationThread.join();
			resetGame = false;
			chunkManager.reset();
			chunkManager = std::make_unique<ChunkManager>();

			chunkGenerationThread = std::thread{[&](std::unique_ptr<ChunkManager>* chunkManager)
				{chunkManager->get()->update(std::ref(player), std::ref(window), std::ref(resetGame), 
					std::ref(playerMutex), std::ref(renderingMutex)); }, &chunkManager };
		
			player.spawn(*chunkManager, playerMutex);
		}

		glm::mat4 view = glm::lookAt(player.getPosition(), player.getPosition() + player.getCamera().front, player.getCamera().up);
		glm::mat4 projection = glm::perspective(glm::radians(player.getCamera().FOV),
			static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), player.getCamera().nearPlaneDistance, player.getCamera().farPlaneDistance);

		for (auto pickup = pickUps.begin(); pickup != pickUps.end();)
		{
			if (pickup->isInReachOfPlayer(player.getPosition()) || !visibilityRect.contains(pickup->getAABB()))
			{
				player.addToInventory(pickup->getCubeType(), gui);
				pickup = pickUps.erase(pickup);
			}
			else
			{
				pickup->update(player.getPosition(), deltaTime, *chunkManager);
				++pickup;
			}
		}

		frustum.update(projection * view);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
			
			shaderHandler->switchToShader(eShaderType::Pickup);
			shaderHandler->setUniformMat4f(eShaderType::Pickup, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Pickup, "uProjection", projection);
			for (auto& pickUp : pickUps)
			{
				pickUp.render(frustum, *shaderHandler);
			}

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
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

		//Draw GUI
		{	
			shaderHandler->switchToShader(eShaderType::UIItem);
			gui.renderItems(*shaderHandler);
		
			shaderHandler->switchToShader(eShaderType::UIToolbar);
			widjetsTexture->bind();
			//gui.renderToolbar(*shaderHandler, windowSize);
			//gui.renderSelectionBox(*shaderHandler, windowSize, player.getInventory().getSelectedHotbarItem());
			textureArray->bind();
		}

		window.display();
	}

	chunkGenerationThread.join();
	return 0;
}